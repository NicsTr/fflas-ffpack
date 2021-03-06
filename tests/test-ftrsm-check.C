/* -*- mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
// vim:sts=4:sw=4:ts=4:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s

/*
 * Copyright (C) 2015 the FFLAS-FFPACK group
 * Written by Ashley Lesdalons <Ashley.Lesdalons@e.ujf-grenoble.fr>
 *
 * This file is Free Software and part of FFLAS-FFPACK.
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
 *
 */

//--------------------------------------------------------------------------
//          Test for Checker_ftrsm
//--------------------------------------------------------------------------
#define ENABLE_ALL_CHECKINGS 1

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "fflas-ffpack/fflas-ffpack.h"
#include "fflas-ffpack/utils/args-parser.h"
#include "fflas-ffpack/utils/test-utils.h"
#include "fflas-ffpack/utils/fflas_io.h"
using namespace FFLAS;

int main(int argc, char** argv) {
	typedef Givaro::Modular<double> Field;
	Givaro::Integer q = 131071;
	size_t iter = 3;
	size_t MAXN = 100;
	uint64_t seed = getSeed();

	Argument as[] = {
		{ 'q', "-q Q", "Set the field characteristic (-1 for random).", TYPE_INTEGER , &q },
		{ 'i', "-i R", "Set number of repetitions.", TYPE_INT , &iter },
		{ 'n', "-n N", "Set the size of the matrix.", TYPE_INT , &MAXN },
		{ 's', "-s N", "Set the seed.", TYPE_UINT64 , &seed },
		END_OF_ARGUMENTS
	};
	FFLAS::parseArguments(argc,argv,as);	
	Field F(q);
	srandom(seed);

	typename Field::Element alpha,tmp;
	Field::RandIter Rand(F,0,seed);
	Field::NonZeroRandIter NZRand(Rand);

	size_t pass = 0;
	for (size_t i=0; i<iter; ++i) {

		size_t m = random() % MAXN + 1;
		size_t n = random() % MAXN + 1;
		std::cout << "m= " << m << "    n= " << n << "\n";
		Rand.random(alpha);
		FFLAS::FFLAS_SIDE side = rand()%2?FFLAS::FflasLeft:FFLAS::FflasRight;
		FFLAS::FFLAS_UPLO uplo = rand()%2?FFLAS::FflasLower:FFLAS::FflasUpper;
		FFLAS::FFLAS_TRANSPOSE trans = rand()%2?FFLAS::FflasNoTrans:FFLAS::FflasTrans;
		FFLAS::FFLAS_DIAG diag = rand()%2?FFLAS::FflasNonUnit:FFLAS::FflasUnit;
		size_t k = (side==FFLAS::FflasLeft?m:n);
		//std::cout << alpha << "  " << side << "  " << uplo << "  " << trans << "  " << diag << "  \n";

		Field::Element_ptr X = FFLAS::fflas_new(F,m,n);
		Field::Element_ptr A = FFLAS::fflas_new(F,k,k);

		PAR_BLOCK { FFLAS::pfrand(F,Rand, m,n,X,m/MAX_THREADS); }
		//FFLAS::WriteMatrix (std::cerr<<"X:=",F,m,n,X,n,FflasMaple) <<std::endl;

		for (size_t i=0;i<k;++i){
			for (size_t j=0;j<i;++j)
				A[i*k+j]= (uplo == FFLAS::FflasLower)? Rand.random(tmp) : F.zero;
			A[i*k+i]= (diag == FFLAS::FflasNonUnit)? NZRand.random(tmp) : F.one;
			for (size_t j=i+1;j<k;++j)
				A[i*k+j]= (uplo == FFLAS::FflasUpper)? Rand.random(tmp) : F.zero;
		}
		//FFLAS::WriteMatrix (std::cerr<<"A:=",F,k,k,A,k,FflasMaple) <<std::endl;

		FFLAS::Checker_ftrsm<Field> checker(Rand, m, n, alpha, X, n);
		FFLAS::ftrsm(F, side, uplo, trans, diag, m, n, alpha, A, k, X, n);
		try {
			checker.check(side, uplo, trans, diag, m, n, A, k, X, n);
			std::cout << "Verification successful\n";
			pass++;
		} catch(FailureTrsmCheck &e) {
			std::cout << "Verification failed!\n";
		}

		FFLAS::fflas_delete(X,A);
	}

	std::cout << pass << "/" << iter << " tests were successful.\n";

	return 0;
}
