/******************************************************-*-fundamental-*-
 *
 *  $Id: crypt64.S 154 2007-05-01 03:03:37Z chapuni $
 */

#ifndef DEBUG
#define DEBUG 0
#endif

#if defined(USE_MMX)

#define USE_T	1

.if !USE_T	// to suppress code generated
#include "x86-mmx.S"
.endif
#undef K

#define PTR_T	.long

#define W	8
#define O	1
#define R(n)	%mm##n
#define MOV	movq
#define AND	pand
#define ANDN	pandn
#define OR	por
#define XOR	pxor
#define OX	0x00

#define AX	%eax
#define CNT	%ax
#define KSI	%dl
#define PARAM	%edx
#define TX	%ebx
#define K			%ecx
#define T	/* +16 */	%esi
#define LR	/* +16 */	%ebp
#define RL	/* +16 */	%edi
#define H			%ecx

#elif defined(USE_64)

#define USE_T	0

#define PTR_T	.quad

#define W	8
#define O	1
#define R(n)	%r##n
#define MOV	mov
#define	NOT	not
#define AND	and
#define OR	or
#define XOR	xor
#define OX	0x00

#define AX	%rax
#define CNT	4(%rsp)
#define KSI	6(%rsp)
#define PARAM	%rdx
#define TX	%rdx
#define K			%rcx
#define T	/* +16 */	%rsi
#define LR	/* +16 */	%rbp
#define RL	/* +16 */	%rdi
#define H			%rcx

#define WK(n)	W*(n+16+8)(%rsp)

#define r4 rax
#define r5 rbx
#define r6 rdx
#define r7 rsi
#define r0 r8
#define r1 r9
#define r2 r10
#define r3 r11

#elif defined(USE_64_XMM)

#define USE_T	1

.if !USE_T	// to suppress code generated
//#include "x64-sse.S"
.endif
#undef K

#define PTR_T	.quad

#define W	16
#define O	4
#define R(n)	%xmm##n
#define MOV	movdqa
#define AND	pand
#define ANDN	pandn
#define OR	por
#define XOR	pxor
/* disp8 に収まりきれないので、disp32 に固定する必要あり */
#define OX	0x100000

#define AX	%rax
#define CNT	%ax
#define KSI	%dl
#define PARAM	%rdx
#define TX	%rbx
#define K			%rcx
#define T	/* +16 */	%rsi
#define LR	/* +16 */	%rbp
#define RL	/* +16 */	%rdi
#define H			%rcx

#else	/* XMM */

#define USE_T	1	/* TLS が飼い慣らせない… */

.if 0	// to suppress code generated
#include "x86-sse.S"
.endif
#undef K
#undef tmp_at
#ifdef USE_REGPARM
#define tmp_at(n) param64+W*(((n+16)&31)+64)
#else /* USE_REGPARM */
#define tmp_at(n) _param64+W*(((n+16)&31)+64)
#endif /* USE_REGPARM */

#define PTR_T	.long

#define W	16
#define O	4
#define R(n)	%xmm##n
#define MOV	movdqa
#define AND	pand
#define ANDN	pandn
#define OR	por
#define XOR	pxor
/* disp8 に収まりきれないので、disp32 に固定する必要あり */
#define OX	0x100000

#define AX	%eax
#define CNT	%ax
#define KSI	%dl
#define PARAM	%edx
#define TX	%ebx
#define K			%ecx
#define T	/* +16 */	%esi
#define LR	/* +16 */	%ebp
#define RL	/* +16 */	%edi
#define H			%ecx

#endif

#ifndef WK
#if USE_T
#define WK(n)	W*(n)(T)
#endif
#endif

#define LOADI(v,n,d,s)	MOV	W*((s)-16)+OX(LR),R(d);Ln##n:
#define XORK(v,n,d,s)	XOR	W*s(K),R(d)

#define RMOVE(v,n,d,s)	MOV	R(s),R(d)
#define RANDN(v,n,d,s)	ANDN	R(s),R(d)
#define RAND(v,n,d,s)	AND	R(s),R(d)
#define ROR(v,n,d,s)	OR	R(s),R(d)
#define RXOR(v,n,d,s)	XOR	R(s),R(d)

#define NOP
#define RXORN RXOR
#define MXORN MXOR

#define XORR(v,n,d,s)	XOR	W*((s)-16)(RL),R(d)
#define STORR(v,n,d,s)	MOV	R(d),W*((s)-16)(RL)

#define MXORR(v,n,d,s)	XOR	R(d),W*((s)-16)(RL)

#define STORE(v,n,d,o)	MOV	R(d),WK((o)-16)

#define MMOVE(v,n,d,o)	MOV	WK((o)-16),R(d)
#define MXOR(v,n,d,o)	XOR	WK((o)-16),R(d)
#define MOR(v,n,d,o)	OR	WK((o)-16),R(d)
#define MANDN(v,n,d,o)	ANDN	WK((o)-16),R(d)
#define MAND(v,n,d,o)	AND	WK((o)-16),R(d)

#ifdef NOT
#define RNOT(v,n,d,x)	not	R(d)
#else
#define RNOT(v,n,d,x)	XOR	(T),R(d)
#endif

// (ecx, edx)

// ecx k
// edx t+0
// esi t+16
// ebp lr+16
// edi rl+16

.data
.globl debug_variables
debug_variables:
.globl _debug_variables
_debug_variables:
	PTR_T 0
	PTR_T 0
	PTR_T 0
	PTR_T 0
	PTR_T 0
	PTR_T 0
	PTR_T 0
	PTR_T 0
	PTR_T 0
	PTR_T 0
.text
.align 16
.globl crypt64_desc
crypt64_desc:
.globl _crypt64_desc
_crypt64_desc:
	PTR_T	crypt64_pro
	PTR_T	crypt64_crypt
	PTR_T	crypt64_cmp_pro
	PTR_T	crypt64_cmp_ep
	PTR_T	crypt64_ep
	PTR_T	crypt64_ep_end
	.byte	16/W
	.byte	O
	.word	(loe-los)/(2*48)
	
/* disp(%reg) disp へのポインタ */
#define LOF(n)	(Ln##n-crypt64_crypt-O)
los:
	.word	LOF(00),LOF(10),LOF(20),LOF(30),LOF(40),LOF(50)
	.word	LOF(01),LOF(11),LOF(21),LOF(31),LOF(41),LOF(51)
	.word	LOF(02),LOF(12),LOF(22),LOF(32),LOF(42),LOF(52)
	.word	LOF(03),LOF(13),LOF(23),LOF(33),LOF(43),LOF(53)
	.word	LOF(04),LOF(14),LOF(24),LOF(34),LOF(44),LOF(54)
	.word	LOF(05),LOF(15),LOF(25),LOF(35),LOF(45),LOF(55)
	.word	LOF(06),LOF(16),LOF(26),LOF(36),LOF(46),LOF(56)
	.word	LOF(07),LOF(17),LOF(27),LOF(37),LOF(47),LOF(57)
loe:
.align 64

.globl _crypt64
_crypt64:
crypt64_pro:
	push	LR
	push	RL
	push	TX
	push	T
#ifdef USE_64
	push	%rbx
	push	%r12
	push	%r13
	push	%r14
	push	%r15
	sub	$0x110,%rsp
#endif

#if 0&&DEBUG>=1
	rdtsc
#endif
	push	AX	// t0 もしくはダミー

crypt64_crypt:

// 初期値クリア
// メンドくさいからといって memset(3) 使うと悲惨だよ(笑)
	XOR	R(7),R(7)
	mov	$-W*64,AX
clr_lr:
	MOV	R(7),W*64(PARAM,AX)
	add	$W,AX
	jne	clr_lr

	// さいしょは逆で始める。どーせ対称だし。
	lea	W*(16)(PARAM),LR	/* XXX */
	lea	W*(32+16)(PARAM),RL
#ifndef USE_64
	lea	W*(64+16)(PARAM),T
#endif

	movb	$0,KSI
	movw	$16*25,CNT

.align	64
loo:
	add	W*2*56(K),K

#ifdef S1

#if USE_T
#undef tmp_at
#define tmp_at(n) W*(n)(T)
#endif

#undef pnot
#define pnot (T)

#ifdef a6_p
#define S_1(o1,o2,o3,o4) S1(o1,o2,o3,o4,)
#define S_2(o1,o2,o3,o4) S2(o1,o2,o3,o4,)
#define S_3(o1,o2,o3,o4) S3(o1,o2,o3,o4,)
#define S_4(o1,o2,o3,o4) S4(o1,o2,o3,o4,)
#define S_5(o1,o2,o3,o4) S5(o1,o2,o3,o4,)
#define S_6(o1,o2,o3,o4) S6(o1,o2,o3,o4,)
#define S_7(o1,o2,o3,o4) S7(o1,o2,o3,o4,)
#define S_8(o1,o2,o3,o4) S8(o1,o2,o3,o4,)
#else
#define S_1(o1,o2,o3,o4) S1(o1,o2,o3,o4)
#define S_2(o1,o2,o3,o4) S2(o1,o2,o3,o4)
#define S_3(o1,o2,o3,o4) S3(o1,o2,o3,o4)
#define S_4(o1,o2,o3,o4) S4(o1,o2,o3,o4)
#define S_5(o1,o2,o3,o4) S5(o1,o2,o3,o4)
#define S_6(o1,o2,o3,o4) S6(o1,o2,o3,o4)
#define S_7(o1,o2,o3,o4) S7(o1,o2,o3,o4)
#define S_8(o1,o2,o3,o4) S8(o1,o2,o3,o4)
#endif

#undef B
#define B(n) W*(n-16)(RL)

#define EK(e,k1,k2,k3,k4,k5,k6)	\
LOADI(1,0##e,0,(31&(4*(e)-1)));	XORK(1,0##e,0,k1);	\
LOADI(2,1##e,1,(31&(4*(e)+0)));	XORK(2,1##e,1,k2);	\
LOADI(3,2##e,2,(31&(4*(e)+1)));	XORK(3,2##e,2,k3);	\
LOADI(4,3##e,3,(31&(4*(e)+2)));	XORK(4,3##e,3,k4);	\
LOADI(5,4##e,4,(31&(4*(e)+3)));	XORK(5,4##e,4,k5);	\
LOADI(6,5##e,5,(31&(4*(e)+4)));	XORK(6,5##e,5,k6)

/* use John */
EK(0,13,16,10,23,0,4);S_1(B(8),B(16),B(22),B(30))
EK(1,2,27,14,5,20,9);S_2(B(12),B(27),B(1),B(17))
EK(2,22,18,11,3,25,7);S_3(B(23),B(15),B(29),B(5))
EK(3,15,6,26,19,12,1);S_4(B(25),B(19),B(9),B(0))
EK(4,68,79,58,64,74,82);S_5(B(7),B(13),B(24),B(2))
EK(5,57,67,78,72,60,75);S_6(B(3),B(28),B(10),B(18))
EK(6,71,76,66,83,61,80);S_7(B(31),B(11),B(21),B(6))
EK(7,73,69,77,63,56,59);S_8(B(4),B(26),B(14),B(20))

#elif defined(USE_64_XMM)
#include "x64-xmm.inc"
#elif defined(USE_64)
#include "x64-alu.inc"
#else
#include "crypt64.inc"
#endif

	mov	LR,TX
	addb	$(256/16),KSI
	cmovne	RL,LR
	cmovne	TX,RL
	decw	CNT
	jne	loo

// 後段のためにポインタを揃え直す
	lea	W*(32+16)(LR),H

// 一貫性を保つために挿入
// 実際に以下のコードを展開するのはキモチよくないが許せ。
	lea	W*(-16)(RL),PARAM
	lea	W*(-16)(H),T

crypt64_cmp_pro:

#if 0&&DEBUG>=1
// Timing of the comparator
	rdtsc
	push	AX		// t1
#endif
// 以下も省略しようと思えばできるが、そのうち。
// I think he's saying he'd also like to omit this:
	lea	W*(16)(PARAM),LR
	lea	W*(32+16)(PARAM),RL
	lea	W*(64+16)(PARAM),T
	
// The comparator is generated here

crypt64_cmp_ep:

crypt64_ep:

	pop	LR		// t1 or dummy

#if 0&&DEBUG>=1
// aggregate timing
	rdtsc			// t2
	mov	AX,PARAM
	sub	LR,PARAM	// t2 - t1
	mov	LR,AX
	pop	LR		// t0
	sub	LR,AX		// t1 - t0
#if defined(USE_64) || defined(USE_64_XMM)
	shl	$32,PARAM
	and	$0xFFFFFFFF,%rax
	or	PARAM,%rax
#endif
#endif

#ifdef USE_64
	add	$0x110,%rsp
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbx
#endif
	pop	T
	pop	TX
	pop	RL
	pop	LR

	ret

crypt64_ep_end:

//EOF
