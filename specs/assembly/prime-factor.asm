section .data
	is_prime_no dd 0
	is_prime_prime dd 1
	is_prime_index dd 2
	is_prime_mod dd 0
	main_x dd 11
	main_prime dd 0
	int_fmt db "%d",10,0
	str_fmt db "%s",10,0
	flt_fmt db "%f",10,0
	STR_1 db "Your number:",0
	STR_2 db "It is prime!!",0
	STR_3 db "Not prime!",0

section .text
	global is_prime
	global main
	extern printf

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
	push dword [main_x]
	call is_prime
	
	mov [main_prime], eax
	
	push dword STR_1
	push dword str_fmt
	call printf
	
	push dword [main_x]
	push dword int_fmt
	call printf
	
	mov eax, [main_prime]
	cmp eax, 1
	jne L6
	
	push dword STR_2
	push dword str_fmt
	call printf
	
	jmp L5
L6:
	push dword STR_3
	push dword str_fmt
	call printf
	
	jmp L5
L7:
L5:
	mov eax, 1
	mov ebx, 0
	int 0x80
	
