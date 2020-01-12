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
	
	mov dword [rbp-20], 1
	mov dword [rbp-16], 2
	mov dword [rbp-12], 3
	mov dword [rbp-8], 4
	
	mov dword [rbp-36], 10
	mov dword [rbp-32], 20
	mov dword [rbp-28], 30
	mov dword [rbp-24], 40
	
	vzeroall
	
	movups xmm0, [rbp-20]
	movups xmm1, [rbp-36]
	addps xmm0, xmm1
	
	movups [rbp-52], xmm0
	
	vzeroall
	
	movups xmm0, [rbp-20]
	movups xmm1, [rbp-36]
	pmulld xmm0, xmm1
	
	movups [rbp-68], xmm0
	
	mov dword [rbp-72], 0
	
	mov edi, STR_1
	call puts
	
	jmp L2
L1:
	mov eax, [rbp-72]
	cdqe
	mov rbx, rax
	mov rax, [rbp-52]
	imul rbx, 4
	mov eax, [rbp-52+rbx]
	mov dword [rbp-76], eax
	
	mov eax, [rbp-76]
	mov edi, eax
	call print_int
	
	add dword [rbp-72], 1
	
L2:
	mov eax, [rbp-72]
	cmp eax, 4
	jl L1
	
	mov dword [rbp-72], 0
	
	mov edi, STR_2
	call puts
	
	jmp L4
L3:
	mov eax, [rbp-72]
	cdqe
	mov rbx, rax
	mov rax, [rbp-68]
	imul rbx, 4
	mov eax, [rbp-68+rbx]
	mov dword [rbp-80], eax
	
	mov eax, [rbp-80]
	mov edi, eax
	call print_int
	
	add dword [rbp-72], 1
	
L4:
	mov eax, [rbp-72]
	cmp eax, 4
	jl L3
	
	leave
	ret
	
