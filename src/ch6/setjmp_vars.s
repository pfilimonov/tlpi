	.file	"setjmp_vars.c"
	.text
	.local	env
	.comm	env,200,32
	.section	.rodata
	.align 8
.LC0:
	.string	"Inside doJump(): nvar=%d rvar=%d vvar=%d\n"
	.text
	.type	doJump, @function
doJump:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-12(%rbp), %ecx
	movl	-8(%rbp), %edx
	movl	-4(%rbp), %eax
	leaq	.LC0(%rip), %rdi
	movl	%eax, %esi
	movl	$0, %eax
	call	printf@PLT
	leaq	env(%rip), %rax
	movl	$1, %esi
	movq	%rax, %rdi
	call	longjmp@PLT
	.cfi_endproc
.LFE6:
	.size	doJump, .-doJump
	.section	.rodata
	.align 8
.LC1:
	.string	"After longjmp(): nvar=%d rvar=%d vvar=%d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	movl	$111, -4(%rbp)
	movl	$222, -24(%rbp)
	movl	$333, -8(%rbp)
	leaq	env(%rip), %rax
	movq	%rax, %rdi
	call	_setjmp@PLT
	endbr64
	testl	%eax, %eax
	jne	.L4
	movl	$777, -4(%rbp)
	movl	$888, %esi
	movl	$999, -8(%rbp)
	movl	-8(%rbp), %edx
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	doJump
	jmp	.L5
.L4:
	movl	-8(%rbp), %edx
	movl	-4(%rbp), %eax
	leaq	.LC1(%rip), %rdi
	movl	%edx, %ecx
	movl	-24(%rbp), %edx
	movl	%eax, %esi
	movl	$0, %eax
	call	printf@PLT
.L5:
	movl	$0, %edi
	call	exit@PLT
	.cfi_endproc
.LFE7:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 15.2.0-16ubuntu1) 15.2.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
