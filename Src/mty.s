	.file	"mty.c"
	.text
	.def	_popcnt32;	.scl	3;	.type	32;	.endef
_popcnt32:
LFB9:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	8(%ebp), %eax
	shrl	%eax
	andl	$1431655765, %eax
	subl	%eax, 8(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, %edx
	andl	$858993459, %edx
	movl	8(%ebp), %eax
	shrl	$2, %eax
	andl	$858993459, %eax
	addl	%edx, %eax
	movl	%eax, 8(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, %edx
	shrl	$4, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	andl	$252645135, %eax
	movl	%eax, 8(%ebp)
	movl	8(%ebp), %eax
	shrl	$8, %eax
	addl	%eax, 8(%ebp)
	movl	8(%ebp), %eax
	shrl	$16, %eax
	addl	%eax, 8(%ebp)
	movl	8(%ebp), %eax
	andl	$63, %eax
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE9:
	.def	_popcnt64;	.scl	3;	.type	32;	.endef
_popcnt64:
LFB10:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$16, %esp
	.cfi_offset 3, -12
	movl	8(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	_popcnt32
	movl	%eax, %ebx
	movl	-16(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	xorl	%edx, %edx
	movl	%eax, (%esp)
	call	_popcnt32
	addl	%ebx, %eax
	addl	$16, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE10:
.lcomm _mutex_key,4,4
	.section .rdata,"dr"
	.align 4
_crypt64_descs:
	.long	_crypt64_desc
.lcomm _n_cpus,4,4
.lcomm _loop_cpu,8192,32
	.text
	.def	_usec;	.scl	3;	.type	32;	.endef
_usec:
LFB11:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$52, %esp
	.cfi_offset 3, -12
	leal	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	_ftime
	movl	-28(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-24(%ebp), %eax
	cwtl
	movl	%eax, -16(%ebp)
	movl	-12(%ebp), %eax
	movl	$0, %edx
	imull	$1000, %edx, %ebx
	imull	$0, %eax, %ecx
	addl	%ecx, %ebx
	movl	$1000, %ecx
	mull	%ecx
	leal	(%ebx,%edx), %ecx
	movl	%ecx, %edx
	movl	-16(%ebp), %ecx
	movl	$0, %ebx
	addl	%ecx, %eax
	adcl	%ebx, %edx
	addl	$52, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE11:
	.def	_cpuid_getfflags;	.scl	3;	.type	32;	.endef
_cpuid_getfflags:
LFB12:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$24, %esp
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	movl	$1, -36(%ebp)
	movl	-36(%ebp), %eax
/APP
 # 104 "mty.c" 1
	cpuid
 # 0 "" 2
/NO_APP
	movl	%edx, %esi
	movl	%ecx, %edi
	movl	%eax, -32(%ebp)
	movl	-32(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	%ebx, -20(%ebp)
	movl	%edi, -24(%ebp)
	movl	%esi, -28(%ebp)
	movl	-28(%ebp), %eax
	addl	$24, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%esi
	.cfi_restore 6
	popl	%edi
	.cfi_restore 7
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE12:
	.def	_cpuid_issupported;	.scl	3;	.type	32;	.endef
_cpuid_issupported:
LFB13:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	$109084672, -4(%ebp)
	call	_cpuid_getfflags
	notl	%eax
	andl	-4(%ebp), %eax
	testl	%eax, %eax
	sete	%al
	movzbl	%al, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE13:
	.section .rdata,"dr"
LC0:
	.ascii "mty.c\0"
LC1:
	.ascii "n >= 1\0"
	.text
	.def	_packet_create;	.scl	3;	.type	32;	.endef
_packet_create:
LFB14:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$68, %esp
	.cfi_offset 3, -12
	movl	$128, -24(%ebp)
	cmpl	$0, 8(%ebp)
	jg	L12
	movl	$134, 8(%esp)
	movl	$LC0, 4(%esp)
	movl	$LC1, (%esp)
	call	__assert
L12:
	movl	8(%ebp), %eax
	decl	%eax
	sall	$12, %eax
	movl	%eax, %edx
	movl	-24(%ebp), %eax
	addl	%eax, %edx
	movl	12(%ebp), %eax
	sall	$4, %eax
	addl	$3792, %eax
	addl	%edx, %eax
	decl	%eax
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %eax
	movl	$1, 4(%esp)
	movl	%eax, (%esp)
	call	_calloc
	movl	%eax, -32(%ebp)
	movl	-32(%ebp), %edx
	movl	-24(%ebp), %eax
	addl	%edx, %eax
	leal	-1(%eax), %edx
	movl	-24(%ebp), %eax
	negl	%eax
	andl	%edx, %eax
	movl	%eax, -36(%ebp)
	movl	$0, -12(%ebp)
	jmp	L13
L20:
	movl	-12(%ebp), %eax
	movl	%eax, %edx
	sall	$12, %edx
	movl	-36(%ebp), %eax
	addl	%edx, %eax
	addl	$3536, %eax
	movl	$16, 8(%esp)
	movl	$-1, 4(%esp)
	movl	%eax, (%esp)
	call	_memset
	movl	-12(%ebp), %eax
	movl	%eax, %edx
	sall	$12, %edx
	movl	-36(%ebp), %eax
	addl	%edx, %eax
	addl	$16, %eax
	movl	%eax, (%esp)
	call	_key_init_sk
	movl	$0, -16(%ebp)
	jmp	L14
L17:
	movl	$0, -20(%ebp)
	jmp	L15
L16:
	movl	-12(%ebp), %eax
	movl	%eax, %edx
	sall	$12, %edx
	movl	-36(%ebp), %eax
	addl	%eax, %edx
	movl	-16(%ebp), %eax
	movl	_ks_ls(,%eax,4), %eax
	movl	%eax, %ecx
	sall	$4, %ecx
	movl	-16(%ebp), %eax
	leal	0(,%eax,4), %ebx
	movl	-20(%ebp), %eax
	addl	%ebx, %eax
	addl	$452, %eax
	movl	%ecx, (%edx,%eax,4)
	incl	-20(%ebp)
L15:
	movl	-20(%ebp), %eax
	cmpl	$3, %eax
	jbe	L16
	incl	-16(%ebp)
L14:
	cmpl	$27, -16(%ebp)
	jle	L17
	movl	$0, -16(%ebp)
	jmp	L18
L19:
	movl	-12(%ebp), %eax
	movl	%eax, %edx
	sall	$12, %edx
	movl	-36(%ebp), %eax
	addl	%eax, %edx
	movl	-16(%ebp), %ecx
	movl	16(%ebp), %eax
	addl	%ecx, %eax
	movb	(%eax), %al
	movl	-16(%ebp), %ecx
	addl	%edx, %ecx
	movb	%al, (%ecx)
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %al
	movzbl	%al, %eax
	movl	-12(%ebp), %edx
	movl	%edx, %ecx
	sall	$12, %ecx
	movl	-36(%ebp), %edx
	addl	%ecx, %edx
	addl	$16, %edx
	movl	$127, 16(%esp)
	movl	$0, 12(%esp)
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_key_set64
	incl	-16(%ebp)
L18:
	cmpl	$7, -16(%ebp)
	jle	L19
	incl	-12(%ebp)
L13:
	movl	-12(%ebp), %eax
	cmpl	8(%ebp), %eax
	jl	L20
	movl	-36(%ebp), %eax
	addl	$68, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE14:
	.section .rdata,"dr"
	.align 4
LC2:
	.ascii "Usually %d times is the norm (? may be a very inaccurate translation)\12\0"
	.text
	.def	_thread_avail;	.scl	3;	.type	32;	.endef
_thread_avail:
LFB15:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	call	_GetCurrentProcess@0
	leal	-16(%ebp), %edx
	movl	%edx, 8(%esp)
	leal	-12(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_GetProcessAffinityMask@12
	subl	$12, %esp
	testl	%eax, %eax
	je	L23
	movl	-12(%ebp), %eax
	testl	%eax, %eax
	je	L23
	movl	-16(%ebp), %eax
	testl	%eax, %eax
	jne	L24
L23:
	movl	$1, %eax
	movl	$0, %edx
	jmp	L27
L24:
	movl	-16(%ebp), %eax
	movl	$0, %edx
	movl	%eax, (%esp)
	movl	%edx, 4(%esp)
	call	_popcnt64
	cmpl	$1, %eax
	je	L26
	movl	-12(%ebp), %edx
	movl	-16(%ebp), %eax
	cmpl	%eax, %edx
	jne	L26
	movl	-12(%ebp), %eax
	movl	$0, %edx
	movl	%eax, (%esp)
	movl	%edx, 4(%esp)
	call	_popcnt64
	movl	%eax, 8(%esp)
	movl	$LC2, 4(%esp)
	movl	__imp___iob, %eax
	addl	$64, %eax
	movl	%eax, (%esp)
	call	_fprintf
L26:
	movl	-12(%ebp), %eax
	movl	$0, %edx
L27:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE15:
	.def	_thread_crypt64_new;	.scl	3;	.type	32;	.endef
_thread_crypt64_new:
LFB16:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	movl	$8332, %eax
	call	___chkstk_ms
	subl	%eax, %esp
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	movl	8(%ebp), %eax
	movl	%eax, -32(%ebp)
	movl	-32(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -36(%ebp)
	movl	-32(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, %edx
	movl	-36(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -40(%ebp)
	leal	-8284(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$1024, 4(%esp)
	movl	$16, (%esp)
	call	_packet_create
	movl	%eax, -44(%ebp)
	call	_GetCurrentThread@0
	movl	%eax, -48(%ebp)
	movl	_mutex_key, %eax
	movl	$-1, 4(%esp)
	movl	%eax, (%esp)
	call	_WaitForSingleObject@8
	subl	$8, %esp
	movl	_n_cpus, %eax
	leal	0(,%eax,8), %edx
	addl	$_loop_cpu, %edx
	movl	%edx, -52(%ebp)
	incl	%eax
	movl	%eax, _n_cpus
	call	_usec
	movl	%eax, %edx
	movl	-32(%ebp), %eax
	movl	8(%eax), %eax
	xorl	%eax, %edx
	movl	-48(%ebp), %eax
	xorl	%edx, %eax
	movl	%eax, (%esp)
	call	_srand
	leal	-8284(%ebp), %eax
	movl	%eax, (%esp)
	call	_key_init
	movl	_mutex_key, %eax
	movl	%eax, (%esp)
	call	_ReleaseMutex@4
	subl	$4, %esp
	movl	-32(%ebp), %eax
	movl	12(%eax), %eax
	movl	%eax, 4(%esp)
	movl	-48(%ebp), %eax
	movl	%eax, (%esp)
	call	_SetThreadPriority@8
	subl	$8, %esp
L31:
	movl	$0, -28(%ebp)
	jmp	L29
L30:
	leal	-8284(%ebp), %edx
	movl	-28(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %dl
	movl	-44(%ebp), %ecx
	movl	-28(%ebp), %eax
	addl	%ecx, %eax
	movb	(%eax), %al
	xorl	%edx, %eax
	movzbl	%al, %edx
	leal	-8284(%ebp), %ecx
	movl	-28(%ebp), %eax
	addl	%ecx, %eax
	movb	(%eax), %al
	movzbl	%al, %eax
	movl	-44(%ebp), %ecx
	addl	$16, %ecx
	movl	$0, 16(%esp)
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%ecx, (%esp)
	call	_key_set64
	leal	-8284(%ebp), %edx
	movl	-28(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %al
	movl	-44(%ebp), %ecx
	movl	-28(%ebp), %edx
	addl	%ecx, %edx
	movb	%al, (%edx)
	incl	-28(%ebp)
L29:
	cmpl	$7, -28(%ebp)
	jle	L30
	movl	-44(%ebp), %eax
	leal	16(%eax), %ecx
	movl	-44(%ebp), %eax
	leal	2256(%eax), %edx
	movl	-36(%ebp), %eax
	movl	%eax, -8308(%ebp)
	movl	%ecx, %esi
	movl	%edx, %ebx
	movl	%esi, %ecx
	movl	%ebx, %edx
	movl	-8308(%ebp), %eax
/APP
 # 405 "mty.c" 1
	call *%eax
 # 0 "" 2
/NO_APP
	movl	%edx, %ebx
	movl	%ecx, %esi
	movl	%eax, %edi
	movl	%edi, -56(%ebp)
	movl	%esi, -60(%ebp)
	movl	%ebx, -64(%ebp)
	movl	-64(%ebp), %eax
	movl	$0, %edx
	movl	%eax, %ecx
	movl	%edx, %ebx
	movl	%ecx, %ebx
	movl	$0, %ecx
	movl	-56(%ebp), %eax
	movl	$0, %edx
	movl	%ecx, %esi
	orl	%eax, %esi
	movl	%esi, -8304(%ebp)
	movl	%ebx, %esi
	orl	%edx, %esi
	movl	%esi, -8300(%ebp)
	movl	-44(%ebp), %eax
	leal	3792(%eax), %ecx
	movl	-44(%ebp), %eax
	leal	2256(%eax), %edx
	movl	-40(%ebp), %eax
	movl	%eax, -8308(%ebp)
	movl	%ecx, %esi
	movl	%edx, %ebx
	movl	%esi, %ecx
	movl	%ebx, %edx
	movl	-8308(%ebp), %eax
/APP
 # 408 "mty.c" 1
	call *%eax
 # 0 "" 2
/NO_APP
	movl	%edx, %ebx
	movl	%ecx, %esi
	movl	%eax, %edi
	movl	%edi, -68(%ebp)
	movl	%esi, -72(%ebp)
	movl	%ebx, -76(%ebp)
	movl	-44(%ebp), %eax
	addl	$3792, %eax
	movl	%eax, 4(%esp)
	movl	-44(%ebp), %eax
	movl	%eax, (%esp)
	call	_check_hit
	movl	-52(%ebp), %eax
	movl	4(%eax), %edx
	movl	(%eax), %eax
	addl	$128, %eax
	adcl	$0, %edx
	movl	-52(%ebp), %ecx
	movl	%eax, (%ecx)
	movl	%edx, 4(%ecx)
	movl	$8, 8(%esp)
	movl	$6, 4(%esp)
	leal	-8284(%ebp), %eax
	movl	%eax, (%esp)
	call	_key_inc
	testl	%eax, %eax
	jne	L31
	movl	$8, 8(%esp)
	movl	$4, 4(%esp)
	leal	-8284(%ebp), %eax
	movl	%eax, (%esp)
	call	_key_inc
	testl	%eax, %eax
	jne	L31
	movl	_mutex_key, %eax
	movl	$-1, 4(%esp)
	movl	%eax, (%esp)
	call	_WaitForSingleObject@8
	subl	$8, %esp
	movl	$0, 4(%esp)
	leal	-8284(%ebp), %eax
	movl	%eax, (%esp)
	call	_key_reset
	movl	_mutex_key, %eax
	movl	%eax, (%esp)
	call	_ReleaseMutex@4
	subl	$4, %esp
	jmp	L31
	.cfi_endproc
LFE16:
	.def	_initialize_gpu_searcher;	.scl	3;	.type	32;	.endef
_initialize_gpu_searcher:
LFB17:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	$0, 12(%esp)
	movl	$_gpu_main, 8(%esp)
	movl	$0, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__pthread_create, %eax
	call	*%eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE17:
	.def	___main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
	.align 4
LC3:
	.ascii "Processor ID or architecture was not recognized.\12Code may not run optimally.\12\0"
LC4:
	.ascii "target.txt\0"
LC5:
	.ascii "log.txt\0"
LC6:
	.ascii "Starting search!\12\0"
	.def	___udivdi3;	.scl	2;	.type	32;	.endef
	.align 4
LC7:
	.ascii "%4d.%03dMtrips/s [%4d.%06dMtrips/s]\15\0"
	.text
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
LFB18:
	.cfi_startproc
	leal	4(%esp), %ecx
	.cfi_def_cfa 1, 0
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	.cfi_escape 0x10,0x5,0x2,0x75,0
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	pushl	%ecx
	.cfi_escape 0xf,0x3,0x75,0x70,0x6
	.cfi_escape 0x10,0x7,0x2,0x75,0x7c
	.cfi_escape 0x10,0x6,0x2,0x75,0x78
	.cfi_escape 0x10,0x3,0x2,0x75,0x74
	movl	$8392, %eax
	call	___chkstk_ms
	subl	%eax, %esp
	call	___main
	movl	$0, -44(%ebp)
	movl	$0, -48(%ebp)
	movl	$0, -40(%ebp)
	call	_cpuid_issupported
	testl	%eax, %eax
	jne	L34
	movl	__imp___iob, %eax
	addl	$64, %eax
	movl	%eax, 12(%esp)
	movl	$77, 8(%esp)
	movl	$1, 4(%esp)
	movl	$LC3, (%esp)
	call	_fwrite
	movl	$1, (%esp)
	call	_exit
L34:
	movl	$0, 8(%esp)
	movl	$0, 4(%esp)
	movl	$0, (%esp)
	call	_CreateMutexA@12
	subl	$12, %esp
	movl	%eax, _mutex_key
	movl	$LC4, (%esp)
	call	_expr_parse
	movl	%eax, -52(%ebp)
	call	_scoreboard_open
	movl	%eax, -56(%ebp)
	movl	_crypt64_descs, %eax
	movl	8(%eax), %eax
	movl	%eax, %edx
	movl	_crypt64_descs, %eax
	movl	(%eax), %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	movl	%eax, %edx
	movl	_crypt64_descs, %eax
	movl	(%eax), %eax
	movl	-56(%ebp), %ecx
	movl	%ecx, 12(%esp)
	movl	%edx, 8(%esp)
	movl	$1, 4(%esp)
	movl	%eax, (%esp)
	call	_fwrite
	call	_thread_avail
	movl	%eax, -64(%ebp)
	movl	%edx, -60(%ebp)
	movl	_crypt64_descs, %eax
	movl	20(%eax), %eax
	movl	%eax, %edx
	movl	_crypt64_descs, %eax
	movl	16(%eax), %eax
	movl	%edx, %edi
	subl	%eax, %edi
	movl	%edi, %eax
	movl	%eax, %edx
	movl	_crypt64_descs, %eax
	movl	16(%eax), %eax
	movl	-56(%ebp), %ecx
	movl	%ecx, 12(%esp)
	movl	%edx, 8(%esp)
	movl	$1, 4(%esp)
	movl	%eax, (%esp)
	call	_fwrite
	movl	-56(%ebp), %eax
	movl	%eax, (%esp)
	call	_ftell
	movl	%eax, -68(%ebp)
	movl	-68(%ebp), %eax
	negl	%eax
	andl	$63, %eax
	movl	$1, 8(%esp)
	movl	%eax, 4(%esp)
	movl	-56(%ebp), %eax
	movl	%eax, (%esp)
	call	_fseek
	movl	-56(%ebp), %eax
	movl	%eax, (%esp)
	call	_ftell
	movl	%eax, -68(%ebp)
	movl	_crypt64_descs, %eax
	movl	4(%eax), %eax
	movl	%eax, %edx
	movl	_crypt64_descs, %eax
	movl	(%eax), %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	movl	%eax, %edx
	movl	_crypt64_descs, %eax
	movl	(%eax), %eax
	movl	-56(%ebp), %ecx
	movl	%ecx, 12(%esp)
	movl	%edx, 8(%esp)
	movl	$1, 4(%esp)
	movl	%eax, (%esp)
	call	_fwrite
	movl	_crypt64_descs, %eax
	movl	12(%eax), %eax
	movl	%eax, %edx
	movl	_crypt64_descs, %eax
	movl	8(%eax), %eax
	movl	%edx, %edi
	subl	%eax, %edi
	movl	%edi, %eax
	movl	%eax, %edx
	movl	_crypt64_descs, %eax
	movl	8(%eax), %eax
	movl	-56(%ebp), %ecx
	movl	%ecx, 12(%esp)
	movl	%edx, 8(%esp)
	movl	$1, 4(%esp)
	movl	%eax, (%esp)
	call	_fwrite
	movl	-52(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-56(%ebp), %eax
	movl	%eax, (%esp)
	call	_synth_synthesize
	movl	%eax, -72(%ebp)
	movl	_crypt64_descs, %eax
	movl	20(%eax), %eax
	movl	%eax, %edx
	movl	_crypt64_descs, %eax
	movl	12(%eax), %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	movl	%eax, %edx
	movl	_crypt64_descs, %eax
	movl	12(%eax), %eax
	movl	-56(%ebp), %ecx
	movl	%ecx, 12(%esp)
	movl	%edx, 8(%esp)
	movl	$1, 4(%esp)
	movl	%eax, (%esp)
	call	_fwrite
	movl	-56(%ebp), %eax
	movl	%eax, (%esp)
	call	_scoreboard_map
	movl	%eax, -44(%ebp)
	movl	_mutex_key, %eax
	movl	$-1, 4(%esp)
	movl	%eax, (%esp)
	call	_WaitForSingleObject@8
	subl	$8, %esp
	call	_usec
	movl	%eax, (%esp)
	call	_srand
	leal	-8324(%ebp), %eax
	movl	%eax, (%esp)
	call	_key_init
	movl	_mutex_key, %eax
	movl	%eax, (%esp)
	call	_ReleaseMutex@4
	subl	$4, %esp
	movl	_crypt64_descs, %eax
	leal	-8324(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	-44(%ebp), %eax
	movl	%eax, (%esp)
	call	_set_salt
	movl	$LC5, (%esp)
	call	_log_open
	testl	%eax, %eax
	je	L35
	movl	$1, %eax
	jmp	L47
L35:
	movl	_mutex_key, %eax
	movl	$-1, 4(%esp)
	movl	%eax, (%esp)
	call	_WaitForSingleObject@8
	subl	$8, %esp
	movl	$0, -32(%ebp)
	cmpl	$0, -68(%ebp)
	je	L37
	movl	$-1, -76(%ebp)
	movl	-64(%ebp), %eax
	movl	-60(%ebp), %edx
	movl	%eax, (%esp)
	movl	%edx, 4(%esp)
	call	_popcnt64
	sall	%eax
	movl	$16, 4(%esp)
	movl	%eax, (%esp)
	call	_calloc
	movl	%eax, -48(%ebp)
	movl	$0, -28(%ebp)
	jmp	L38
L40:
	movl	-28(%ebp), %ecx
	movl	-64(%ebp), %eax
	movl	-60(%ebp), %edx
	shrdl	%cl, %edx, %eax
	shrl	%cl, %edx
	testb	$32, %cl
	je	L49
	movl	%edx, %eax
	xorl	%edx, %edx
L49:
	movl	%eax, %ebx
	andl	$1, %ebx
	movl	%edx, %esi
	andl	$0, %esi
	movl	%esi, %eax
	orl	%ebx, %eax
	testl	%eax, %eax
	je	L39
	movl	-32(%ebp), %eax
	movl	%eax, %edx
	sall	$4, %edx
	movl	-48(%ebp), %eax
	addl	%eax, %edx
	movl	-44(%ebp), %eax
	movl	%eax, (%edx)
	movl	-32(%ebp), %eax
	movl	%eax, %edx
	sall	$4, %edx
	movl	-48(%ebp), %eax
	addl	%eax, %edx
	movl	-68(%ebp), %eax
	movl	%eax, 4(%edx)
	movl	-32(%ebp), %eax
	movl	%eax, %edx
	sall	$4, %edx
	movl	-48(%ebp), %eax
	leal	(%edx,%eax), %edi
	call	_rand
	movl	%eax, 8(%edi)
	movl	-32(%ebp), %eax
	movl	%eax, %edx
	sall	$4, %edx
	movl	-48(%ebp), %eax
	addl	%edx, %eax
	movl	$-2, 12(%eax)
	movl	-32(%ebp), %eax
	movl	%eax, %edx
	sall	$4, %edx
	movl	-48(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, 8(%esp)
	movl	$8192, 4(%esp)
	movl	$_thread_crypt64_new, (%esp)
	call	__beginthread
	movl	%eax, -80(%ebp)
	movl	-28(%ebp), %eax
	movl	$1, %edx
	movl	%edx, %edi
	movb	%al, %cl
	sall	%cl, %edi
	movl	%edi, %eax
	movl	%eax, 4(%esp)
	movl	-80(%ebp), %eax
	movl	%eax, (%esp)
	call	_SetThreadAffinityMask@8
	subl	$8, %esp
	incl	-32(%ebp)
	movl	-32(%ebp), %eax
	movl	%eax, %edx
	sall	$4, %edx
	movl	-48(%ebp), %eax
	addl	%eax, %edx
	movl	-44(%ebp), %eax
	movl	%eax, (%edx)
	movl	-32(%ebp), %eax
	movl	%eax, %edx
	sall	$4, %edx
	movl	-48(%ebp), %eax
	addl	%eax, %edx
	movl	-68(%ebp), %eax
	movl	%eax, 4(%edx)
	movl	-32(%ebp), %eax
	movl	%eax, %edx
	sall	$4, %edx
	movl	-48(%ebp), %eax
	leal	(%edx,%eax), %edi
	call	_rand
	movl	%eax, 8(%edi)
	movl	-32(%ebp), %eax
	movl	%eax, %edx
	sall	$4, %edx
	movl	-48(%ebp), %eax
	addl	%edx, %eax
	movl	$-15, 12(%eax)
	movl	-32(%ebp), %eax
	movl	%eax, %edx
	sall	$4, %edx
	movl	-48(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, 8(%esp)
	movl	$8192, 4(%esp)
	movl	$_thread_crypt64_new, (%esp)
	call	__beginthread
	movl	%eax, -80(%ebp)
	movl	-64(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-80(%ebp), %eax
	movl	%eax, (%esp)
	call	_SetThreadAffinityMask@8
	subl	$8, %esp
	incl	-32(%ebp)
L39:
	incl	-28(%ebp)
L38:
	cmpl	$63, -28(%ebp)
	jle	L40
	cmpl	$0, -76(%ebp)
	je	L37
	movl	-76(%ebp), %eax
	movl	$1, %edx
	movl	%edx, %ebx
	movb	%al, %cl
	sall	%cl, %ebx
	call	_GetCurrentThread@0
	movl	%ebx, 4(%esp)
	movl	%eax, (%esp)
	call	_SetThreadAffinityMask@8
	subl	$8, %esp
L37:
	leal	-8368(%ebp), %eax
	movl	%eax, (%esp)
	call	_initialize_gpu_searcher
	movl	__imp___iob, %eax
	addl	$64, %eax
	movl	%eax, 12(%esp)
	movl	$17, 8(%esp)
	movl	$1, 4(%esp)
	movl	$LC6, (%esp)
	call	_fwrite
	movl	_mutex_key, %eax
	movl	%eax, (%esp)
	call	_ReleaseMutex@4
	subl	$4, %esp
	movl	$2147483647, -84(%ebp)
	movl	$0, -36(%ebp)
	movl	$32, 8(%esp)
	movl	$0, 4(%esp)
	leal	-8360(%ebp), %eax
	movl	%eax, (%esp)
	call	_memset
	call	_usec
	movl	%eax, -8352(%ebp)
	movl	%edx, -8348(%ebp)
	movl	-8352(%ebp), %eax
	movl	-8348(%ebp), %edx
	movl	%eax, -8360(%ebp)
	movl	%edx, -8356(%ebp)
	jmp	L45
L48:
	nop
L45:
	movl	$5000, (%esp)
	call	_Sleep@4
	subl	$4, %esp
	movl	$0, -8344(%ebp)
	movl	$0, -8340(%ebp)
	movl	$0, -28(%ebp)
	jmp	L41
L42:
	movl	-8344(%ebp), %ecx
	movl	-8340(%ebp), %ebx
	movl	-28(%ebp), %eax
	movl	_loop_cpu+4(,%eax,8), %edx
	movl	_loop_cpu(,%eax,8), %eax
	addl	%ecx, %eax
	adcl	%ebx, %edx
	movl	%eax, -8344(%ebp)
	movl	%edx, -8340(%ebp)
	incl	-28(%ebp)
L41:
	movl	_n_cpus, %eax
	cmpl	%eax, -28(%ebp)
	jl	L42
	movl	-8344(%ebp), %ecx
	movl	-8340(%ebp), %ebx
	movl	-8336(%ebp), %esi
	movl	-8332(%ebp), %edi
	movl	-40(%ebp), %eax
	movl	$0, %edx
	addl	%esi, %eax
	adcl	%edi, %edx
	cmpl	%edx, %ebx
	jb	L48
	cmpl	%edx, %ebx
	ja	L46
	cmpl	%eax, %ecx
	jb	L48
L46:
	call	_usec
	movl	%eax, -96(%ebp)
	movl	%edx, -92(%ebp)
	movl	-8352(%ebp), %eax
	movl	-8348(%ebp), %edx
	movl	%edx, %ecx
	xorl	-92(%ebp), %ecx
	xorl	-96(%ebp), %eax
	orl	%ecx, %eax
	testl	%eax, %eax
	je	L48
	movl	-8360(%ebp), %ecx
	movl	-8356(%ebp), %ebx
	movl	-96(%ebp), %eax
	movl	-92(%ebp), %edx
	subl	%ecx, %eax
	sbbl	%ebx, %edx
	movl	%eax, -104(%ebp)
	movl	%edx, -100(%ebp)
	movl	-8344(%ebp), %eax
	movl	-8340(%ebp), %edx
	movl	-104(%ebp), %ecx
	movl	-100(%ebp), %ebx
	movl	%ecx, 8(%esp)
	movl	%ebx, 12(%esp)
	movl	%eax, (%esp)
	movl	%edx, 4(%esp)
	call	___udivdi3
	movl	%eax, -108(%ebp)
	movl	-8352(%ebp), %ecx
	movl	-8348(%ebp), %ebx
	movl	-96(%ebp), %eax
	movl	-92(%ebp), %edx
	subl	%ecx, %eax
	sbbl	%ebx, %edx
	movl	%eax, -104(%ebp)
	movl	%edx, -100(%ebp)
	movl	-8344(%ebp), %eax
	movl	-8340(%ebp), %edx
	movl	-8336(%ebp), %ecx
	movl	-8332(%ebp), %ebx
	subl	%ecx, %eax
	sbbl	%ebx, %edx
	imull	$1000, %edx, %ebx
	imull	$0, %eax, %ecx
	addl	%ecx, %ebx
	movl	$1000, %ecx
	mull	%ecx
	leal	(%ebx,%edx), %ecx
	movl	%ecx, %edx
	movl	-104(%ebp), %ecx
	movl	-100(%ebp), %ebx
	movl	%ecx, 8(%esp)
	movl	%ebx, 12(%esp)
	movl	%eax, (%esp)
	movl	%edx, 4(%esp)
	call	___udivdi3
	movl	%eax, -112(%ebp)
	movl	-112(%ebp), %eax
	sall	$3, %eax
	movl	%eax, -116(%ebp)
	movl	-116(%ebp), %eax
	movl	-112(%ebp), %ecx
	movl	-40(%ebp), %edx
	addl	%ecx, %edx
	cmpl	%edx, %eax
	cmova	%edx, %eax
	movl	%eax, -40(%ebp)
	movl	-96(%ebp), %eax
	movl	-92(%ebp), %edx
	movl	%eax, -8352(%ebp)
	movl	%edx, -8348(%ebp)
	movl	-8344(%ebp), %eax
	movl	-8340(%ebp), %edx
	movl	%eax, -8336(%ebp)
	movl	%edx, -8332(%ebp)
	movl	-112(%ebp), %eax
	movl	$1000000, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %ebx
	movl	-112(%ebp), %ecx
	movl	$1125899907, %eax
	imull	%ecx
	sarl	$18, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	movl	%edx, %edi
	subl	%eax, %edi
	movl	-108(%ebp), %eax
	movl	$1000, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %ecx
	movl	-108(%ebp), %esi
	movl	$274877907, %eax
	imull	%esi
	sarl	$6, %edx
	movl	%esi, %eax
	sarl	$31, %eax
	movl	%edx, %esi
	subl	%eax, %esi
	movl	%esi, %eax
	movl	%ebx, 20(%esp)
	movl	%edi, 16(%esp)
	movl	%ecx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC7, 4(%esp)
	movl	__imp___iob, %eax
	addl	$64, %eax
	movl	%eax, (%esp)
	call	_fprintf
	incl	-36(%ebp)
	jmp	L48
L47:
	leal	-16(%ebp), %esp
	popl	%ecx
	.cfi_restore 1
	.cfi_def_cfa 1, 0
	popl	%ebx
	.cfi_restore 3
	popl	%esi
	.cfi_restore 6
	popl	%edi
	.cfi_restore 7
	popl	%ebp
	.cfi_restore 5
	leal	-4(%ecx), %esp
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE18:
	.def	_ftime;	.scl	2;	.type	32;	.endef
	.def	__assert;	.scl	2;	.type	32;	.endef
	.def	_calloc;	.scl	2;	.type	32;	.endef
	.def	_memset;	.scl	2;	.type	32;	.endef
	.def	_key_init_sk;	.scl	2;	.type	32;	.endef
	.def	_key_set64;	.scl	2;	.type	32;	.endef
	.def	_GetCurrentProcess@0;	.scl	2;	.type	32;	.endef
	.def	_GetProcessAffinityMask@12;	.scl	2;	.type	32;	.endef
	.def	_fprintf;	.scl	2;	.type	32;	.endef
	.def	_GetCurrentThread@0;	.scl	2;	.type	32;	.endef
	.def	_WaitForSingleObject@8;	.scl	2;	.type	32;	.endef
	.def	_srand;	.scl	2;	.type	32;	.endef
	.def	_key_init;	.scl	2;	.type	32;	.endef
	.def	_ReleaseMutex@4;	.scl	2;	.type	32;	.endef
	.def	_SetThreadPriority@8;	.scl	2;	.type	32;	.endef
	.def	_check_hit;	.scl	2;	.type	32;	.endef
	.def	_key_inc;	.scl	2;	.type	32;	.endef
	.def	_key_reset;	.scl	2;	.type	32;	.endef
	.def	_gpu_main;	.scl	2;	.type	32;	.endef
	.def	_fwrite;	.scl	2;	.type	32;	.endef
	.def	_exit;	.scl	2;	.type	32;	.endef
	.def	_CreateMutexA@12;	.scl	2;	.type	32;	.endef
	.def	_expr_parse;	.scl	2;	.type	32;	.endef
	.def	_scoreboard_open;	.scl	2;	.type	32;	.endef
	.def	_fwrite;	.scl	2;	.type	32;	.endef
	.def	_ftell;	.scl	2;	.type	32;	.endef
	.def	_fseek;	.scl	2;	.type	32;	.endef
	.def	_synth_synthesize;	.scl	2;	.type	32;	.endef
	.def	_scoreboard_map;	.scl	2;	.type	32;	.endef
	.def	_set_salt;	.scl	2;	.type	32;	.endef
	.def	_log_open;	.scl	2;	.type	32;	.endef
	.def	_rand;	.scl	2;	.type	32;	.endef
	.def	__beginthread;	.scl	2;	.type	32;	.endef
	.def	_SetThreadAffinityMask@8;	.scl	2;	.type	32;	.endef
	.def	_Sleep@4;	.scl	2;	.type	32;	.endef
