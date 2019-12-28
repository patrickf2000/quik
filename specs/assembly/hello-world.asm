section .data
	print_int_no dd 0
	STR_1 db "%d",0
	STR_2 db "",0
	STR_3 db "Hello, world!",0

section .bss
	print_int_msg resb 100
	println_msg resb 100

section .text
	extern puts
	extern printf
	extern exit
	extern fflush
	extern input_int
	global print_int
	global println
	global main

print_int:
	mov eax, [esp+4]
	mov [print_int_msg], eax
	
	mov eax, [esp+8]
	mov [print_int_no], eax
	
	push dword [print_int_msg]
	call printf
	add esp, 4
	
	push dword [print_int_no]
	push dword STR_1
	call printf
	add esp, 4
	add esp, 4
	
	push dword STR_2
	call puts
	add esp, 4
	
	ret
	
println:
	mov eax, [esp+4]
	mov [println_msg], eax
	
	push dword [println_msg]
	call puts
	add esp, 4
	
	ret
	
main:
	push dword STR_3
	call println
	
	mov eax, 1
	mov ebx, 0
	int 0x80
	
