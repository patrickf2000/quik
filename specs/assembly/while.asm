section .data
	print_int_no dd 0
	STR_1 db "%d",0
	STR_2 db "",0
	STR_3 db "Counting to 10...",0
	count_up_x dd 1
	STR_4 db "",0
	STR_5 db "Done",0
	STR_6 db "",0
	STR_7 db "Counting to 10 by 2...",0
	count_by_two_x dd 0
	STR_8 db "",0
	STR_9 db "Done",0
	STR_10 db "",0

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
	global count_up
	global count_by_two
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
	
count_up:
	push dword STR_3
	call println
	add esp, 4
	
	jmp L2
L1:
	push dword [count_up_x]
	push dword STR_4
	call print_int
	add esp, 4
	add esp, 4
	
	mov eax, [count_up_x]
	add eax, 1
	mov [count_up_x], eax
	
L2:
	mov eax, [count_up_x]
	cmp eax, 10
	jle L1
	
	push dword STR_5
	call println
	add esp, 4
	
	push dword STR_6
	call println
	add esp, 4
	
	ret
	
count_by_two:
	push dword STR_7
	call println
	add esp, 4
	
	jmp L4
L3:
	push dword [count_by_two_x]
	push dword STR_8
	call print_int
	add esp, 4
	add esp, 4
	
	mov eax, [count_by_two_x]
	add eax, 2
	mov [count_by_two_x], eax
	
L4:
	mov eax, [count_by_two_x]
	cmp eax, 10
	jle L3
	
	push dword STR_9
	call println
	add esp, 4
	
	push dword STR_10
	call println
	add esp, 4
	
	ret
	
main:
	call count_up
	
	call count_by_two
	
	mov eax, 1
	mov ebx, 0
	int 0x80
	
