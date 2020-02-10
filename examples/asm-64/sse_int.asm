.intel_syntax noprefix
.data
	STR_1: .string "Add:"
	STR_2: .string "Mul:"

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
	sub rsp, 196
	
	mov DWORD PTR [rbp-20], 1
	mov DWORD PTR [rbp-16], 2
	mov DWORD PTR [rbp-12], 3
	mov DWORD PTR [rbp-8], 4
	
	mov DWORD PTR [rbp-36], 10
	mov DWORD PTR [rbp-32], 20
	mov DWORD PTR [rbp-28], 30
	mov DWORD PTR [rbp-24], 40
	
	movups xmm0, [rbp-20]
	movups xmm1, [rbp-36]
	addps xmm0, xmm1
	
	movups [rbp-52], xmm0
	
	movups xmm0, [rbp-20]
	movups xmm1, [rbp-36]
	pmulld xmm0, xmm1
	
	movups [rbp-68], xmm0
	
	mov DWORD PTR [rbp-72], 0
	
	mov edi, OFFSET FLAT:STR_1
	call puts
	
	jmp L2
L1:
	mov eax, [rbp-72]
	cdqe
	mov rbx, rax
	mov rax, [rbp-52]
	imul rbx, 4
	mov eax, [rbp-52+rbx]
	mov DWORD PTR [rbp-76], eax
	
	mov eax, [rbp-76]
	mov edi, eax
	call print_int
	
	add DWORD PTR [rbp-72], 1
	
L2:
	mov eax, [rbp-72]
	cmp eax, 4
	jl L1
	
	mov DWORD PTR [rbp-72], 0
	
	mov edi, OFFSET FLAT:STR_2
	call puts
	
	jmp L4
L3:
	mov eax, [rbp-72]
	cdqe
	mov rbx, rax
	mov rax, [rbp-68]
	imul rbx, 4
	mov eax, [rbp-68+rbx]
	mov DWORD PTR [rbp-80], eax
	
	mov eax, [rbp-80]
	mov edi, eax
	call print_int
	
	add DWORD PTR [rbp-72], 1
	
L4:
	mov eax, [rbp-72]
	cmp eax, 4
	jl L3
	
	leave
	ret
	
