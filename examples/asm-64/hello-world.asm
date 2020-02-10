.intel_syntax noprefix
.data
	STR_1: .string "Hello, world!"

.bss

.text
	.extern puts
	.extern printf
	.extern exit
	.extern fflush
	.extern input_int
	.extern print_int
	.global main

main:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov edi, OFFSET FLAT:STR_1
	call puts
	
	leave
	ret
	
