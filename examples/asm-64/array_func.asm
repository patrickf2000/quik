.intel_syntax noprefix
.data
	STR_1: .string "Index 1:"
	STR_2: .string ""
	STR_3: .string "Printing numbers..."
	STR_4: .string "Done"

.bss

.text
	.extern puts
	.extern printf
	.extern exit
	.extern fflush
	.extern input_int
	.extern print_int
	.global print_numbers
	.global main

print_numbers:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov rax, rdi
	mov [rbp-8], rax
	
	mov DWORD PTR [rbp-20], 0
	
	mov DWORD PTR [rbp-24], 0
	
	mov rax, [rbp-8]
	mov eax, [rax+4]
	mov DWORD PTR [rbp-20], eax
	
	mov edi, OFFSET FLAT:STR_1
	call puts
	
	mov eax, [rbp-20]
	mov edi, eax
	call print_int
	
	mov edi, OFFSET FLAT:STR_2
	call puts
	
	jmp L2
L1:
	mov rax, [rbp-8]
	mov eax, [rbp-24]
	cdqe
	mov rbx, rax
	mov rax, [rbp-8]
	imul rbx, 4
	add rax, rbx
	mov eax, [rax]
	mov DWORD PTR [rbp-20], eax
	
	mov eax, [rbp-20]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-24]
	add eax, 1
	mov DWORD PTR[rbp-24], eax
	
L2:
	mov eax, [rbp-24]
	cmp eax, 5
	jl L1
	
	leave
	ret
	
main:
	push rbp
	mov rbp, rsp
	sub rsp, 128
	
	mov DWORD PTR [rbp-24], 9
	mov DWORD PTR [rbp-20], 8
	mov DWORD PTR [rbp-16], 7
	mov DWORD PTR [rbp-12], 6
	mov DWORD PTR [rbp-8], 5
	
	mov edi, OFFSET FLAT:STR_3
	call puts
	
	lea rax, [rbp-24]
	mov rdi, rax
	call print_numbers
	
	mov edi, OFFSET FLAT:STR_4
	call puts
	
	leave
	ret
	
