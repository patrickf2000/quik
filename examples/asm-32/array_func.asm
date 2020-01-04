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
	push ebp
	mov ebp, esp
	sub esp, 48
	
	mov eax, [ebp+8]
	mov [ebp-4], eax
	
	mov eax, 0
	mov dword [ebp-12], eax
	
	mov eax, 0
	mov dword [ebp-16], eax
	
	mov eax, [ebp-4]
	mov eax, [eax+4]
	
	mov dword [ebp-12], eax
	
	push dword STR_1
	call puts
	add esp, 4
	
	push dword [ebp-12]
	call print_int
	add esp, 4
	
	push dword STR_2
	call puts
	add esp, 4
	
	jmp L2
L1:
	mov eax, [ebp-4]
	mov ebx, [ebp-16]
	imul ebx, 4
	mov eax, [eax+ebx]
	
	mov dword [ebp-12], eax
	
	push dword [ebp-12]
	call print_int
	add esp, 4
	
	mov eax, [ebp-16]
	add eax, 1
	mov dword [ebp-16], eax
	
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
	
	mov dword [ebp-20], 9
	mov dword [ebp-16], 8
	mov dword [ebp-12], 7
	mov dword [ebp-8], 6
	mov dword [ebp-4], 5
	
	push dword STR_3
	call puts
	add esp, 4
	
	lea eax, [ebp-20]
	push eax
	call print_numbers
	add esp, 4
	
	push dword STR_4
	call puts
	add esp, 4
	
	leave
	ret
	
