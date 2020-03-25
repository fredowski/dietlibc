####################################################################################
# Assembler math routines for x86_64 ### only uses FPU and SSE instructions
# Author: Emil
####################################################################################

.text

.global ilogb
	.type	ilogb,@function
ilogb:
	movq	%xmm0, %rax
	cmp	$0, %rax	# zero
	jnz	2f
	stc
	rcr	$1, %eax	# FP_ILOGB0
	ret
2:
	shl	$1, %rax
	shr	$53, %rax
	sub	$0x3ff, %eax
	ret
1:
	.size	ilogb,1b-ilogb

.global logb
	.type	logb,@function
logb:
	movq	%xmm0, %rax
	cmp	$0, %rax	# zero
	jnz	2f
	sub	$1, %rax
	shl	$52, %rax	# -inf
	movq	%rax, %xmm0
	ret
2:
	shl	$1, %rax
	shr	$53, %rax
	sub	$0x3ff, %rax
	cvtsi2sd %rax, %xmm0
	ret
1:
	.size	logb,1b-logb

.global ldexp
	.type	ldexp,@function
ldexp:
	sub	$8, %rsp
	mov	%edi, (%rsp)
	fildl	(%rsp)
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	fscale
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	ldexp,1b-ldexp

.global sqrt
	.type	sqrt,@function
sqrt:
	pxor	%xmm1, %xmm1
	ucomisd %xmm1, %xmm0
	jb	2f
	sqrtsd	%xmm0, %xmm0
	ret
2:
	pxor	%xmm0, %xmm0
	divpd	%xmm0, %xmm0
	ret
1:
	.size	sqrt,1b-sqrt

.global hypot
	.type	hypot,@function
hypot:
	mulsd	%xmm0, %xmm0
	mulsd	%xmm1, %xmm1
	addsd	%xmm1, %xmm0
	sqrtsd	%xmm0, %xmm0
	ret
1:
	.size	hypot,1b-hypot

.global exp,expm1,exp2,exp10,pow10
	.type	exp,@function
	.type	expm1,@function
	.type	exp2,@function
	.type	exp10,@function
	.type	pow10,@function
exp:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl2e
	fmull	(%rsp)
	fst	%st(1)
	frndint
	fst	%st(2)
	fsubrp
	f2xm1
	fld1
	faddp	%st(0), %st(1)
	fscale
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	 exp,1b-exp
# exp(x)-1
expm1:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl2e
	fmull	(%rsp)		# x*log2(e)
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
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	expm1,1b-expm1
exp2:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	fst	%st(1)
	frndint
	fst	%st(2)
	fsubrp
	f2xm1
	fld1
	faddp	%st(0), %st(1)
	fscale
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	 exp2,1b-exp2
pow10:
exp10:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl2t
	fmull	(%rsp)
	fst	%st(1)
	frndint
	fst	%st(2)
	fsubrp
	f2xm1
	fld1
	faddp	%st(0), %st(1)
	fscale
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	 pow10,1b-pow10
	.size	 exp10,1b-exp10

.global log
	.type	log,@function
log:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldln2
	fldl	(%rsp)
	fyl2x
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	log,1b-log

.global log1p
	.type	log1p,@function
log1p:
	sub	$8, %rsp
	push	$0x3e95f61a	# 1 - sqrt(2)/2
	pop	%rax
	fldl	(%rsp)
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	fabs
	fcompp			# compare and pop twice
	fstsw	%ax
	movsd	%xmm0, (%rsp)
	fldln2
	fldl	(%rsp)
	sahf			# |x| >= 1 - sqrt(2)/2
	jc 2f
	fyl2xp1
	jmp 3f
2:
	fld1			# x = x + 1
	faddp
	fyl2x
3:
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	log1p,1b-log1p

.global log2
	.type	log2,@function
log2:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fld1
	fldl	(%rsp)
	fyl2x
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	 log2,1b-log2

.global log10
	.type	log10,@function
log10:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldlg2
	fldl	(%rsp)
	fyl2x
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	 log10,1b-log10

.global pow
	.type	pow,@function
pow:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	movsd	%xmm1, (%rsp)
	fldl	(%rsp)
# x^y; st(0)=y, st(1)=x
	cvttsd2si %xmm1, %ecx	# integer power
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
	xor	%rax, %rax
	sub	$1, %rax
	shl	$53, %rax
	shr	$1, %rax # return +inf
	movq	%rax, %xmm0
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
	fstpl	(%rsp)
	fcompp		# drop two stack
	jmp	2f
4:
	fstpl	(%rsp)
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
	fstpl	(%rsp)
2:
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	pow,1b-pow

	.section	.note.GNU-stack,"",@progbits
