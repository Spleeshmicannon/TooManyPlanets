#pragma once
// using __forceinline only with compatible compiler
#ifdef _MSC_VER
#define INLINE __forceinline
#else
#define INLINE static inline
#endif