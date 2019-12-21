section .data
	numbers dd 0,0,0,0,0
	main_x dd 0
	main_index dd 0
	int_fmt db "%d",10,0
	str_fmt db "%s",10,0
	flt_fmt db "%f",10,0
	STR_1 db "Enter five numbers:",0
	STR_2 db "Doubling all your numbers...",0
	STR_3 db "Your new array is:",0
	STR_4 db "Done",0

section .text
	extern input_int
	global main
	extern printf

main:
	push dword STR_1
	push dword str_fmt
	call printf
	
	jmp L2
L1:
	call input_int
	
	mov [main_x], eax
	
	mov ebx, [main_index]
	imul ebx, 4
	mov eax, [main_x]
	mov [numbers+ebx], eax
	
	mov eax, [main_index]
	add eax, 1
	mov [main_index], eax
	
L2:
	mov eax, [main_index]
	cmp eax, 5
	jl L1
	
	push dword STR_2
	push dword str_fmt
	call printf
	
	mov eax, 0
	mov [main_index], eax
	
	jmp L4
L3:
	mov ebx, [main_index]
	imul ebx, 4
	mov ebx, [main_index]
	imul ebx, 4
	mov eax, [numbers+ebx]
	
	imul eax, 2
	mov [numbers+ebx], eax
	
	mov eax, [main_index]
	add eax, 1
	mov [main_index], eax
	
L4:
	mov eax, [main_index]
	cmp eax, 5
	jl L3
	
	push dword STR_3
	push dword str_fmt
	call printf
	
	mov eax, 0
	mov [main_index], eax
	
	jmp L6
L5:
	mov ebx, [main_index]
	imul ebx, 4
	mov eax, [numbers+ebx]
	
	mov [main_x], eax
	
	push dword [main_x]
	push dword int_fmt
	call printf
	
	mov eax, [main_index]
	add eax, 1
	mov [main_index], eax
	
L6:
	mov eax, [main_index]
	cmp eax, 5
	jl L5
	
	push dword STR_4
	push dword str_fmt
	call printf
	
	mov eax, 1
	mov ebx, 0
	int 0x80
	
