/***********************************************************************
 *
 *	file: config.h
 *
 *	$Id: config.h 171 2007-08-23 07:23:16Z chapuni $
 *
 */

#ifndef CONFIG_H__
#define CONFIG_H__

/* 力の限りスレッドを起動 */
#define USE_MT 1

#define USE_DT 0

/* シーケンシャルサーチ用閾値
   2004版に基づくものでは 3 */
#define KEY_SHUFFLE_POS	4

#include "mtytypes.h"

/****************************************************************
 *
 *	USE_MMX ... i686-mmx
 *
 *	P6, K7 以前のアーキテクチャではこれを利用すべし。
 *
 */

#if defined(USE_MMX)

#define N_STRIDE 6
typedef uint64_t	WS_T;
typedef uint32_t	ALU_T;
typedef int8_t		SALTOFS_T;

#define REQUIRED_CAPS	(CPUID_CMOV | CPUID_MMX)

#define CRYPT64_CLOBBER	/* "%ecx", "%edx", */

/****************************************************************
 *
 *	USE_64 ... x64-alu
 *
 *	AMD64 K8(Opteron, Athlon64)では、XMMよりもALUの方が速い。
 *	64-bit ALU に対して 3-issue.
 *
 */

#elif defined(USE_64)	/* 64-bit ALU */

#define N_STRIDE 6
typedef uint64_t	WS_T;
typedef uint64_t	ALU_T;
typedef int8_t		SALTOFS_T;

#define REQUIRED_CAPS	(CPUID_CMOV)

#define CRYPT64_CLOBBER /* "%rcx", "%rdx",*/ "%r8", "%r9",

/****************************************************************
 *
 *	USE_64_XMM ... x64-xmm
 *
 *	Core2 Microarchitecture は、
 *	128-bit XMM 演算を効率よく処理できる。
 *
 */

#elif defined(USE_64_XMM)

#define N_STRIDE 7
typedef DQWORD_T	WS_T;
typedef uint64_t	ALU_T;
typedef int32_t		SALTOFS_T;

#define REQUIRED_CAPS	(CPUID_CMOV | CPUID_SSE | CPUID_SSE2)

#define CRYPT64_CLOBBER /* "%rcx", "%rdx",*/
				   
/****************************************************************
 *
 *	USE_XMM ... default
 *
 *	SSE2対応と言われているアーキテクチャでは妥当。
 *
 */

#else	/* XMM */

#define N_STRIDE 7
typedef DQWORD_T	WS_T;
typedef uint32_t	ALU_T;
typedef int32_t		SALTOFS_T;

#define REQUIRED_CAPS	(CPUID_CMOV | CPUID_MMX | CPUID_SSE | CPUID_SSE2)

#define CRYPT64_CLOBBER /* "%ecx", "%edx", */

#endif

#include <limits.h>
#define ALU_BITS	(CHAR_BIT * sizeof(ALU_T))

#endif /* CONFIG_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
