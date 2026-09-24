	.file	"setjmp_vars.c"
	.text
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"Inside doJump(): nvar=%d rvar=%d vvar=%d\n"
	.align 8
.LC1:
	.string	"After longjmp(): nvar=%d rvar=%d vvar=%d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB40:
	.cfi_startproc
	endbr64
	pushq	%rax
	.cfi_def_cfa_offset 16
	popq	%rax
	.cfi_def_cfa_offset 8
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movl	$333, 12(%rsp)
	leaq	env(%rip), %rdi
	call	_setjmp@PLT
	endbr64
	testl	%eax, %eax
	je	.L5
	movl	12(%rsp), %r8d
	movl	$222, %ecx
	movl	$111, %edx
	leaq	.LC1(%rip), %rsi
	movl	$2, %edi
	movl	$0, %eax
	call	__printf_chk@PLT
	movl	$0, %edi
	call	exit@PLT
.L5:
	movl	$999, 12(%rsp)
	movl	12(%rsp), %r8d
	movl	$888, %ecx
	movl	$777, %edx
	leaq	.LC0(%rip), %rsi
	movl	$2, %edi
	call	__printf_chk@PLT
	movl	$1, %esi
	leaq	env(%rip), %rdi
	call	__longjmp_chk@PLT
	.cfi_endproc
.LFE40:
	.size	main, .-main
	.local	env
	.comm	env,200,32
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
