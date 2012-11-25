/***********************************************************************
 *
 *	file: config.h
 *
 *	$Id: config.h 171 2007-08-23 07:23:16Z chapuni $
 *
 */

#ifndef CONFIG_H__
#define CONFIG_H__

/* �͂̌���X���b�h���N�� */
#define USE_MT 1

#define USE_DT 0

/* �V�[�P���V�����T�[�`�p臒l
   2004�łɊ�Â����̂ł� 3 */
#define KEY_SHUFFLE_POS	4

#include "mtytypes.h"

/****************************************************************
 *
 *	USE_MMX ... i686-mmx
 *
 *	P6, K7 �ȑO�̃A�[�L�e�N�`���ł͂���𗘗p���ׂ��B
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
 *	AMD64 K8(Opteron, Athlon64)�ł́AXMM����ALU�̕��������B
 *	64-bit ALU �ɑ΂��� 3-issue.
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
 *	Core2 Microarchitecture �́A
 *	128-bit XMM ���Z�������悭�����ł���B
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
 *	SSE2�Ή��ƌ����Ă���A�[�L�e�N�`���ł͑Ó��B
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
