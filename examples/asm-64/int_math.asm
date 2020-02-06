section .data

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
	
	mov dword [rbp-8], 10
	
	mov dword [rbp-12], 5
	
	mov eax, [rbp-8]
	add eax, [rbp-12]
	mov dword[rbp-16], eax
	
	mov eax, [rbp-16]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-8]
	add eax, [rbp-12]
	imul eax, 10
	mov dword[rbp-20], eax
	
	mov eax, [rbp-20]
	mov edi, eax
	call print_int
	
	mov eax, 10
	add eax, [rbp-8]
	add eax, [rbp-12]
	sub eax, 7
	mov dword[rbp-24], eax
	
	mov eax, [rbp-24]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-8]
	sub eax, [rbp-12]
	mov dword[rbp-28], eax
	
	mov eax, [rbp-28]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-8]
	imul eax, [rbp-12]
	mov dword[rbp-32], eax
	
	mov eax, [rbp-32]
	mov edi, eax
	call print_int
	
	mov eax, [rbp-8]
	imul eax, [rbp-12]
	imul eax, 2
	mov dword[rbp-36], eax
	
	mov eax, [rbp-36]
	mov edi, eax
	call print_int
	
	leave
	ret
	
