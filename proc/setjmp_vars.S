	.text
	.file	"setjmp_vars.c"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	$0, -4(%rbp)
	movl	%edi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$111, -20(%rbp)
	movl	$222, -24(%rbp)
	movl	$333, -28(%rbp)         # imm = 0x14D
	leaq	env(%rip), %rdi
	callq	_setjmp@PLT
	cmpl	$0, %eax
	jne	.LBB0_2
# %bb.1:
	movl	$777, -20(%rbp)         # imm = 0x309
	movl	$888, -24(%rbp)         # imm = 0x378
	movl	$999, -28(%rbp)         # imm = 0x3E7
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	movl	-28(%rbp), %edx
	callq	doJump
	jmp	.LBB0_3
.LBB0_2:
	movl	-20(%rbp), %esi
	movl	-24(%rbp), %edx
	movl	-28(%rbp), %ecx
	leaq	.L.str(%rip), %rdi
	movb	$0, %al
	callq	printf@PLT
	movl	%eax, -32(%rbp)         # 4-byte Spill
.LBB0_3:
	xorl	%edi, %edi
	callq	exit@PLT
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function doJump
	.type	doJump,@function
doJump:                                 # @doJump
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-4(%rbp), %esi
	movl	-8(%rbp), %edx
	movl	-12(%rbp), %ecx
	leaq	.L.str.1(%rip), %rdi
	movb	$0, %al
	callq	printf@PLT
	leaq	env(%rip), %rdi
	movl	$1, %esi
	movl	%eax, -16(%rbp)         # 4-byte Spill
	callq	longjmp@PLT
.Lfunc_end1:
	.size	doJump, .Lfunc_end1-doJump
	.cfi_endproc
                                        # -- End function
	.type	env,@object             # @env
	.local	env
	.comm	env,200,16
	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"After longjmp(): nvar=%d rvar=%d vvar=%d\n"
	.size	.L.str, 42

	.type	.L.str.1,@object        # @.str.1
.L.str.1:
	.asciz	"Inside doJump(): nvar=%d rvar=%d vvar=%d\n"
	.size	.L.str.1, 42


	.ident	"clang version 7.0.0 (tags/RELEASE_700/final)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym main
	.addrsig_sym _setjmp
	.addrsig_sym doJump
	.addrsig_sym printf
	.addrsig_sym exit
	.addrsig_sym longjmp
	.addrsig_sym env
