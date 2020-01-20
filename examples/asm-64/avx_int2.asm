section .data

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
	push rbp
	mov rbp, rsp
	sub rsp, 196
	
	mov dword [rbp-36], 1
	mov dword [rbp-32], 2
	mov dword [rbp-28], 3
	mov dword [rbp-24], 4
	mov dword [rbp-20], 5
	mov dword [rbp-16], 6
	mov dword [rbp-12], 7
	mov dword [rbp-8], 8
	
	vmovupd ymm0, [rbp-36]
	vmovupd ymm1, [rbp-36]
	vphaddd ymm0, ymm0, ymm1
	
	vextracti128 xmm4, ymm0, 0
	vextracti128 xmm5, ymm0, 1
	
	pextrd eax, xmm4, 0
	pextrd ebx, xmm4, 1
	pextrd ecx, xmm5, 0
	pextrd edx, xmm5, 1
	
	pinsrd xmm3, eax, 0
	pinsrd xmm3, ebx, 1
	pinsrd xmm3, ecx, 2
	pinsrd xmm3, edx, 3
	
	movups [rbp-52], xmm3
	
	mov dword [rbp-56], 0
	
	jmp L2
L1:
	mov eax, [rbp-56]
	cdqe
	mov rbx, rax
	mov rax, [rbp-52]
	imul rbx, 4
	mov eax, [rbp-52+rbx]
	mov dword [rbp-60], eax
	
	mov eax, [rbp-60]
	mov edi, eax
	call print_int
	
	add dword [rbp-56], 1
	
L2:
	mov eax, [rbp-56]
	cmp eax, 4
	jl L1
	
	leave
	ret
	
