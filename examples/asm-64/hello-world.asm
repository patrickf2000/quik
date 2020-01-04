section .data
	STR_1 db "Hello, world!",0

section .bss

section .text
	extern puts
	extern printf
	extern exit
	extern fflush
	extern input_int
	extern print_int
	global main

main:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov edi, STR_1
	call puts
	
	leave
	ret
	
