section .data
	STR_1 db "Enter a number:",0
	STR_2 db "",0
	STR_3 db "The factorial is:",0

section .bss

section .text
	extern puts
	extern printf
	extern exit
	extern fflush
	extern input_int
	extern print_int
	global factorial
	global main

factorial:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov eax, edi
	mov [rbp-8], eax
	
	mov eax, 1
	mov dword [rbp-16], eax
	
	jmp L2
L1:
	mov eax, [rbp-16]
	imul eax, [rbp-8]
	mov dword [rbp-16], eax
	
	mov eax, [rbp-8]
	sub eax, 1
	mov dword [rbp-8], eax
	
L2:
	mov eax, [rbp-8]
	cmp eax, 1
	jne L1
	
	mov eax, [rbp-16]
	leave
	ret
	
main:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov edi, STR_1
	call puts
	
	call input_int
	
	
	mov dword [rbp-8], eax
	
	mov eax, [rbp-8]
	mov edi, eax
	call factorial
	
	
	mov dword [rbp-12], eax
	
	mov edi, STR_2
	call puts
	
	mov edi, STR_3
	call puts
	
	mov eax, [rbp-12]
	mov edi, eax
	call print_int
	
	leave
	ret
	
