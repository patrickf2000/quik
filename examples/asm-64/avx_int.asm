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
	
	mov DWORD PTR [rbp-36], 1
	mov DWORD PTR [rbp-32], 2
	mov DWORD PTR [rbp-28], 3
	mov DWORD PTR [rbp-24], 4
	mov DWORD PTR [rbp-20], 5
	mov DWORD PTR [rbp-16], 6
	mov DWORD PTR [rbp-12], 7
	mov DWORD PTR [rbp-8], 8
	
	mov DWORD PTR [rbp-68], 10
	mov DWORD PTR [rbp-64], 20
	mov DWORD PTR [rbp-60], 30
	mov DWORD PTR [rbp-56], 40
	mov DWORD PTR [rbp-52], 50
	mov DWORD PTR [rbp-48], 60
	mov DWORD PTR [rbp-44], 70
	mov DWORD PTR [rbp-40], 80
	
	vmovupd ymm0, [rbp-36]
	vmovupd ymm1, [rbp-68]
	vaddps ymm0, ymm0, ymm1
	
	vmovups [rbp-100], ymm0
	
	vmovupd ymm0, [rbp-36]
	vmovupd ymm1, [rbp-68]
	vpmulld ymm0, ymm0, ymm1
	
	vmovups [rbp-132], ymm0
	
	mov DWORD PTR [rbp-136], 0
	
	mov edi, OFFSET FLAT:STR_1
	call puts
	
	jmp L2
L1:
	mov eax, [rbp-136]
	cdqe
	mov rbx, rax
	mov rax, [rbp-100]
	imul rbx, 4
	mov eax, [rbp-100+rbx]
	mov DWORD PTR [rbp-140], eax
	
	mov eax, [rbp-140]
	mov edi, eax
	call print_int
	
	add DWORD PTR [rbp-136], 1
	
L2:
	mov eax, [rbp-136]
	cmp eax, 8
	jl L1
	
	mov DWORD PTR [rbp-136], 0
	
	mov edi, OFFSET FLAT:STR_2
	call puts
	
	jmp L4
L3:
	mov eax, [rbp-136]
	cdqe
	mov rbx, rax
	mov rax, [rbp-132]
	imul rbx, 4
	mov eax, [rbp-132+rbx]
	mov DWORD PTR [rbp-144], eax
	
	mov eax, [rbp-144]
	mov edi, eax
	call print_int
	
	add DWORD PTR [rbp-136], 1
	
L4:
	mov eax, [rbp-136]
	cmp eax, 8
	jl L3
	
	leave
	ret
	
