.intel_syntax noprefix
.data

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
	
	mov DWORD PTR [ebp-8], 10
	
	mov DWORD PTR [ebp-12], 5
	
	mov eax, [ebp-8]
	add eax, [ebp-12]
	mov DWORD PTR[ebp-16], eax
	
	push DWORD PTR [ebp-16]
	call print_int
	add esp, 4
	
	mov eax, [ebp-8]
	add eax, [ebp-12]
	imul eax, 10
	mov DWORD PTR[ebp-20], eax
	
	push DWORD PTR [ebp-20]
	call print_int
	add esp, 4
	
	mov eax, 10
	add eax, [ebp-8]
	add eax, [ebp-12]
	sub eax, 7
	mov DWORD PTR[ebp-24], eax
	
	push DWORD PTR [ebp-24]
	call print_int
	add esp, 4
	
	mov eax, [ebp-8]
	sub eax, [ebp-12]
	mov DWORD PTR[ebp-28], eax
	
	push DWORD PTR [ebp-28]
	call print_int
	add esp, 4
	
	mov eax, [ebp-8]
	imul eax, [ebp-12]
	mov DWORD PTR[ebp-32], eax
	
	push DWORD PTR [ebp-32]
	call print_int
	add esp, 4
	
	mov eax, [ebp-8]
	imul eax, [ebp-12]
	imul eax, 2
	mov DWORD PTR[ebp-36], eax
	
	push DWORD PTR [ebp-36]
	call print_int
	add esp, 4
	
	leave
	ret
	
