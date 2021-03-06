#include "givaro//modular-balanced.h"
#include "givaro//modular.h"
#include "fflas-ffpack/ffpack/ffpack.h"

#define PASTER(x,y) x ## _ ## y
#define EVALUATOR(x,y)  PASTER(x,y)
#define NAME(fun) EVALUATOR(fun, FFLAS_TYPE)

#if FFLAS_FIELD == Modular
 #define FFLAS_POSITIVE true
#else
 #define FFLAS_POSITIVE false
#endif

 namespace FFPACK{
template <>
size_t ColumnEchelonForm (const  Givaro::FFLAS_FIELD<FFLAS_TYPE>& F, const size_t M, const size_t N,
			  FFLAS_TYPE* A, const size_t lda,
			  size_t* P, size_t* Qt, bool transform,
			  const FFPACK::FFPACK_LU_TAG LuTag){
    return NAME(ColumnEchelonForm_modular) (F.cardinality(), M, N, A, lda, P, Qt, transform, LuTag, FFLAS_POSITIVE);
}
template <>
size_t RowEchelonForm (const Givaro::FFLAS_FIELD<FFLAS_TYPE>& F, const size_t M, const size_t N,
			       FFLAS_TYPE* A, const size_t lda,
			       size_t* P, size_t* Qt, bool transform,
			       const FFPACK::FFPACK_LU_TAG LuTag){
    return NAME(RowEchelonForm_modular) (F.cardinality(), M, N, A, lda, P, Qt, transform, LuTag, FFLAS_POSITIVE);
}
template <>
size_t ReducedColumnEchelonForm (const Givaro::FFLAS_FIELD<FFLAS_TYPE>& F, const size_t M, const size_t N,
			  FFLAS_TYPE* A, const size_t lda,
			  size_t* P, size_t* Qt, bool transform,
			  const FFPACK::FFPACK_LU_TAG LuTag){
    return NAME(ReducedColumnEchelonForm_modular) (F.cardinality(), M, N, A, lda, P, Qt, transform, LuTag, FFLAS_POSITIVE);
}
template <>
size_t ReducedRowEchelonForm (const Givaro::FFLAS_FIELD<FFLAS_TYPE>& F, const size_t M, const size_t N,
			       FFLAS_TYPE* A, const size_t lda,
			       size_t* P, size_t* Qt, bool transform,
			       const FFPACK::FFPACK_LU_TAG LuTag){
    return NAME(ReducedRowEchelonForm_modular) (F.cardinality(), M, N, A, lda, P, Qt, transform, LuTag, FFLAS_POSITIVE);
}
}

#undef FFLAS_POSITIVE
#undef PASTER
#undef EVALUATOR
#undef NAME
