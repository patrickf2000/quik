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
	push ebp
	mov ebp, esp
	sub esp, 48
	
	push dword STR_1
	call puts
	add esp, 4
	
	leave
	ret
	
