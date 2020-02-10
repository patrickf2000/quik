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
	push ebp
	mov ebp, esp
	sub esp, 48
	
	push OFFSET FLAT:STR_1
	call puts
	add esp, 4
	
	leave
	ret
	
