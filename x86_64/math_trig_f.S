####################################################################################
# Assembler math routines for x86_64 ### only uses FPU and SSE instructions
# Author: Emil
####################################################################################

.text

.global sinf
	.type	sinf,@function
sinf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
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
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	sinf,1b-sinf

.global cosf
	.type	cosf,@function
cosf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
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
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	cosf,1b-cosf

.global sincosf
	.type	sincosf,@function
sincosf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call	__fmod2pi@PLT
	PIC_RESTORE
#else
	call	__fmod2pi
#endif
	fsincos
	fstps	(%rsi)
	fstps	(%rdi)
	add	$8, %rsp
	ret
1:
	.size	sincosf,1b-sincosf

.global tanf
	.type	tanf,@function
tanf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
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
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	tanf,1b-tanf

.global asinf
	.type	asinf,@function
asinf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	fld	%st
	fmul	%st
	fld1
	fsubp
	fsqrt
	fpatan
	fstps	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	asinf,1b-asinf

.global acosf
	.type	acosf,@function
acosf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	fld	%st
	fmul	%st
	fld1
	fsubp
	fsqrt
	fxch	%st(1)
	fpatan
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	acosf,1b-acosf

.global atan2f
	.type	atan2f,@function
atan2f:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	movss	%xmm1, (%rsp)
	flds	(%rsp)
	fpatan
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	atan2f,1b-atan2f

.global atanf
	.type	atanf,@function
atanf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	fld1
	fpatan
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	atanf,1b-atanf

	.section	.note.GNU-stack,"",@progbits
