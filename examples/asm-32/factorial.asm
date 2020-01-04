section .data
	STR_1 db "Enter a number:",0
	STR_2 db "",0
	STR_3 db "The factorial is:",0

section .bss

section .text
	extern puts
	extern printf
	extern exit
	extern fflush
	extern input_int
	extern print_int
	global factorial
	global main

factorial:
	push ebp
	mov ebp, esp
	sub esp, 48
	
	mov eax, [ebp+8]
	mov [ebp-4], eax
	
	mov eax, 1
	mov dword [ebp-12], eax
	
	jmp L2
L1:
	mov eax, [ebp-12]
	imul eax, [ebp-4]
	mov dword [ebp-12], eax
	
	mov eax, [ebp-4]
	sub eax, 1
	mov dword [ebp-4], eax
	
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
	
	push dword STR_1
	call puts
	add esp, 4
	
	call input_int
	
	
	mov dword [ebp-8], eax
	
	push dword [ebp-8]
	call factorial
	add esp, 4
	
	
	mov dword [ebp-12], eax
	
	push dword STR_2
	call puts
	add esp, 4
	
	push dword STR_3
	call puts
	add esp, 4
	
	push dword [ebp-12]
	call print_int
	add esp, 4
	
	leave
	ret
	
