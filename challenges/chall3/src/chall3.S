.data
msg:
	.string "overflow:\n"

.text
.globl _start
_start:

	sub $0x20, %rsp

	mov $1, %rax
	mov $1, %rdi
	movabs $msg, %rsi
	mov $10, %rdx
	syscall

	xor %rax, %rax
	xor %rdi, %rdi
	mov %rsp, %rsi
	mov $0x100, %rdx
	syscall

	add $0x20, %rsp
	mov $1, %rdi
	mov $3, %rsi
	mov $3, %rdx
	mov $7, %rcx

	cmp $0x20, %rax
	jz  exit

	movq $0x23, 8(%rsp)
	retfq

exit:
	movq $60, %rax
	movq $0, %rbx
	syscall
