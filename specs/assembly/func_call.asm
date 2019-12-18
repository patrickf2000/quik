section .data
	int_fmt db "%d",10,0
	str_fmt db "%s",10,0
	flt_fmt db "%f",10,0
	STR_1 db "Hello!",0
	add_int_n1 dd 0
	add_int_n2 dd 0
	add_int_answer dd 0
	add_float_n1 dq 0.000000
	add_float_n2 dq 0.000000
	add_float_answer dq 0
	main_answer1 dd 0
	FLT_2 dq 2.330000
	FLT_3 dq 1.400000
	main_answer2 dq 0

section .text
	global sayHello
	extern printf
	global add_int
	global add_float
	global main

sayHello:
	push eax
	push dword STR_1
	push dword str_fmt
	call printf
	add esp, 12
	
	ret
	
add_int:
	mov eax, [esp+4]
	mov [add_int_n1], eax
	
	mov eax, [esp+8]
	mov [add_int_n2], eax
	
	mov eax, [add_int_n1]
	add eax, [add_int_n2]
	mov [add_int_answer], eax
	
	mov eax, [add_int_answer]
	ret
	
add_float:
	fstp qword [add_float_n1]
	
	fstp qword [add_float_n2]
	
	fld qword [add_float_n1]
	fld qword [add_float_n2]
	fadd st0, st1
	fstp qword [add_float_answer]
	
	fld qword [add_float_answer]
	ret
	
main:
	call sayHello
	
	push 3
	push 2
	call add_int
	
	mov [main_answer1], eax
	
	push dword [main_answer1]
	push dword int_fmt
	call printf
	
	fld qword [FLT_2]
	fld qword [FLT_3]
	call add_int
	
	fstp qword [main_answer2]
	
	push dword [main_answer2+4]
	push dword [main_answer2]
	push dword flt_fmt
	call printf
	
	mov eax, 1
	mov ebx, 0
	int 0x80
	
