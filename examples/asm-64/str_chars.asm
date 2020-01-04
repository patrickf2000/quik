section .data
	STR_1 db "Char: %c",0xA,0
	STR_2 db "Your string:",0
	STR_3 db "Hello!",0

section .bss

section .text
	extern puts
	extern printf
	extern exit
	extern fflush
	extern input_int
	extern print_int
	extern strlen
	global count_char
	global main

count_char:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov eax, edi
	mov [rbp-8], eax
	
	mov edi, [rbp-8]
	call strlen
	
	
	mov dword [rbp-16], eax
	
	mov eax, 0
	mov dword [rbp-20], eax
	
	mov eax, 0
	mov byte [rbp-21], al
	
	jmp L2
L1:
	mov rax, [rbp-8]
	mov eax, [rbp-20]
	cdqe
	mov rbx, rax
	mov rax, [rbp-8]
	imul rbx, 1
	add rax, rbx
	mov eax, [rax]
	
	mov byte [rbp-21], al
	
	mov edi, STR_1
	movsx eax, byte [rbp-21]
	mov esi, eax
	call printf
	
	mov edi, 0
	call fflush
	
	mov eax, [rbp-20]
	add eax, 1
	mov dword [rbp-20], eax
	
L2:
	mov eax, [rbp-20]
	cmp eax, [rbp-16]
	jl L1
	
	leave
	ret
	
main:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov edi, STR_2
	call puts
	
	mov edi, STR_3
	call count_char
	
	leave
	ret
	
