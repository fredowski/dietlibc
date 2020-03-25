####################################################################################
# Assembler math routines for x86_64 ### only uses FPU and SSE instructions
# Author: Emil
####################################################################################

.text

.global __finexp
        .type   __finexp,@function
__finexp:
	fst	%st(1)
	frndint
	fst	%st(2)
	fsubrp
	f2xm1
	fld1
	faddp
	fscale
	ret

.global __fmod2pi
        .type   __fmod2pi,@function
__fmod2pi:
	fldpi
	fadd	%st(0)
	fxch
2:
	fprem
	fstsw	%ax
	sahf
	jp	2b
	fstp	%st(1)
	ret

.data

.global __half,__shalf,__neg_zero,__sneg_zero
.type	__half,@object
.size	__half,8
.size	__neg_zero,8
.size	__shalf,4
.size	__sneg_zero,4
__half: .quad 0x3fe0000000000000
__neg_zero: .quad 0x8000000000000000
__shalf: .long 0x3f000000
__sneg_zero: .long 0x80000000

	.section	.note.GNU-stack,"",@progbits
