####################################################################################
# Assembler math routines for x86_64 ### only uses FPU and SSE instructions
# Author: Emil
####################################################################################

.text

# sinh(x)=(exp(x)-exp(-x))/2
# cosh(x)=(exp(x)+exp(-x))/2
# tanh(x)=sinh(x)/cosh(x)
.global sinhf
	.type	sinhf,@function
sinhf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	fldl2e
	fmuls	(%rsp)
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
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	sinhf,1b-sinhf

.global coshf
	.type	coshf,@function
coshf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	fldl2e
	fmuls	(%rsp)
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call	__finexp@PLT
	PIC_RESTORE
#else
	call __finexp
#endif
	fld1
	fdiv	%st(1),%st(0)	# st(0)=1/exp(x), st(1)=exp(x)
	faddp	%st,%st(1)
	fldl	(__half)
	fmulp
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	coshf,1b-coshf

.global tanhf
	.type	tanhf,@function
tanhf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	fldl2e
	fmuls	(%rsp)
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
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	tanhf,1b-tanhf

.global asinhf
	.type	asinhf,@function
asinhf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	fld	%st(0)
	fmul	%st(0),%st(0)
	fld1
	faddp	%st(0),%st(1)
	fsqrt
	faddp	%st(0),%st(1)
	fldln2
	fxch
	fyl2x
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	asinhf,1b-asinhf

.global acoshf
	.type	acoshf,@function
acoshf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	fld	%st(0)
	fmul	%st(0),%st(0)
	fld1
	fsubrp	%st(0),%st(1)	# st1=st1-st0; pop
	fsqrt
	faddp	%st(0),%st(1)
	fldln2
	fxch
	fyl2x
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	acoshf,1b-acoshf

.global atanhf
	.type	atanhf,@function
atanhf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	fst	%st(1)
	fld1			# st0=1, st1=x, st2=x
	fadd	%st(0),%st(2)	# st0=1, st1=x, st2=x+1
	fsubp	%st(0),%st(1)	# st0=1-x, st1=x+1
	fdivrp	%st(0),%st(1)
	fsqrt
	fldln2
	fxch
	fyl2x
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	atanhf,1b-atanhf

	.section	.note.GNU-stack,"",@progbits
