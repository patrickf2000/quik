section .data
	STR_1 db "Index 1:",0
	STR_2 db "",0
	STR_3 db "Printing numbers...",0
	STR_4 db "Done",0

section .bss

section .text
	extern puts
	extern printf
	extern exit
	extern fflush
	extern input_int
	extern print_int
	global print_numbers
	global main

print_numbers:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov rax, rdi
	mov [rbp-8], rax
	
	mov eax, 0
	mov dword [rbp-16], eax
	
	mov eax, 0
	mov dword [rbp-20], eax
	
	mov rax, [rbp-8]
	mov eax, [rax+4]
	
	mov dword [rbp-16], eax
	
	mov edi, STR_1
	call puts
	
	mov eax, [rbp-16]
	mov edi, eax
	call print_int
	
	mov edi, STR_2
	call puts
	
	jmp L2
L1:
	mov rax, [rbp-8]
	mov eax, [rbp-20]
	cdqe
	mov rbx, rax
	mov rax, [rbp-8]
	imul rbx, 4
	add rax, rbx
	mov eax, [rax]
	
	mov dword [rbp-16], eax
	
	mov eax, [rbp-16]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-20]
	add eax, 1
	mov dword [rbp-20], eax
	
L2:
	mov eax, [rbp-20]
	cmp eax, 5
	jl L1
	
	leave
	ret
	
main:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov dword [rbp-20], 9
	mov dword [rbp-16], 8
	mov dword [rbp-12], 7
	mov dword [rbp-8], 6
	mov dword [rbp-4], 5
	
	mov edi, STR_3
	call puts
	
	lea rax, [rbp-20]
	mov rdi, rax
	call print_numbers
	
	mov edi, STR_4
	call puts
	
	leave
	ret
	
