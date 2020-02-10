.intel_syntax noprefix
.data
	STR_1: .string "Char: %c\n"
	STR_2: .string "Your string:"
	STR_3: .string "Hello!"

.bss

.text
	.extern puts
	.extern printf
	.extern exit
	.extern fflush
	.extern input_int
	.extern print_int
	.extern strlen
	.global count_char
	.global main

count_char:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	mov eax, edi
	mov [rbp-8], eax
	
	mov rdi, QWORD PTR [rbp-8]
	call strlen
	
	mov DWORD PTR [rbp-16], eax
	
	mov DWORD PTR [rbp-20], 0
	
	mov BYTE PTR [rbp-21], 0
	
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
	mov BYTE PTR [rbp-21], al
	
	mov edi, OFFSET FLAT:STR_1
	movsx eax, BYTE PTR [rbp-21]
	mov esi, eax
	call printf
	
	mov edi, 0
	call fflush
	
	mov eax, [rbp-20]
	add eax, 1
	mov DWORD PTR[rbp-20], eax
	
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
	
	mov edi, OFFSET FLAT:STR_2
	call puts
	
	mov edi, OFFSET FLAT:STR_3
	call count_char
	
	leave
	ret
	
