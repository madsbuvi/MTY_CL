/***********************************************************************
 *
 *	file: mtytypes.h
 *
 */

#ifndef MTYTYPES_H__
#define MTYTYPES_H__

#define C64	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

typedef signed char CODE_T;

#ifdef __GNUC__

#define NORETURN void __attribute__((noreturn))

#include <inttypes.h>
#define ALIGN_PREFIX(n)
#define ALIGN_SUFFIX(n) __attribute__ ((aligned(n)))
#ifdef __SSE__
typedef unsigned DQWORD_T __attribute__ ((mode(V4SI)));
#endif

#else

#define NORETURN __declspec(noreturn) void

#include <intrin.h>
#include <xmmintrin.h>
#define ALIGN_PREFIX(n) __declspec(align(16))
#define ALIGN_SUFFIX(n)

/* inttypes.h
   MSVC environment is undefined */

typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

typedef __m128 DQWORD_T;

#endif

/* CPUID capabilities */
#define CPUID_FPU	(1 << 0)
#define CPUID_CMOV	(1 << 15)
#define CPUID_MMX	(1 << 23)
#define CPUID_SSE	(1 << 25)
#define CPUID_SSE2	(1 << 26)

#endif /* MTYTYPES_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
