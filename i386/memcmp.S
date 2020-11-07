.text
.global memcmp
.type	memcmp,function
memcmp:
.global bcmp
.type bcmp,function
bcmp:
	pushl	%esi
	pushl	%edi
	xorl	%eax,%eax
	movl	%esp,%ecx
	movl	12(%ecx),%esi
	movl	16(%ecx),%edi
	movl	20(%ecx),%ecx
	cld
	rep cmpsb
	jz	.Lout
	sbbl	%eax,%eax
	orl	$1,%eax
.Lout:
	popl %edi
	popl %esi
	ret
.size	memcmp,.-memcmp
.size	bcmp,.-bcmp
	.section	.note.GNU-stack,"",@progbits
