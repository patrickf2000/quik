section .data
	STR_1 db "Add:",0
	STR_2 db "Mul:",0

section .bss

section .text
	extern puts
	extern printf
	extern exit
	extern fflush
	extern input_int
	extern print_int
	global main

main:
	push rbp
	mov rbp, rsp
	sub rsp, 196
	
	mov dword [rbp-36], 1
	mov dword [rbp-32], 2
	mov dword [rbp-28], 3
	mov dword [rbp-24], 4
	mov dword [rbp-20], 5
	mov dword [rbp-16], 6
	mov dword [rbp-12], 7
	mov dword [rbp-8], 8
	
	mov dword [rbp-68], 10
	mov dword [rbp-64], 20
	mov dword [rbp-60], 30
	mov dword [rbp-56], 40
	mov dword [rbp-52], 50
	mov dword [rbp-48], 60
	mov dword [rbp-44], 70
	mov dword [rbp-40], 80
	
	vzeroall
	
	vmovupd ymm0, [rbp-36]
	vmovupd ymm1, [rbp-68]
	vaddps ymm0, ymm0, ymm1
	
	vmovups [rbp-100], ymm0
	
	vzeroall
	
	vmovupd ymm0, [rbp-36]
	vmovupd ymm1, [rbp-68]
	vpmulld ymm0, ymm0, ymm1
	
	vmovups [rbp-132], ymm0
	
	mov dword [rbp-136], 0
	
	mov edi, STR_1
	call puts
	
	jmp L2
L1:
	mov eax, [rbp-136]
	cdqe
	mov rbx, rax
	mov rax, [rbp-100]
	imul rbx, 4
	mov eax, [rbp-100+rbx]
	mov dword [rbp-140], eax
	
	mov eax, [rbp-140]
	mov edi, eax
	call print_int
	
	add dword [rbp-136], 1
	
L2:
	mov eax, [rbp-136]
	cmp eax, 8
	jl L1
	
	mov dword [rbp-136], 0
	
	mov edi, STR_2
	call puts
	
	jmp L4
L3:
	mov eax, [rbp-136]
	cdqe
	mov rbx, rax
	mov rax, [rbp-132]
	imul rbx, 4
	mov eax, [rbp-132+rbx]
	mov dword [rbp-144], eax
	
	mov eax, [rbp-144]
	mov edi, eax
	call print_int
	
	add dword [rbp-136], 1
	
L4:
	mov eax, [rbp-136]
	cmp eax, 8
	jl L3
	
	leave
	ret
	
