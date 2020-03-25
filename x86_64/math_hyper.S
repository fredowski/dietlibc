####################################################################################
# Assembler math routines for x86_64 ### only uses FPU and SSE instructions
# Author: Emil
####################################################################################

.text

# sinh(x)=(exp(x)-exp(-x))/2
# cosh(x)=(exp(x)+exp(-x))/2
# tanh(x)=sinh(x)/cosh(x)
.global sinh
	.type	sinh,@function
sinh:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl2e
	fmull	(%rsp)
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call	__finexp@PLT
	PIC_RESTORE
#else
	call	__finexp
#endif
	fld1
	fdiv	%st(1),%st(0)	# st(0)=1/exp(x), st(1)=exp(x)
	fsubrp	%st(0),%st(1)
	fldl	(__half)
	fmulp
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	sinh,1b-sinh

.global cosh
	.type	cosh,@function
cosh:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl2e
	fmull	(%rsp)
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call	__finexp@PLT
	PIC_RESTORE
#else
	call	__finexp
#endif
	fld1
	fdiv	%st(1),%st(0)	# st(0)=1/exp(x), st(1)=exp(x)
	faddp	%st,%st(1)
	fldl	(__half)
	fmulp
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	cosh,1b-cosh

.global tanh
	.type	tanh,@function
tanh:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl2e
	fmull	(%rsp)
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call	__finexp@PLT
	PIC_RESTORE
#else
	call	__finexp
#endif
	fld1
	fdiv	%st(1),%st	# st(0)=1/exp(x), st(1)=exp(x)
	fst	%st(2)		# st(2)=1/exp(x)
	fadd	%st(1),%st(0)
	fstp	%st(3)		# st(2)=exp(x)+exp(-x), st(1)=exp(-x), st(0)=exp(x)
	fsubp			# st(1)=exp(x)+exp(-x), st(0)=exp(x)-exp(-x)
	fdivp
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	tanh,1b-tanh

.global asinh
	.type	asinh,@function
asinh:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	fld	%st(0)
	fmul	%st(0),%st(0)
	fld1
	faddp	%st(0),%st(1)
	fsqrt
	faddp	%st(0),%st(1)
	fldln2
	fxch
	fyl2x
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	asinh,1b-asinh

.global acosh
	.type	acosh,@function
acosh:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	fld	%st(0)
	fmul	%st(0),%st(0)
	fld1
	fsubrp	%st(0),%st(1)	# st1=st1-st0; pop
	fsqrt
	faddp	%st(0),%st(1)
	fldln2
	fxch
	fyl2x
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	acosh,1b-acosh

.global atanh
	.type	atanh,@function
atanh:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	fst	%st(1)
	fld1			# st0=1, st1=x, st2=x
	fadd	%st(0),%st(2)	# st0=1, st1=x, st2=x+1
	fsubp	%st(0),%st(1)	# st0=1-x, st1=x+1
	fdivrp	%st(0),%st(1)
	fsqrt
	fldln2
	fxch
	fyl2x
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	atanh,1b-atanh

	.section	.note.GNU-stack,"",@progbits
