section .data
	int_fmt db "%d",10,0
	str_fmt db "%s",10,0
	flt_fmt db "%f",10,0
	STR_1 db "Hello, world!",0

section .text
	global main
	extern printf

main:
	push dword STR_1
	push dword str_fmt
	call printf
	
	mov eax, 1
	mov ebx, 0
	int 0x80
	
