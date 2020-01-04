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
	push ebp
	mov ebp, esp
	sub esp, 48
	
	mov eax, [ebp+8]
	mov [ebp-4], eax
	
	push dword [ebp-4]
	call strlen
	add esp, 4
	
	
	mov dword [ebp-12], eax
	
	mov eax, 0
	mov dword [ebp-16], eax
	
	mov eax, 0
	mov byte [ebp-17], al
	
	jmp L2
L1:
	mov eax, [ebp-4]
	mov ebx, [ebp-16]
	imul ebx, 1
	mov eax, [eax+ebx]
	
	mov byte [ebp-17], al
	
	push dword [ebp-17]
	push dword STR_1
	call printf
	add esp, 4
	add esp, 4
	
	push 0
	call fflush
	add esp, 4
	
	mov eax, [ebp-16]
	add eax, 1
	mov dword [ebp-16], eax
	
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
	
	push dword STR_2
	call puts
	add esp, 4
	
	push dword STR_3
	call count_char
	add esp, 4
	
	leave
	ret
	
