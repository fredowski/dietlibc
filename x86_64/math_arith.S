####################################################################################
# Assembler math routines for x86_64 ### only uses FPU and SSE instructions
# Author: Emil
####################################################################################

.text

# |ret| = |x|, sign(ret) = sign(y)
.global copysign
	.type	copysign,@function
copysign:
	movq	%xmm0, %rax
	shl	$1, %rax	# drop sign
	movq	%xmm1, %rcx
	shl	$1, %rcx	# sign in carry
	rcr	$1, %rax
	movq	%rax, %xmm0
	ret
1:
	.size	copysign,1b-copysign

.global frexp
	.type	frexp,@function
frexp:
	movq	%xmm0, %rax
	xor	%rcx, %rcx
	shl	$1, %rax
	rcl	$1, %rcx	# save sign
	shl	$11, %rcx
	or	$0x3fe, %rcx	# exponent 0
	shl	$52, %rcx
	shr	$53, %rax
	cmp	$0, %eax	# subnormal number
	jnz	2f
	mov	%eax, (%rdi)
	shl	$1, %rcx
	rcr	$1, %rax
	jmp	3f
2:
	sub	$0x3fe, %eax
	mov	%eax, (%rdi)
	movq	%xmm0, %rax
	shl	$12, %rax
	shr	$12, %rax	# remove exponent
	or	%rcx, %rax
3:
	movq	%rax, %xmm0
	ret
1:
	.size	frexp,1b-frexp

.global fabs
	.type	fabs,@function
fabs:
	movq	%xmm0, %rax
	xor	%rcx, %rcx
	sub	$1, %rcx
	shr	$1, %rcx
	and	%rcx, %rax	# mask the sign bit
	movq	%rax, %xmm0
	ret
1:
	.size	fabs,1b-fabs

.global trunc
	.type	trunc,@function
trunc:
	movq	   %xmm0, %rcx
	cvttsd2si  %xmm0, %rax
	cvtsi2sd   %rax, %xmm0
	cmp	   $0, %rax	# restore the sign for 0
	jne	   2f
	shl	   $1, %rcx	# test negative
	jnc	   2f
	movsd	   (__neg_zero), %xmm0
2:
	ret
1:
	.size	trunc,1b-trunc

.global round
	.type	round,@function
round:
	movq	   %xmm0, %rax
	mov	   %rax, %rcx
	shl	   $1, %rax	# sign in carry
	jc	   2f
	addsd	   (__half), %xmm0
	jmp	   3f
2:
	subsd	   (__half), %xmm0
3:
	cvttsd2si  %xmm0, %rax
	cmp	   $0, %rax	# restore the sign for 0
	jne	   4f
	shl	   $1, %rcx	# original sign in carry
	jnc	   4f
	movsd	   (__neg_zero), %xmm0
	ret
4:
	cvtsi2sd   %rax, %xmm0
	ret
1:
	.size	round,1b-round

.global floor
	.type	floor,@function
floor:
	movq	   %xmm0, %rax
	shl	   $1, %rax	# sign in carry
	cvttsd2si  %xmm0, %rax
	jnc	   2f
	cvtsi2sd   %rax, %xmm1
	comisd	   %xmm0, %xmm1
	jz	   2f
	sub	   $1, %rax
2:
	cvtsi2sd   %rax, %xmm0
	ret
1:
	.size	floor,1b-floor

.global ceil
	.type	ceil,@function
ceil:
	movq	   %xmm0, %rax
	mov	   %rax, %rcx
	shl	   $1, %rax	# sign in carry
	cvttsd2si  %xmm0, %rax
	jc	   2f
	cvtsi2sd   %rax, %xmm1
	comisd	   %xmm0, %xmm1
	jz	   2f
	add	   $1, %rax
2:
	cmp	   $0, %rax	# restore the sign for 0
	jne	   3f
	shl	   $1, %rcx	# original sign in carry
	jnc	   3f
	movsd	   (__neg_zero), %xmm0
	ret
3:
	cvtsi2sd   %rax, %xmm0
	ret
1:
	.size	ceil,1b-ceil

.global rint
	.type	rint,@function
rint:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	frndint
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	rint,1b-rint

.global fdim
	.type	fdim,@function
fdim:
	subsd	%xmm1, %xmm0
	xorpd	%xmm1, %xmm1
	maxsd	%xmm1, %xmm0
	ret
1:
	.size	fdim,1b-fdim

.global fmod
	.type	fmod,@function
fmod:
	sub	$8, %rsp
	movsd	%xmm1, (%rsp)
	fldl	(%rsp)
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
2:
	fprem
	fstsw	%ax
	sahf
	jp	2b
	fstp	%st(1)
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	fmod,1b-fmod

.global drem,remainder
	.type	drem,@function
	.type	remainder,@function
drem:
remainder:
	movq	   %xmm1, %rax
	cmp	   $0, %rax
	jnz	   2f
	sub	   $1, %rax
	shl	   $1, %rax
	movq	   %rax, %xmm0
	ret
2:
	movq	   %xmm0, %rcx
	divsd	   %xmm1, %xmm0
	movq	   %xmm0, %rax
	shl	   $1, %rax	# sign in carry
	jc	   3f
	addsd	   (__half), %xmm0
	jmp	   4f
3:
	subsd	   (__half), %xmm0
4:
	cvttsd2si  %xmm0, %rax
	cvtsi2sd   %rax, %xmm0
	mulsd	   %xmm0, %xmm1
	movq	   %rcx, %xmm0
	subsd	   %xmm1, %xmm0
	ret
1:
	.size	drem,1b-drem
	.size	remainder,1b-remainder

.global fma
	.type	fma,@function
fma:
	sub	$8, %rsp
	movsd	%xmm0, (%rsp)
	fldl	(%rsp)
	movsd	%xmm1, (%rsp)
	fmull	(%rsp)
	movsd	%xmm2, (%rsp)
	faddl	(%rsp)
	fstpl	(%rsp)
	movsd	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	fma,1b-fma

	.section	.note.GNU-stack,"",@progbits
