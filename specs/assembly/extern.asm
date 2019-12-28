section .data
	STR_1 db "Calling the LS command.",0
	STR_2 db "ls -l",0
	STR_3 db "",0
	STR_4 db "Exiting with code of 2.",0

section .bss

section .text
	extern puts
	extern system
	extern exit
	global main

main:
	push dword STR_1
	call puts
	
	push dword STR_2
	call system
	
	push dword STR_3
	call puts
	
	push dword STR_4
	call puts
	
	push 2
	call exit
	
	mov eax, 1
	mov ebx, 0
	int 0x80
	
