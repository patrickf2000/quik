section .data
	print_int_no dd 0
	STR_1 db "%d",0
	STR_2 db "",0
	main_numbers dd 0,0,0,0,0
	main_x dd 0
	main_index dd 0
	STR_3 db "Enter five numbers:",0
	STR_4 db "Doubling all your numbers...",0
	STR_5 db "Your new array is:",0
	STR_6 db "",0
	STR_7 db "Done",0

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
	
	jmp L2
L1:
	call input_int
	
	mov [main_x], eax
	
	mov ebx, [main_index]
	imul ebx, 4
	mov eax, [main_x]
	mov [main_numbers+ebx], eax
	
	mov eax, [main_index]
	add eax, 1
	mov [main_index], eax
	
L2:
	mov eax, [main_index]
	cmp eax, 5
	jl L1
	
	push dword STR_4
	call println
	
	mov eax, 0
	mov [main_index], eax
	
	jmp L4
L3:
	mov ebx, [main_index]
	imul ebx, 4
	mov ebx, [main_index]
	imul ebx, 4
	mov eax, [main_numbers+ebx]
	
	imul eax, 2
	mov [main_numbers+ebx], eax
	
	mov eax, [main_index]
	add eax, 1
	mov [main_index], eax
	
L4:
	mov eax, [main_index]
	cmp eax, 5
	jl L3
	
	push dword STR_5
	call println
	
	mov eax, 0
	mov [main_index], eax
	
	jmp L6
L5:
	mov ebx, [main_index]
	imul ebx, 4
	mov eax, [main_numbers+ebx]
	
	mov [main_x], eax
	
	push dword [main_x]
	push dword STR_6
	call print_int
	
	mov eax, [main_index]
	add eax, 1
	mov [main_index], eax
	
L6:
	mov eax, [main_index]
	cmp eax, 5
	jl L5
	
	call println
	
	push dword STR_7
	call println
	
	mov eax, 1
	mov ebx, 0
	int 0x80
	
