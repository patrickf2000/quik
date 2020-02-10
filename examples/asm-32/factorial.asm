.intel_syntax noprefix
.data
	STR_1: .string "Enter a number:"
	STR_2: .string ""
	STR_3: .string "The factorial is:"

.bss

.text
	.extern puts
	.extern printf
	.extern exit
	.extern fflush
	.extern input_int
	.extern print_int
	.global factorial
	.global main

factorial:
	push ebp
	mov ebp, esp
	sub esp, 48
	
	mov eax, [ebp+8]
	mov [ebp-4], eax
	
	mov DWORD PTR [ebp-12], 1
	
	jmp L2
L1:
	mov eax, [ebp-12]
	imul eax, [ebp-4]
	mov DWORD PTR[ebp-12], eax
	
	mov eax, [ebp-4]
	sub eax, 1
	mov DWORD PTR[ebp-4], eax
	
L2:
	mov eax, [ebp-4]
	cmp eax, 1
	jne L1
	
	mov eax, [ebp-12]
	mov esp, ebp
	pop ebp
	ret
	
main:
	push ebp
	mov ebp, esp
	sub esp, 48
	
	push OFFSET FLAT:STR_1
	call puts
	add esp, 4
	
	call input_int
	
	mov DWORD PTR [ebp-8], eax
	
	push DWORD PTR [ebp-8]
	call factorial
	add esp, 4
	
	mov DWORD PTR [ebp-12], eax
	
	push OFFSET FLAT:STR_2
	call puts
	add esp, 4
	
	push OFFSET FLAT:STR_3
	call puts
	add esp, 4
	
	push DWORD PTR [ebp-12]
	call print_int
	add esp, 4
	
	leave
	ret
	
