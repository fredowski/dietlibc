#include "dietfeatures.h"
#include "syscalls.h"

#if 0

// this does not work because the vdso prototypes are different, so
// #ifdef out and fall back to regular syscall for now

.text
.global getrandom
#ifdef __PIE__
.hidden getrandom
#endif
.type getrandom,@function
getrandom:
#ifdef WANT_VALGRIND_SUPPORT
#ifdef __PIC__
	mov __valgrind@GOTPCREL(%rip), %rax
	cmpb $1, (%rax)
#else
	cmpb $1, __valgrind(%rip)
#endif
	je .Lvalgrind
#endif
	movq gtod(%rip), %rax
	test %rax,%rax
	jz .L1
.L2:
	callq *%rax
	jmp __error_unified_syscall
.L1:
	push %rsi
	push %rdi
	push %rdx
#ifdef __PIC__
	movq __vdso@GOTPCREL(%rip), %rax
	movq (%rax),%rdi
	movq funcname@GOTPCREL(%rip), %rsi
	call vdso_dlsym@PLT
#else
	movq __vdso(%rip),%rdi
	movq $funcname, %rsi
	call vdso_dlsym
#endif
	pop %rdx
	pop %rdi
	pop %rsi
	jmp .L2

#ifdef WANT_VALGRIND_SUPPORT
.Lvalgrind:
	mov $__NR_getrandom, %ax
	jmp __unified_syscall
#endif
.Lhere2:
	.size	 getrandom,.Lhere2-getrandom
.local gtod
.comm gtod,8,8
.section .rodata.str1.1,"aMS",@progbits,1
.local funcname
funcname:
	.string "__vdso_getrandom"

#endif

syscall(getrandom,getrandom)

