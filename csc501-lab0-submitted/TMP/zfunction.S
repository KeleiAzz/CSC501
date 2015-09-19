.section	.data
.section	.text
.globl	zfunction

zfunction:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$0xFFFFF00F, 8(%ebp)
	movl	8(%ebp), %eax
	shll	$0x8, %eax
	leave
	ret
