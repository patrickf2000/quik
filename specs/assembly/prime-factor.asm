section .data
	print_int_no dd 0
	STR_1 db "%d",0
	STR_2 db "",0
	is_prime_no dd 0
	is_prime_prime dd 1
	is_prime_index dd 2
	is_prime_mod dd 0
	STR_3 db "Enter a number:",0
	main_x dd 0
	main_prime dd 0
	STR_4 db "It is prime!!",0
	STR_5 db "Not prime!",0

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
	global is_prime
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
	
is_prime:
	mov eax, [esp+4]
	mov [is_prime_no], eax
	
	jmp L2
L1:
	mov eax, [is_prime_no]
	mov ebx, [is_prime_index]
	cdq
	div ebx
	mov eax, edx
	mov [is_prime_mod], eax
	
	mov eax, [is_prime_mod]
	cmp eax, 0
	jne L4
	
	mov [is_prime_prime], eax
	
	mov eax, [is_prime_no]
	mov [is_prime_index], eax
	
	jmp L3
L4:
L3:
	mov eax, [is_prime_index]
	add eax, 1
	mov [is_prime_index], eax
	
L2:
	mov eax, [is_prime_index]
	cmp eax, [is_prime_no]
	jl L1
	
	mov eax, [is_prime_prime]
	ret
	
main:
	push dword STR_3
	call println
	
	call input_int
	
	mov [main_x], eax
	
	push dword [main_x]
	call is_prime
	
	mov [main_prime], eax
	
	mov eax, [main_prime]
	cmp eax, 1
	jne L6
	
	push dword STR_4
	call println
	
	jmp L5
L6:
	push dword STR_5
	call println
	
	jmp L5
L7:
L5:
	mov eax, 1
	mov ebx, 0
	int 0x80
	
