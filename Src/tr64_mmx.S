/******************************************************-*-fundamental-*-
 *
 *  最終転置+行列転置(64bit MMX)
 *
 *  $Id: tr64_mmx.S 114 2007-04-10 02:22:06Z chapuni $
 *
 */

#if defined(USE_MMX)

#define W	8

#else	/* XMM */

#define W	16

#endif

	.text

#define IC	%eax
#define IK	%ebx

#define MASK	%mm7
#define MC	%mm6

#define IOFS(b,n,i) W*(n)+4*i(b)
#define OOFS(b,n,i) 8*(n)+4*i(b)

#define SW32(l,m,j)	\
	.if (l)<64;	\
	mov	IOFS(%ecx,l,0),%eax;	\
	mov	IOFS(%ecx,l,1),%ebx;	\
	mov	%eax,OOFS(%edx,j,0);	\
	mov	%ebx,OOFS(%edx,j+0x20,0);	\
	.endif;				\
	.if (m)<64;	\
	mov	IOFS(%ecx,m,0),%eax;	\
	mov	IOFS(%ecx,m,1),%ebx;	\
	mov	%eax,OOFS(%edx,j,1);	\
	mov	%ebx,OOFS(%edx,j+0x20,1);	\
	.endif

/* (uint64_t const *ECX, uint64 *EDX) (clobber eax) */
.globl _transpose64
_transpose64:
.globl transpose64
transpose64:
	push	%ebx

	pxor	%mm0,%mm0
	movq	%mm0,OOFS(%edx,0x16,0)
	movq	%mm0,OOFS(%edx,0x16+0x20,0)
	movq	%mm0,OOFS(%edx,0x17,0)
	movq	%mm0,OOFS(%edx,0x17+0x20,0)
	// 初段(コピー)は展開
	SW32(0x0E,0x1B,0x00)
	SW32(0x2E,0x3B,0x01)
	SW32(0x06,0x13,0x02)
	SW32(0x26,0x33,0x03)
	SW32(0x1F,0x1A,0x04)
	SW32(0x3F,0x3A,0x05)
	SW32(0x05,0x12,0x06)
	SW32(0x25,0x32,0x07)
	SW32(0x1E,0x0A,0x08)
	SW32(0x3E,0x2A,0x09)
	SW32(0x16,0x11,0x0A)
	SW32(0x36,0x31,0x0B)
	SW32(0x1D,0x09,0x0C)
	SW32(0x3D,0x29,0x0D)
	SW32(0x15,0x01,0x0E)
	SW32(0x35,0x21,0x0F)
	SW32(0x0D,0x08,0x10)
	SW32(0x2D,0x28,0x11)
	SW32(0x14,0x00,0x12)
	SW32(0x34,0x20,0x13)
	SW32(0x0C,0x19,0x14)
	SW32(0x2C,0x39,0x15)
	SW32(0x04,0x41,0x16)
	SW32(0x24,0x40,0x17)
	SW32(0x0B,0x18,0x18)
	SW32(0x2B,0x38,0x19)
	SW32(0x03,0x10,0x1A)
	SW32(0x23,0x30,0x1B)
	SW32(0x1C,0x44,0x1C)
	SW32(0x3C,0x44,0x1D)
	SW32(0x02,0x44,0x1E)
	SW32(0x22,0x44,0x1F)

	// マスクの用意
	pcmpeqb	MASK,MASK
	mov	$16,IC
	movd	IC,MC
	psllq	$32,MASK
	
LI:
	movq	MASK,%mm0
	psrlq	MC,MASK
	pxor	%mm0,MASK
	lea	(%edx,IC,8),%ecx
	xor	IK,IK
LJ:
	movq	(%edx,IK,8),%mm0
	movq	(%ecx,IK,8),%mm1
	movq	%mm1,%mm2
	psllq	MC,%mm1
	pxor	%mm0,%mm1
	pand	MASK,%mm1
	pxor	%mm1,%mm0
	movq	%mm0,(%edx,IK,8)
	psrlq	MC,%mm1
	pxor	%mm2,%mm1
	movq	%mm1,(%ecx,IK,8)
	lea	1(IK,IC),IK
	or	IC,IK
	xor	IC,IK
	cmp	$64,IK
	jl	LJ

	psrld	$1,MC
	shr	IC
	jne	LI


	emms
	pop	%ebx
	ret

// EOF
