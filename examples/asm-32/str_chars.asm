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
	push ebp
	mov ebp, esp
	sub esp, 48
	
	mov eax, [ebp+8]
	mov [ebp-4], eax
	
	push DWORD PTR [ebp-4]
	call strlen
	add esp, 4
	
	mov DWORD PTR [ebp-12], eax
	
	mov DWORD PTR [ebp-16], 0
	
	mov BYTE PTR [ebp-17], 0
	
	jmp L2
L1:
	mov eax, [ebp-4]
	mov ebx, [ebp-16]
	imul ebx, 1
	mov eax, [eax+ebx]
	mov BYTE PTR [ebp-17], al
	
	push DWORD PTR [ebp-17]
	push OFFSET FLAT:STR_1
	call printf
	add esp, 4
	add esp, 4
	
	push 0
	call fflush
	add esp, 4
	
	mov eax, [ebp-16]
	add eax, 1
	mov DWORD PTR[ebp-16], eax
	
L2:
	mov eax, [ebp-16]
	cmp eax, [ebp-12]
	jl L1
	
	mov esp, ebp
	pop ebp
	ret
	
main:
	push ebp
	mov ebp, esp
	sub esp, 48
	
	push OFFSET FLAT:STR_2
	call puts
	add esp, 4
	
	push OFFSET FLAT:STR_3
	call count_char
	add esp, 4
	
	leave
	ret
	
