/* -*- mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
// vim:sts=4:sw=4:ts=4:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s

/* fflas/fflas_pfgemv.inl
 *
 * ========LICENCE========
 * This file is part of the library FFLAS-FFPACK.
 *
 * FFLAS-FFPACK is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 *.
 */



namespace FFLAS
{


	template<class Field, class AlgoT, class FieldTrait>
	typename Field::Element_ptr
	pfgemv(const Field& F,
		   const FFLAS_TRANSPOSE ta,
		   const size_t m,
		   const size_t n,
		   const typename Field::Element alpha,
		   const typename Field::ConstElement_ptr A, const size_t lda,
		   const typename Field::ConstElement_ptr X, const size_t incX,
		   const typename Field::Element beta,
		   typename Field::Element_ptr Y, const size_t incY, 
		   MMHelper<Field, AlgoT, FieldTrait, ParSeqHelper::Parallel<CuttingStrategy::Recursive, StrategyParameter::Threads> > & H){
		
		if (m<2){
			PAR_BLOCK{
				SYNCH_GROUP(
							TASK(CONSTREFERENCE(F) MODE( READ(A,X) READWRITE(Y)),
								 {fgemv(F, ta,  m, n,  alpha, A, lda, X, incX, beta, Y, incY);}
								 );
							);
			}
		}else{			
			typedef MMHelper<Field,AlgoT,FieldTrait,ParSeqHelper::Parallel<CuttingStrategy::Recursive, StrategyParameter::Threads> > MMH_t;
			MMH_t H1(H);
			MMH_t H2(H);
			size_t M2= m>>1;
			PAR_BLOCK{
				
				H1.parseq.set_numthreads(H1.parseq.numthreads() >> 1);
				H2.parseq.set_numthreads(H.parseq.numthreads() - H1.parseq.numthreads());
				
				typename Field::ConstElement_ptr A1= A;
				typename Field::ConstElement_ptr A2= A+(M2+1)*lda;
				//typename Field::ConstElement_ptr B1= X;
				//typename Field::ConstElement_ptr B2= X+(M2+1)*incX;
				typename Field::Element_ptr C1= Y;
				typename Field::Element_ptr C2= Y+(M2+1)*incY;
				
				// 2 multiply (1 split on dimension m)						
				TASK(CONSTREFERENCE(F) MODE( READ(A1,X) READWRITE(C1)),
					 {pfgemv( F, ta,  M2, n, alpha, A1, lda, X, incX, beta, C1, incY, H1);}
					 );
				
				TASK(MODE(CONSTREFERENCE(F) READ(A2,X) READWRITE(C2)),
					 {pfgemv(F, ta, m-M2, n, alpha, A2, lda, X, incX, beta, C2, incY, H2);}
					 );
			}
			
		}
		return Y;		
		
	}


	template<class Field, class AlgoT, class FieldTrait>
	typename Field::Element_ptr
	pfgemv(const Field& F,
		   const FFLAS_TRANSPOSE ta,
		   const size_t m,
		   const size_t n,
		   const typename Field::Element alpha,
		   const typename Field::ConstElement_ptr A, const size_t lda,
		   const typename Field::ConstElement_ptr X, const size_t incX,
		   const typename Field::Element beta,
		   typename Field::Element_ptr Y, const size_t incY,
		   MMHelper<Field, AlgoT, FieldTrait, ParSeqHelper::Parallel<CuttingStrategy::Row, StrategyParameter::Threads> > & H){
		
		typedef MMHelper<Field,AlgoT,FieldTrait,ParSeqHelper::Parallel<CuttingStrategy::Row, StrategyParameter::Threads> > MMH_t;
		MMH_t HH(H);
		size_t BS = m/(H.parseq.numthreads());
		size_t Niter = std::max(m, H.parseq.numthreads());
		if(m<Niter){
			PAR_BLOCK{
				
				HH.parseq.set_numthreads(H.parseq.numthreads());
				using FFLAS::CuttingStrategy::Row;
				using FFLAS::StrategyParameter::Threads;				
				//typename Field::ConstElement_ptr AA = A+((ta==FFLAS::FflasTrans)?1:lda);
				//typename Field::Element_ptr YY = Y+(m-1)*incY;
				//size_t m = H.parseq.numthreads();
				FORBLOCK1D(iter, Niter,	SPLITTER(BS,Row,Threads),				
						   TASK(CONSTREFERENCE(F) MODE( READ(A1,X) READWRITE(YY)),
								{fgemv( F, ta, 1, n, alpha, A + iter.blockindex()*lda, lda, X, incX, beta, Y, incY);}
								);
						   }
		            );
		}else{
			PAR_BLOCK{
				
				HH.parseq.set_numthreads(H.parseq.numthreads());
				using FFLAS::CuttingStrategy::Row;
				using FFLAS::StrategyParameter::Threads;				
				//typename Field::ConstElement_ptr AA = A+((ta==FFLAS::FflasTrans)?1:lda);
				//typename Field::Element_ptr YY = Y+(m-1)*incY;
				size_t m = H.parseq.numthreads();
				FORBLOCK1D(iter, Niter,	SPLITTER(BS,Row,Threads),				
						   TASK(CONSTREFERENCE(F) MODE( READ(A1,X) READWRITE(YY)),
								{fgemv( F, ta, 1, n, alpha, A + iter.blockindex()*m*lda, lda, X, incX, beta, Y, incY);}
								);
						   }
		            );

		}

		return Y;		
			
	}
	

	////////////////////////////////////////////////////////////////////////////////////////

	template<class Field, class AlgoT, class FieldTrait>
	typename Field::Element_ptr
	pfgemv(const Field& F,
		   const FFLAS_TRANSPOSE ta,
		   const size_t m,
		   const size_t n,
		   const typename Field::Element alpha,
		   const typename Field::ConstElement_ptr A, const size_t lda,
		   const typename Field::ConstElement_ptr X, const size_t incX,
		   const typename Field::Element beta,
		   typename Field::Element_ptr Y, const size_t incY,
		   size_t BS,
		   MMHelper<Field, AlgoT, FieldTrait, ParSeqHelper::Parallel<CuttingStrategy::Row, StrategyParameter::Grain> > & H){

		typedef MMHelper<Field,AlgoT,FieldTrait,ParSeqHelper::Parallel<CuttingStrategy::Row, StrategyParameter::Grain> > MMH_t;
		MMH_t HH(H);
		if(BS<H.parseq.numthreads()) BS = m/(H.parseq.numthreads());
		size_t Niter = std::max(m, H.parseq.numthreads());
		if(m<Niter){
			PAR_BLOCK{
				
				HH.parseq.set_numthreads(H.parseq.numthreads());
				using FFLAS::CuttingStrategy::Row;
				using FFLAS::StrategyParameter::Grain;				
				//typename Field::ConstElement_ptr AA = A+((ta==FFLAS::FflasTrans)?1:lda);
				//typename Field::Element_ptr YY = Y+(m-1)*incY;
				//size_t m = H.parseq.numthreads();
				FORBLOCK1D(iter, Niter,	SPLITTER(BS,Row,Grain),				
						   TASK(CONSTREFERENCE(F) MODE( READ(A1,X) READWRITE(YY)),
								{fgemv( F, ta, 1, n, alpha, A + iter.blockindex()*lda, lda, X, incX, beta, Y, incY);}
								);
						   }
		            );
		}else{
			PAR_BLOCK{
				
				HH.parseq.set_numthreads(H.parseq.numthreads());
				using FFLAS::CuttingStrategy::Row;
				using FFLAS::StrategyParameter::Grain;				
				//typename Field::ConstElement_ptr AA = A+((ta==FFLAS::FflasTrans)?1:lda);
				//typename Field::Element_ptr YY = Y+(m-1)*incY;
				
				FORBLOCK1D(iter, Niter,	SPLITTER(BS,Row,Grain),				
						   TASK(CONSTREFERENCE(F) MODE( READ(A1,X) READWRITE(YY)),
								{fgemv( F, ta, 1, n, alpha, A + iter.blockindex()*BS*lda, lda, X, incX, beta, Y, incY);}
								);
						   }
		            );

		}

		return Y;		
			
	}



	
} // FFLAS