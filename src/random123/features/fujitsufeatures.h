/*
Note: Minimum/Initial version derived from openclfeatures.h to work
with fujitsu compiler (_FCC).
*/

#ifndef __fujitsufeatures_dot_hpp
#define __fujitsufeatures_dot_hpp

#ifndef R123_STATIC_INLINE
#define R123_STATIC_INLINE static __inline
#endif

#ifndef R123_FORCE_INLINE
#define R123_FORCE_INLINE(decl) decl
#endif

#ifndef R123_CUDA_DEVICE
#define R123_CUDA_DEVICE
#endif

#ifndef R123_ASSERT
#include <assert.h>
#define R123_ASSERT(x) assert(x)
#endif

#ifndef R123_BUILTIN_EXPECT
#define R123_BUILTIN_EXPECT(expr,likely) expr
#endif

#ifndef R123_USE_WMMINTRIN_H
#define R123_USE_WMMINTRIN_H 0
#endif

#ifndef R123_USE_INTRIN_H
#define R123_USE_INTRIN_H 0
#endif

#ifndef R123_USE_MULHILO32_ASM
#define R123_USE_MULHILO32_ASM 0
#endif

#ifndef R123_USE_MULHILO64_ASM
#define R123_USE_MULHILO64_ASM 0
#endif

#ifndef R123_USE_MULHILO64_MSVC_INTRIN
#define R123_USE_MULHILO64_MSVC_INTRIN 0
#endif

#ifndef R123_USE_MULHILO64_CUDA_INTRIN
#define R123_USE_MULHILO64_CUDA_INTRIN 0
#endif

#ifndef R123_USE_MULHILO64_OPENCL_INTRIN
#define R123_USE_MULHILO64_OPENCL_INTRIN 0
#endif

#ifndef R123_USE_MULHILO64_MULHI_INTRIN
#if (defined(__powerpc64__))
#define R123_USE_MULHILO64_MULHI_INTRIN 1
#else
#define R123_USE_MULHILO64_MULHI_INTRIN 0
#endif
#endif

#ifndef R123_MULHILO64_MULHI_INTRIN
#define R123_MULHILO64_MULHI_INTRIN __mulhdu
#endif

#ifndef R123_USE_MULHILO32_MULHI_INTRIN
#define R123_USE_MULHILO32_MULHI_INTRIN 0
#endif

#ifndef R123_MULHILO32_MULHI_INTRIN
#define R123_MULHILO32_MULHI_INTRIN __mulhwu
#endif

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#include <stdint.h>
#ifndef UINT64_C
#error UINT64_C not defined.  You must define __STDC_CONSTANT_MACROS before you #include <stdint.h>
#endif

#endif
