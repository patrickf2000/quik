.intel_syntax noprefix
.data

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
	sub rsp, 48
	
	mov DWORD PTR [rbp-8], 10
	
	mov DWORD PTR [rbp-12], 5
	
	mov eax, [rbp-8]
	add eax, [rbp-12]
	mov DWORD PTR[rbp-16], eax
	
	mov eax, [rbp-16]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-8]
	add eax, [rbp-12]
	imul eax, 10
	mov DWORD PTR[rbp-20], eax
	
	mov eax, [rbp-20]
	mov edi, eax
	call print_int
	
	mov eax, 10
	add eax, [rbp-8]
	add eax, [rbp-12]
	sub eax, 7
	mov DWORD PTR[rbp-24], eax
	
	mov eax, [rbp-24]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-8]
	sub eax, [rbp-12]
	mov DWORD PTR[rbp-28], eax
	
	mov eax, [rbp-28]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-8]
	imul eax, [rbp-12]
	mov DWORD PTR[rbp-32], eax
	
	mov eax, [rbp-32]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-8]
	imul eax, [rbp-12]
	imul eax, 2
	mov DWORD PTR[rbp-36], eax
	
	mov eax, [rbp-36]
	mov edi, eax
	call print_int
	
	leave
	ret
	
