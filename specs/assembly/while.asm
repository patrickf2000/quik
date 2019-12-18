section .data
	int_fmt db "%d",10,0
	str_fmt db "%s",10,0
	flt_fmt db "%f",10,0
	STR_1 db "Counting to 10...",0
	count_up_x dd 1
	STR_2 db "Done",0
	STR_3 db "",0
	STR_4 db "Counting to 10 by 2...",0
	count_by_two_x dd 0
	STR_5 db "Done",0
	STR_6 db "",0

section .text
	global count_up
	extern printf
	global count_by_two
	global main

count_up:
	push eax
	push dword STR_1
	push dword str_fmt
	call printf
	add esp, 12
	
	jmp L2
L1:
	push eax
	push dword [count_up_x]
	push dword int_fmt
	call printf
	add esp, 12
	
	mov eax, [count_up_x]
	add eax, 1
	mov [count_up_x], eax
	
L2:
	mov eax, [count_up_x]
	cmp eax, 10
	jle L1
	
	push eax
	push dword STR_2
	push dword str_fmt
	call printf
	add esp, 12
	
	push eax
	push dword STR_3
	push dword str_fmt
	call printf
	add esp, 12
	
	ret
	
count_by_two:
	push eax
	push dword STR_4
	push dword str_fmt
	call printf
	add esp, 12
	
	jmp L4
L3:
	push eax
	push dword [count_by_two_x]
	push dword int_fmt
	call printf
	add esp, 12
	
	mov eax, [count_by_two_x]
	add eax, 2
	mov [count_by_two_x], eax
	
L4:
	mov eax, [count_by_two_x]
	cmp eax, 10
	jle L3
	
	push eax
	push dword STR_5
	push dword str_fmt
	call printf
	add esp, 12
	
	push eax
	push dword STR_6
	push dword str_fmt
	call printf
	add esp, 12
	
	ret
	
main:
	call count_up
	
	call count_by_two
	
	mov eax, 1
	mov ebx, 0
	int 0x80
	
