.intel_syntax noprefix
.data
	STR_1: .string "Enter 5 numbers:"
	STR_2: .string ""
	STR_3: .string "Your numbers:"
	STR_4: .string ""
	STR_5: .string "Your numbers doubled are:"

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
	push ebp
	mov ebp, esp
	sub esp, 48
	
	mov DWORD PTR [ebp-24], 0
	mov DWORD PTR [ebp-20], 0
	mov DWORD PTR [ebp-16], 0
	mov DWORD PTR [ebp-12], 0
	mov DWORD PTR [ebp-8], 0
	
	mov DWORD PTR [ebp-36], 0
	
	mov DWORD PTR [ebp-40], 0
	
	push OFFSET FLAT:STR_1
	call puts
	add esp, 4
	
	jmp L2
L1:
	call input_int
	
	mov DWORD PTR [ebp-36], eax
	
	mov ebx, [ebp-40]
	imul ebx, 4
	mov eax, [ebp-36]
	mov DWORD PTR [ebp-24+ebx], eax
	
	mov eax, [ebp-40]
	add eax, 1
	mov DWORD PTR[ebp-40], eax
	
L2:
	mov eax, [ebp-40]
	cmp eax, 5
	jl L1
	
	push OFFSET FLAT:STR_2
	call puts
	add esp, 4
	
	push OFFSET FLAT:STR_3
	call puts
	add esp, 4
	
	mov DWORD PTR [ebp-40], 0
	
	jmp L4
L3:
	mov ebx, [ebp-40]
	imul ebx, 4
	mov eax, [ebp-24+ebx]
	mov DWORD PTR [ebp-36], eax
	
	push DWORD PTR [ebp-36]
	call print_int
	add esp, 4
	
	mov eax, [ebp-36]
	imul eax, 2
	mov DWORD PTR[ebp-36], eax
	
	mov ebx, [ebp-40]
	imul ebx, 4
	mov eax, [ebp-36]
	mov DWORD PTR [ebp-24+ebx], eax
	
	mov eax, [ebp-40]
	add eax, 1
	mov DWORD PTR[ebp-40], eax
	
L4:
	mov eax, [ebp-40]
	cmp eax, 5
	jl L3
	
	push OFFSET FLAT:STR_4
	call puts
	add esp, 4
	
	push OFFSET FLAT:STR_5
	call puts
	add esp, 4
	
	mov DWORD PTR [ebp-40], 0
	
	jmp L6
L5:
	mov ebx, [ebp-40]
	imul ebx, 4
	mov eax, [ebp-24+ebx]
	mov DWORD PTR [ebp-36], eax
	
	push DWORD PTR [ebp-36]
	call print_int
	add esp, 4
	
	mov eax, [ebp-40]
	add eax, 1
	mov DWORD PTR[ebp-40], eax
	
L6:
	mov eax, [ebp-40]
	cmp eax, 5
	jl L5
	
	leave
	ret
	
