####################################################################################
# Assembler math routines for x86_64 ### only uses FPU and SSE instructions
# Author: Emil
####################################################################################

.text

# |ret| = |x|, sign(ret) = sign(y)
.global copysignf
	.type	copysignf,@function
copysignf:
	movd	%xmm0, %eax
	shl	$1, %eax	# drop sign
	movd	%xmm1, %ecx
	shl	$1, %ecx	# sign in carry
	rcr	$1, %eax
	movd	%eax, %xmm0
	ret
1:
	.size	copysignf,1b-copysignf

.global frexpf
	.type	frexpf,@function
frexpf:
	movd	%xmm0, %eax
	xor	%ecx, %ecx
	shl	$1, %eax
	rcl	$1, %ecx	# save sign
	shl	$8, %ecx
	or	$0x7e, %ecx	# exponent 0
	shl	$23, %ecx
	shr	$24, %eax
	cmp	$0, %eax	# subnormal number
	jnz	2f
	mov	%eax, (%rdi)
	shl	$1, %ecx
	rcr	$1, %eax
	jmp	3f
2:
	sub	$0x7e, %eax
	mov	%eax, (%rdi)
	movd	%xmm0, %eax
	shl	$9, %eax
	shr	$9, %eax	# remove exponent
	or	%ecx, %eax
3:
	movd	%eax, %xmm0
	ret
1:
	.size	frexpf,1b-frexpf

.global fabsf
	.type	fabsf,@function
fabsf:
	movd	%xmm0, %eax
	xor	%ecx, %ecx
	sub	$1, %ecx
	shr	$1, %ecx
	and	%ecx, %eax	# mask the sign bit
	movd	%eax, %xmm0
	ret
1:
	.size	fabsf,1b-fabsf

.global truncf
	.type	truncf,@function
truncf:
	movd	   %xmm0, %ecx
	cvttss2si  %xmm0, %rax
	cvtsi2sd   %rax, %xmm0
	cmp	   $0, %rax	# restore the sign for 0
	jne	   2f
	shl	   $1, %ecx	# test negative
	jnc	   2f
	movsd	   (__neg_zero), %xmm0
2:
	cvtsd2ss   %xmm0, %xmm0
	ret
1:
	.size	truncf,1b-truncf

.global roundf
	.type	roundf,@function
roundf:
	movd	   %xmm0, %eax
	mov	   %eax, %ecx
	shl	   $1, %eax	# sign in carry
	jc	   2f
	addss	   (__shalf), %xmm0
	jmp	   3f
2:
	subss	   (__shalf), %xmm0
3:
	cvttss2si  %xmm0, %eax
	cmp	   $0, %eax	# restore the sign for 0
	jne	   4f
	shl	   $1, %ecx	# original sign in carry
	jnc	   4f
	movss	   (__sneg_zero), %xmm0
	ret
4:
	cvtsi2ss   %eax, %xmm0
	ret
1:
	.size	roundf,1b-roundf

.global floorf
	.type	floorf,@function
floorf:
	movd	   %xmm0, %eax
	shl	   $1, %eax	# sign in carry
	cvttss2si  %xmm0, %eax
	jnc	   2f
	cvtsi2ss   %eax, %xmm1
	comiss	   %xmm0, %xmm1
	jz	   2f
	sub	   $1, %eax
2:
	cvtsi2ss   %eax, %xmm0
	ret
1:
	.size	floorf,1b-floorf

.global ceilf
	.type	ceilf,@function
ceilf:
	movd	   %xmm0, %eax
	mov	   %eax, %ecx
	shl	   $1, %eax	# sign in carry
	cvttss2si  %xmm0, %eax
	jc	   2f
	cvtsi2ss   %eax, %xmm1
	comiss	   %xmm0, %xmm1
	jz	   2f
	add	   $1, %eax
2:
	cmp	   $0, %eax	# restore the sign for 0
	jne	   3f
	shl	   $1, %ecx	# original sign in carry
	jnc	   3f
	movsd	   (__neg_zero), %xmm0
	cvtsd2ss   %xmm0, %xmm0
	ret
3:
	cvtsi2ss   %eax, %xmm0
	ret
	cvtsi2ss   %eax, %xmm0
	ret
1:
	.size	ceilf,1b-ceilf

.global rintf
	.type	rintf,@function
rintf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	frndint
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	rintf,1b-rintf

.global fdimf
	.type	fdimf,@function
fdimf:
	subss	%xmm1, %xmm0
	xorps	%xmm1, %xmm1
	maxss	%xmm1, %xmm0
	ret
1:
	.size	fdimf,1b-fdimf

.global fmodf
	.type	fmodf,@function
fmodf:
	sub	$8, %rsp
	movss	%xmm1, (%rsp)
	flds	(%rsp)
	movss	%xmm0, (%rsp)
	flds	(%rsp)
2:
	fprem
	fstsw	%ax
	sahf
	jp	2b
	fstp	%st(1)
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	fmodf,1b-fmodf

.global dremf,remainderf
	.type	dremf,@function
	.type	remainderf,@function
dremf:
remainderf:
	movd	   %xmm1, %eax
	cmp	   $0, %eax
	jnz	   2f
	sub	   $1, %eax
	shl	   $1, %eax
	movd	   %eax, %xmm0
	ret
2:
	movd	   %xmm0, %ecx
	divss	   %xmm1, %xmm0
	movd	   %xmm0, %eax
	shl	   $1, %eax	# sign in carry
	jc	   3f
	addss	   (__shalf), %xmm0
	jmp	   4f
3:
	subss	   (__shalf), %xmm0
4:
	cvttss2si  %xmm0, %eax
	cvtsi2ss   %eax, %xmm0
	mulss	   %xmm0, %xmm1
	movd	   %ecx, %xmm0
	subss	   %xmm1, %xmm0
	ret
1:
	.size	dremf,1b-dremf
	.size	remainderf,1b-remainderf

.global fmaf
	.type	fmaf,@function
fmaf:
	sub	$8, %rsp
	movss	%xmm0, (%rsp)
	flds	(%rsp)
	movss	%xmm1, (%rsp)
	fmuls	(%rsp)
	movss	%xmm2, (%rsp)
	fadds	(%rsp)
	fstps	(%rsp)
	movss	(%rsp), %xmm0
	add	$8, %rsp
	ret
1:
	.size	fmaf,1b-fmaf

	.section	.note.GNU-stack,"",@progbits
