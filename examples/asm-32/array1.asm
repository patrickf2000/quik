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
	push ebp
	mov ebp, esp
	sub esp, 48
	
	mov dword [ebp-20], 0
	mov dword [ebp-16], 0
	mov dword [ebp-12], 0
	mov dword [ebp-8], 0
	mov dword [ebp-4], 0
	
	mov eax, 0
	mov dword [ebp-24], eax
	
	mov eax, 0
	mov dword [ebp-28], eax
	
	push dword STR_1
	call puts
	add esp, 4
	
	jmp L2
L1:
	call input_int
	
	
	mov dword [ebp-24], eax
	
	mov ebx, [ebp-28]
	imul ebx, 4
	mov eax, [ebp-24]
	mov dword [ebp-20+ebx], eax
	
	mov eax, [ebp-28]
	add eax, 1
	mov dword [ebp-28], eax
	
L2:
	mov eax, [ebp-28]
	cmp eax, 5
	jl L1
	
	push dword STR_2
	call puts
	add esp, 4
	
	push dword STR_3
	call puts
	add esp, 4
	
	mov eax, 0
	mov dword [ebp-28], eax
	
	jmp L4
L3:
	mov ebx, [ebp-28]
	imul ebx, 4
	mov eax, [ebp-20+ebx]
	
	mov dword [ebp-24], eax
	
	push dword [ebp-24]
	call print_int
	add esp, 4
	
	mov eax, [ebp-24]
	imul eax, 2
	mov dword [ebp-24], eax
	
	mov ebx, [ebp-28]
	imul ebx, 4
	mov eax, [ebp-24]
	mov dword [ebp-20+ebx], eax
	
	mov eax, [ebp-28]
	add eax, 1
	mov dword [ebp-28], eax
	
L4:
	mov eax, [ebp-28]
	cmp eax, 5
	jl L3
	
	push dword STR_4
	call puts
	add esp, 4
	
	push dword STR_5
	call puts
	add esp, 4
	
	mov eax, 0
	mov dword [ebp-28], eax
	
	jmp L6
L5:
	mov ebx, [ebp-28]
	imul ebx, 4
	mov eax, [ebp-20+ebx]
	
	mov dword [ebp-24], eax
	
	push dword [ebp-24]
	call print_int
	add esp, 4
	
	mov eax, [ebp-28]
	add eax, 1
	mov dword [ebp-28], eax
	
L6:
	mov eax, [ebp-28]
	cmp eax, 5
	jl L5
	
	leave
	ret
	
