.intel_syntax noprefix
.data
	STR_1: .string "Enter a number:"
	STR_2: .string ""
	STR_3: .string "The factorial is:"

.bss

.text
	.extern puts
	.extern printf
	.extern exit
	.extern fflush
	.extern input_int
	.extern print_int
	.global factorial
	.global main

factorial:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov eax, edi
	mov [rbp-8], eax
	
	mov DWORD PTR [rbp-16], 1
	
	jmp L2
L1:
	mov eax, [rbp-16]
	imul eax, [rbp-8]
	mov DWORD PTR[rbp-16], eax
	
	mov eax, [rbp-8]
	sub eax, 1
	mov DWORD PTR[rbp-8], eax
	
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
	
	mov edi, OFFSET FLAT:STR_1
	call puts
	
	call input_int
	
	mov DWORD PTR [rbp-8], eax
	
	mov eax, [rbp-8]
	mov edi, eax
	call factorial
	
	mov DWORD PTR [rbp-12], eax
	
	mov edi, OFFSET FLAT:STR_2
	call puts
	
	mov edi, OFFSET FLAT:STR_3
	call puts
	
	mov eax, [rbp-12]
	mov edi, eax
	call print_int
	
	leave
	ret
	
