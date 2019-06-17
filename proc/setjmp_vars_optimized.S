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
	subq	$16, %rsp
	movl	$333, -4(%rbp)          # imm = 0x14D
	leaq	env(%rip), %rdi
	callq	_setjmp@PLT
	testl	%eax, %eax
	jne	.LBB0_2
# %bb.1:
	movl	$999, -4(%rbp)          # imm = 0x3E7
	movl	-4(%rbp), %edi
	callq	doJump
.LBB0_2:
	movl	-4(%rbp), %ecx
	leaq	.L.str(%rip), %rdi
	movl	$111, %esi
	movl	$222, %edx
	xorl	%eax, %eax
	callq	printf@PLT
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
	movl	%edi, %ecx
	leaq	.L.str.1(%rip), %rdi
	movl	$777, %esi              # imm = 0x309
	movl	$888, %edx              # imm = 0x378
	xorl	%eax, %eax
	callq	printf@PLT
	leaq	env(%rip), %rdi
	movl	$1, %esi
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
	.addrsig_sym env
