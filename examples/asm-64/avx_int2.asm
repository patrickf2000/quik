.intel_syntax noprefix
.data
	flt_1: .long 1066192077
	flt_2: .long 1074580685
	flt_3: .long 1079194419
	flt_4: .long 1082969293
	flt_5: .long 1085276160
	flt_6: .long 1087583027
	flt_7: .long 1089889894
	flt_8: .long 1091357901
	STR_1: .string "%f\n"

.bss

.text
	.extern puts
	.extern printf
	.extern exit
	.extern fflush
	.extern input_int
	.extern print_int
	.global add_float
	.global add_int
	.global main

add_float:
	push rbp
	mov rbp, rsp
	sub rsp, 128
	
	movss xmm0, [flt_1]
	movss [rbp-36], xmm0
	movss xmm0, [flt_2]
	movss [rbp-32], xmm0
	movss xmm0, [flt_3]
	movss [rbp-28], xmm0
	movss xmm0, [flt_4]
	movss [rbp-24], xmm0
	movss xmm0, [flt_5]
	movss [rbp-20], xmm0
	movss xmm0, [flt_6]
	movss [rbp-16], xmm0
	movss xmm0, [flt_7]
	movss [rbp-12], xmm0
	movss xmm0, [flt_8]
	movss [rbp-8], xmm0
	
	vmovupd ymm0, [rbp-36]
	vmovupd ymm1, [rbp-36]
	vhaddps ymm0, ymm0, ymm1
	
	vextractf128 xmm4, ymm0, 0
	vextractf128 xmm5, ymm0, 1
	
	extractps [rbp-52], xmm4, 0
	extractps [rbp-48], xmm4, 1
	extractps [rbp-44], xmm5, 0
	extractps [rbp-40], xmm5, 1
	
	
	mov DWORD PTR [rbp-56], 0
	
	jmp L2
L1:
	mov eax, [rbp-56]
	cdqe
	mov rbx, rax
	mov rax, [rbp-52]
	imul rbx, 4
	movss xmm0, [rbp-52+rbx]
	movss [rbp-60], xmm0
	
	mov edi, OFFSET FLAT:STR_1
	movss xmm0, [rbp-60]
	cvtss2sd xmm0, xmm0
	call printf
	
	add DWORD PTR [rbp-56], 1
	
L2:
	mov eax, [rbp-56]
	cmp eax, 4
	jl L1
	
	leave
	ret
	
add_int:
	push rbp
	mov rbp, rsp
	sub rsp, 196
	
	mov DWORD PTR [rbp-36], 1
	mov DWORD PTR [rbp-32], 2
	mov DWORD PTR [rbp-28], 3
	mov DWORD PTR [rbp-24], 4
	mov DWORD PTR [rbp-20], 5
	mov DWORD PTR [rbp-16], 6
	mov DWORD PTR [rbp-12], 7
	mov DWORD PTR [rbp-8], 8
	
	vmovupd ymm0, [rbp-36]
	vmovupd ymm1, [rbp-36]
	vphaddd ymm0, ymm0, ymm1
	
	vextracti128 xmm4, ymm0, 0
	vextracti128 xmm5, ymm0, 1
	
	pextrd eax, xmm4, 0
	pinsrd xmm3, eax, 0
	
	pextrd eax, xmm4, 1
	pinsrd xmm3, eax, 1
	
	pextrd eax, xmm5, 0
	pinsrd xmm3, eax, 2
	
	pextrd eax, xmm5, 1
	pinsrd xmm3, eax, 3
	
	movups [rbp-52], xmm3
	
	mov DWORD PTR [rbp-56], 0
	
	jmp L4
L3:
	mov eax, [rbp-56]
	cdqe
	mov rbx, rax
	mov rax, [rbp-52]
	imul rbx, 4
	mov eax, [rbp-52+rbx]
	mov DWORD PTR [rbp-60], eax
	
	mov eax, [rbp-60]
	mov edi, eax
	call print_int
	
	add DWORD PTR [rbp-56], 1
	
L4:
	mov eax, [rbp-56]
	cmp eax, 4
	jl L3
	
	leave
	ret
	
main:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	call add_float
	
	call add_int
	
	leave
	ret
	
