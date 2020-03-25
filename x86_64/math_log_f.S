####################################################################################
# Assembler math routines for x86_64 ### only uses FPU and SSE instructions
# Author: Emil
####################################################################################

.text

.global ilogbf
	.type	ilogbf,@function
ilogbf:
	movd	%xmm0, %eax
	cmp	$0, %eax	# zero
	jnz	2f
	stc
	rcr	$1, %eax	# FP_ILOGB0
	ret
2:
	shl	$1, %eax
	shr	$24, %eax
	sub	$0x7f, %eax
	cdqe
	ret
1:
	.size	ilogbf,1b-ilogbf

.global logbf
	.type	logbf,@function
logbf:
	movd	%xmm0, %eax
	cmp	$0, %eax	# zero
	jnz	2f
	sub	$1, %eax
	shl	$23, %eax	# -inf
	movd	%eax, %xmm0
	ret
2:
	shl	$1, %eax
	shr	$24, %eax
	sub	$0x7f, %eax
	cvtsi2ss %eax, %xmm0
	ret
1:
	.size	logbf,1b-logbf

.global ldexpf
	.type	ldexpf,@function
ldexpf:
	sub	$8, %rsp
	mov	%edi, (%rsp)
	fildl	(%rsp)
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	fscale
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	ldexpf,1b-ldexpf

.global sqrtf
	.type	sqrtf,@function
sqrtf:
	pxor	%xmm1, %xmm1
	ucomiss %xmm1, %xmm0
	jb	2f
	sqrtss	%xmm0, %xmm0
	ret
2:
	pxor	%xmm0, %xmm0
	divps	%xmm0, %xmm0
	ret
1:
	.size	sqrtf,1b-sqrtf

.global hypotf
	.type	hypotf,@function
hypotf:
	mulss	%xmm0, %xmm0
	mulss	%xmm1, %xmm1
	addss	%xmm1, %xmm0
	sqrtss	%xmm0, %xmm0
	ret
1:
	.size	hypotf,1b-hypotf

.global expf,expm1f,exp2f,exp10f,pow10f
	.type	expf,@function
	.type	expm1f,@function
	.type	exp2f,@function
	.type	exp10f,@function
	.type	pow10f,@function
expf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	fldl2e
	fmuls	(%rsp)
	fst	%st(1)
	frndint
	fst	%st(2)
	fsubrp
	f2xm1
	fld1
	faddp	%st(0), %st(1)
	fscale
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	 expf,1b-expf
# exp(x)-1
expm1f:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	fldl2e
	fmuls	(%rsp)		# x*log2(e)
	fld	%st(0)		# x*log2(e)			  x*log2(e)
	frndint			# int(x*log2(e))		  x*log2(e)
	fxch			# x*log2(e)			  int(x*log2(e))
	fsub	%st(1),%st(0)	# frac(x*log2(e))		  int(x*log2(e))
	f2xm1			# 2^(fract(x*log2(e)))-1	  int(x*log2(e))
	fscale			# 2^(x*log2(e))-2^int(x*log2(e))  int(x*log2(e))
	fxch			# int(x*log2(e))		  2^(x*log2(e))-2^int(x*log2(e))
	fld1			# 1				  int(x*log2(e))		  2^(x*log2(e))-2^int(x*log2(e))
	fscale			# 2^int(x*log2(e))		  int(x*log2(e))		  2^(x*log2(e))-2^int(x*log2(e))
	fstp	 %st(1)		# 2^int(x*log2(e))		  2^(x*log2(e))-2^int(x*log2(e))
	fld1			# 1				  2^int(x*log2(e))		  2^(x*log2(e))-2^int(x*log2(e))
	fsubrp	 %st(1)		# 2^int(x*log2(e))-1		  2^(x*log2(e))-2^int(x*log2(e))
	faddp	 %st(1)
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	expm1f,1b-expm1f
exp2f:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	fst	%st(1)
	frndint
	fst	%st(2)
	fsubrp
	f2xm1
	fld1
	faddp	%st(0), %st(1)
	fscale
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	 exp2f,1b-exp2f
pow10f:
exp10f:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	fldl2t
	fmuls	(%rsp)
	fst	%st(1)
	frndint
	fst	%st(2)
	fsubrp
	f2xm1
	fld1
	faddp	%st(0), %st(1)
	fscale
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	 pow10f,1b-pow10f
	.size	 exp10f,1b-exp10f

.global logf
	.type	logf,@function
logf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	fldln2
	flds	(%rsp)
	fyl2x
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	logf,1b-logf

.global log1pf
	.type	log1pf,@function
log1pf:
	sub	$8, %rsp
	push	$0x3e95f61a	# 1 - sqrt(2)/2
	pop	%rax
	flds	(%rsp)
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	fabs
	fcompp			# compare and pop twice
	fstsw	%ax
	movss	%xmm0, (%rsp)
	fldln2
	flds	(%rsp)
	sahf			# |x| >= 1 - sqrt(2)/2
	jc 2f
	fyl2xp1
	jmp 3f
2:
	fld1			# x = x + 1
	faddp
	fyl2x
3:
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	log1pf,1b-log1pf

.global log2f
	.type	log2f,@function
log2f:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	fld1
	flds	(%rsp)
	fyl2x
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	log2f,1b-log2f

.global log10f
	.type	log10f,@function
log10f:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	fldlg2
	flds	(%rsp)
	fyl2x
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	 log10f,1b-log10f

.global powf
	.type	powf,@function
powf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	movss	%xmm1, (%rsp)
	flds	(%rsp)
# x^y; st(0)=y, st(1)=x
	cvttss2si %xmm1, %ecx	# integer power
	ftst		# y = 0 ?
	fstsw	%ax
	fld1		# st(0)=1, st(1)=y, st(2)=x
	sahf
	jz	5f	# return 1
	fcomp	%st(1)	# y = 1 ?
	fstsw	%ax
	fxch		# st(0)=x, st(1)=y
	sahf
	jz	4f	# return x
	ftst		# x = 0 ?
	fstsw	%ax
	sahf
	ja	6f	# x > 0
	jnz	7f
	cmp	$0, %ecx
	jg	4f	# zero and positive power
	fcompp		# drop two
	mov	$0xff, %eax
	shl	$23, %eax # return +inf
	movd	%eax, %xmm0
	add	$8, %rsp
	ret
7:
	fxch		# st(0)=y, st(1)=x
	fld	%st(0)	# st(0)=y, st(1)=y, st(2)=x
	frndint		# st(0)=int(y)
	fcomp	%st(1)	# y = int(y)?
	fstsw	%ax
	fxch
	sahf
	jnz	6f	# fyl2x -> st(0) = NaN
	fchs		# st(0) = -x
	fyl2x
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call __finexp@PLT
#else
	call __finexp
#endif
	test	$1, %ecx # y even or odd ?
	jz	3f
	fchs
	jmp	3f
5:
	fstps	(%rsp)
	fcompp		# drop two stack
	jmp	2f
4:
	fstps	(%rsp)
	fstp	%st(0)	# drop one stack
	jmp	2f
6:
	fyl2x
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call __finexp@PLT
#else
	call __finexp
#endif
3:
	fstps	(%rsp)
2:
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	powf,1b-powf

	.section	.note.GNU-stack,"",@progbits
