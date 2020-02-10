.intel_syntax noprefix
.data
	STR_1: .string "Index 1:"
	STR_2: .string ""
	STR_3: .string "Printing numbers..."
	STR_4: .string "Done"

.bss

.text
	.extern puts
	.extern printf
	.extern exit
	.extern fflush
	.extern input_int
	.extern print_int
	.global print_numbers
	.global main

print_numbers:
	push ebp
	mov ebp, esp
	sub esp, 48
	
	mov eax, [ebp+8]
	mov [ebp-4], eax
	
	mov DWORD PTR [ebp-12], 0
	
	mov DWORD PTR [ebp-16], 0
	
	mov eax, [ebp-4]
	mov eax, [eax+4]
	mov DWORD PTR [ebp-12], eax
	
	push OFFSET FLAT:STR_1
	call puts
	add esp, 4
	
	push DWORD PTR [ebp-12]
	call print_int
	add esp, 4
	
	push OFFSET FLAT:STR_2
	call puts
	add esp, 4
	
	jmp L2
L1:
	mov eax, [ebp-4]
	mov ebx, [ebp-16]
	imul ebx, 4
	mov eax, [eax+ebx]
	mov DWORD PTR [ebp-12], eax
	
	push DWORD PTR [ebp-12]
	call print_int
	add esp, 4
	
	mov eax, [ebp-16]
	add eax, 1
	mov DWORD PTR[ebp-16], eax
	
L2:
	mov eax, [ebp-16]
	cmp eax, 5
	jl L1
	
	mov esp, ebp
	pop ebp
	ret
	
main:
	push ebp
	mov ebp, esp
	sub esp, 48
	
	mov DWORD PTR [ebp-24], 9
	mov DWORD PTR [ebp-20], 8
	mov DWORD PTR [ebp-16], 7
	mov DWORD PTR [ebp-12], 6
	mov DWORD PTR [ebp-8], 5
	
	push OFFSET FLAT:STR_3
	call puts
	add esp, 4
	
	lea eax, [ebp-24]
	push eax
	call print_numbers
	add esp, 4
	
	push OFFSET FLAT:STR_4
	call puts
	add esp, 4
	
	leave
	ret
	
