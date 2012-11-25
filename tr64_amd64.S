/******************************************************-*-fundamental-*-
 *
 *  最終転置+行列転置(AMD64)
 *
 *  $Id: tr64_amd64.S 114 2007-04-10 02:22:06Z chapuni $
 *
 */

#if defined(USE_64)

#define W	8

#elif defined(USE_64_XMM)

#define W	16

#endif

	.text

#define IC	%rcx
#define ICL	%cl
#define IK	%rbx

#define MASK	%r11

#define IOFS(b,n,i) W*(n)+4*i(b)
#define OOFS(b,n,i) 8*(n)+4*i(b)

#define SW32(l,m,j)	\
	.if (l)<64;	\
	mov	IOFS(%r10,l,0),%rax;	\
	mov	%eax,OOFS(%rdx,j,0);	\
	shr	$32,%rax;		\
	mov	%eax,OOFS(%rdx,j+0x20,0);	\
	.endif;				\
	.if (m)<64;	\
	mov	IOFS(%r10,m,0),%rax;	\
	mov	%eax,OOFS(%rdx,j,1);	\
	shr	$32,%rax;		\
	mov	%eax,OOFS(%rdx,j+0x20,1);	\
	.endif

/* (uint64_t const *RCX, uint64 *RDX)
   (clobber A C D 8 9 10 11) */
.globl transpose64
transpose64:
.globl _transpose64
_transpose64:
	push	IK

	mov	%rcx,%r10
	xor	%rax,%rax
	mov	%rax,OOFS(%rdx,0x16,0)
	mov	%rax,OOFS(%rdx,0x16+0x20,0)
	mov	%rax,OOFS(%rdx,0x17,0)
	mov	%rax,OOFS(%rdx,0x17+0x20,0)
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
	mov	$-1,MASK
	mov	$16,IC
	shl	$32,MASK
LI:
	mov	MASK,%r8
	shr	ICL,MASK
	xor	%r8,MASK
	lea	(%rdx,IC,8),%r10
	xor	IK,IK
LJ:
	mov	(%rdx,IK,8),%r8
	mov	(%r10,IK,8),%r9
	mov	%r9,%rax
	shl	ICL,%r9
	xor	%r8,%r9
	and	MASK,%r9
	xor	%r9,%r8
	mov	%r8,(%rdx,IK,8)
	shr	ICL,%r9
	xor	%rax,%r9
	mov	%r9,(%r10,IK,8)
	add	IC,IK
	add	$1,IK
	or	IC,IK
	xor	IC,IK
	cmp	$64,IK
	jl	LJ

	shr	IC
	jne	LI

	pop	IK
	ret

// EOF
