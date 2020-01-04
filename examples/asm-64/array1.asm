section .data
	STR_1 db "Enter 5 numbers:",0
	STR_2 db "",0
	STR_3 db "Your numbers:",0
	STR_4 db "",0
	STR_5 db "Your numbers doubled are:",0

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
	sub rsp, 48
	
	mov dword [rbp-20], 0
	mov dword [rbp-16], 0
	mov dword [rbp-12], 0
	mov dword [rbp-8], 0
	mov dword [rbp-4], 0
	
	mov eax, 0
	mov dword [rbp-24], eax
	
	mov eax, 0
	mov dword [rbp-28], eax
	
	mov edi, STR_1
	call puts
	
	jmp L2
L1:
	call input_int
	
	
	mov dword [rbp-24], eax
	
	mov ebx, [rbp-28]
	imul ebx, 4
	mov eax, [rbp-24]
	mov dword [rbp-20+rbx], eax
	
	mov eax, [rbp-28]
	add eax, 1
	mov dword [rbp-28], eax
	
L2:
	mov eax, [rbp-28]
	cmp eax, 5
	jl L1
	
	mov edi, STR_2
	call puts
	
	mov edi, STR_3
	call puts
	
	mov eax, 0
	mov dword [rbp-28], eax
	
	jmp L4
L3:
	mov eax, [rbp-28]
	cdqe
	mov rbx, rax
	mov rax, [rbp-4]
	imul rbx, 4
	mov eax, [rbp-20+rbx]
	
	mov dword [rbp-24], eax
	
	mov eax, [rbp-24]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-24]
	imul eax, 2
	mov dword [rbp-24], eax
	
	mov ebx, [rbp-28]
	imul ebx, 4
	mov eax, [rbp-24]
	mov dword [rbp-20+rbx], eax
	
	mov eax, [rbp-28]
	add eax, 1
	mov dword [rbp-28], eax
	
L4:
	mov eax, [rbp-28]
	cmp eax, 5
	jl L3
	
	mov edi, STR_4
	call puts
	
	mov edi, STR_5
	call puts
	
	mov eax, 0
	mov dword [rbp-28], eax
	
	jmp L6
L5:
	mov eax, [rbp-28]
	cdqe
	mov rbx, rax
	mov rax, [rbp-4]
	imul rbx, 4
	mov eax, [rbp-20+rbx]
	
	mov dword [rbp-24], eax
	
	mov eax, [rbp-24]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-28]
	add eax, 1
	mov dword [rbp-28], eax
	
L6:
	mov eax, [rbp-28]
	cmp eax, 5
	jl L5
	
	leave
	ret
	
