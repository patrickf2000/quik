.intel_syntax noprefix
.data
	STR_1: .string "Enter 5 numbers:"
	STR_2: .string ""
	STR_3: .string "Your numbers:"
	STR_4: .string ""
	STR_5: .string "Your numbers doubled are:"

.bss

.text
	.extern puts
	.extern printf
	.extern exit
	.extern fflush
	.extern input_int
	.extern print_int
	.global main

main:
	push rbp
	mov rbp, rsp
	sub rsp, 128
	
	mov DWORD PTR [rbp-24], 0
	mov DWORD PTR [rbp-20], 0
	mov DWORD PTR [rbp-16], 0
	mov DWORD PTR [rbp-12], 0
	mov DWORD PTR [rbp-8], 0
	
	mov DWORD PTR [rbp-36], 0
	
	mov DWORD PTR [rbp-40], 0
	
	mov edi, OFFSET FLAT:STR_1
	call puts
	
	jmp L2
L1:
	call input_int
	
	mov DWORD PTR [rbp-36], eax
	
	mov ebx, [rbp-40]
	imul ebx, 4
	mov eax, [rbp-36]
	mov DWORD PTR [rbp-24+rbx], eax
	
	mov eax, [rbp-40]
	add eax, 1
	mov DWORD PTR[rbp-40], eax
	
L2:
	mov eax, [rbp-40]
	cmp eax, 5
	jl L1
	
	mov edi, OFFSET FLAT:STR_2
	call puts
	
	mov edi, OFFSET FLAT:STR_3
	call puts
	
	mov DWORD PTR [rbp-40], 0
	
	jmp L4
L3:
	mov eax, [rbp-40]
	cdqe
	mov rbx, rax
	mov rax, [rbp-24]
	imul rbx, 4
	mov eax, [rbp-24+rbx]
	mov DWORD PTR [rbp-36], eax
	
	mov eax, [rbp-36]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-36]
	imul eax, 2
	mov DWORD PTR[rbp-36], eax
	
	mov ebx, [rbp-40]
	imul ebx, 4
	mov eax, [rbp-36]
	mov DWORD PTR [rbp-24+rbx], eax
	
	mov eax, [rbp-40]
	add eax, 1
	mov DWORD PTR[rbp-40], eax
	
L4:
	mov eax, [rbp-40]
	cmp eax, 5
	jl L3
	
	mov edi, OFFSET FLAT:STR_4
	call puts
	
	mov edi, OFFSET FLAT:STR_5
	call puts
	
	mov DWORD PTR [rbp-40], 0
	
	jmp L6
L5:
	mov eax, [rbp-40]
	cdqe
	mov rbx, rax
	mov rax, [rbp-24]
	imul rbx, 4
	mov eax, [rbp-24+rbx]
	mov DWORD PTR [rbp-36], eax
	
	mov eax, [rbp-36]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-40]
	add eax, 1
	mov DWORD PTR[rbp-40], eax
	
L6:
	mov eax, [rbp-40]
	cmp eax, 5
	jl L5
	
	leave
	ret
	
