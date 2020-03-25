####################################################################################
# Assembler math routines for x86_64 ### only uses FPU and SSE instructions
# Author: Emil
####################################################################################

.text

.global sin
	.type	sin,@function
sin:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	fsin
	fnstsw	%ax
	testb	$0x04, %ah
	je	3f
	fldpi
	fadd	%st
	fxch	%st(1)
2:	fprem1
	fnstsw	%ax
	testb	$0x04, %ah
	jne	2b
	fstp	%st(1)
	fsin
3:
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	sin,1b-sin

.global cos
	.type	cos,@function
cos:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	fcos
	fnstsw	%ax
	testb	$0x04, %ah
	je	3f
	fldpi
	fadd	%st
	fxch	%st(1)
2:	fprem1
	fnstsw	%ax
	testb	$0x04, %ah
	jne	2b
	fstp	%st(1)
	fcos
3:
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	cos,1b-cos

.global sincos
	.type	sincos,@function
sincos:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call	__fmod2pi@PLT
	PIC_RESTORE
#else
	call	__fmod2pi
#endif
	fsincos
	fstpl	(%rsi)
	fstpl	(%rdi)
	add	$8, %rsp
	ret
1:
	.size	sincos,1b-sincos

.global tan
	.type	tan,@function
tan:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call	__fmod2pi@PLT
	PIC_RESTORE
#else
	call	__fmod2pi
#endif
	fsincos
	fdivrp
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	tan,1b-tan

.global asin
	.type	asin,@function
asin:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	fld	%st
	fmul	%st
	fld1
	fsubp
	fsqrt
	fpatan
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	asin,1b-asin

.global acos
	.type	acos,@function
acos:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	fld	%st
	fmul	%st
	fld1
	fsubp
	fsqrt
	fxch	%st(1)
	fpatan
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	acos,1b-acos

.global atan2
	.type	atan2,@function
atan2:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	movsd	%xmm1, (%rsp)
	fldl	(%rsp)
	fpatan
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	atan2,1b-atan2

.global atan
	.type	atan,@function
atan:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	fld1
	fpatan
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	atan,1b-atan

	.section	.note.GNU-stack,"",@progbits
