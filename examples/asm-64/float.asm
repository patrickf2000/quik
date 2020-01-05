section .data
	flt_1 dq 1.232000
	flt_2 dq 3.553000
	flt_3 dq 3.224000
	STR_1 db "Add: %f",0xA,0
	flt_4 dq 1.232000
	flt_5 dq 3.553000
	flt_6 dq 3.224000
	STR_2 db "Sub: %f",0xA,0
	flt_7 dq 1.232000
	flt_8 dq 3.553000
	flt_9 dq 3.224000
	STR_3 db "Mul: %f",0xA,0
	flt_10 dq 1.232000
	flt_11 dq 3.553000
	flt_12 dq 3.224000
	STR_4 db "Div: %f",0xA,0

section .bss

section .text
	extern puts
	extern printf
	extern exit
	extern fflush
	extern input_int
	extern print_int
	global add_test
	global sub_test
	global mul_test
	global div_test
	global main

add_test:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	movq xmm0, [flt_1]
	movsd [rbp-12], xmm0
	
	movq xmm0, [flt_2]
	movsd [rbp-20], xmm0
	
	movq xmm0, [rbp-12]
	addsd xmm0, [rbp-20]
	movsd [rbp-28], xmm0
	
	movq xmm0, [rbp-28]
	addsd xmm0, [flt_3]
	movsd [rbp-28], xmm0
	
	mov edi, STR_1
	movq xmm0, [rbp-28]
	call printf
	
	leave
	ret
	
sub_test:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	movq xmm0, [flt_4]
	movsd [rbp-12], xmm0
	
	movq xmm0, [flt_5]
	movsd [rbp-20], xmm0
	
	movq xmm0, [rbp-12]
	subsd xmm0, [rbp-20]
	movsd [rbp-28], xmm0
	
	movq xmm0, [rbp-28]
	addsd xmm0, [flt_6]
	movsd [rbp-28], xmm0
	
	mov edi, STR_2
	movq xmm0, [rbp-28]
	call printf
	
	leave
	ret
	
mul_test:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	movq xmm0, [flt_7]
	movsd [rbp-12], xmm0
	
	movq xmm0, [flt_8]
	movsd [rbp-20], xmm0
	
	movq xmm0, [rbp-12]
	mulsd xmm0, [rbp-20]
	movsd [rbp-28], xmm0
	
	movq xmm0, [rbp-28]
	addsd xmm0, [flt_9]
	movsd [rbp-28], xmm0
	
	mov edi, STR_3
	movq xmm0, [rbp-28]
	call printf
	
	leave
	ret
	
div_test:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	movq xmm0, [flt_10]
	movsd [rbp-12], xmm0
	
	movq xmm0, [flt_11]
	movsd [rbp-20], xmm0
	
	movq xmm0, [rbp-12]
	divsd xmm0, [rbp-20]
	movsd [rbp-28], xmm0
	
	movq xmm0, [rbp-28]
	addsd xmm0, [flt_12]
	movsd [rbp-28], xmm0
	
	mov edi, STR_4
	movq xmm0, [rbp-28]
	call printf
	
	leave
	ret
	
main:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	call add_test
	
	call sub_test
	
	call mul_test
	
	call div_test
	
	mov edi, 0
	call fflush
	
	leave
	ret
	
