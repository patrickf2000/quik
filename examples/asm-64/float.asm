.intel_syntax noprefix
.data
	flt_1: .long 1067299373
	flt_2: .long 1080255578
	flt_3: .long 1078875652
	STR_1: .string "Add: %f\n"
	flt_4: .long 1067299373
	flt_5: .long 1080255578
	flt_6: .long 1078875652
	STR_2: .string "Sub: %f\n"
	flt_7: .long 1067299373
	flt_8: .long 1080255578
	flt_9: .long 1078875652
	STR_3: .string "Mul: %f\n"
	flt_10: .long 1067299373
	flt_11: .long 1080255578
	flt_12: .long 1078875652
	STR_4: .string "Div: %f\n"

.bss

.text
	.extern puts
	.extern printf
	.extern exit
	.extern fflush
	.extern input_int
	.extern print_int
	.global add_test
	.global sub_test
	.global mul_test
	.global div_test
	.global main

add_test:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	movss xmm0, [flt_1]
	movss [rbp-8], xmm0
	
	movss xmm0, [flt_2]
	movss [rbp-12], xmm0
	
	movss xmm0, [rbp-8]
	addss xmm0, [rbp-12]
	movss [rbp-16], xmm0
	
	movss xmm0, [rbp-16]
	addss xmm0, [flt_3]
	movss [rbp-16], xmm0
	
	mov edi, OFFSET FLAT:STR_1
	movss xmm0, [rbp-16]
	cvtss2sd xmm0, xmm0
	call printf
	
	leave
	ret
	
sub_test:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	movss xmm0, [flt_4]
	movss [rbp-8], xmm0
	
	movss xmm0, [flt_5]
	movss [rbp-12], xmm0
	
	movss xmm0, [rbp-8]
	subss xmm0, [rbp-12]
	movss [rbp-16], xmm0
	
	movss xmm0, [rbp-16]
	addss xmm0, [flt_6]
	movss [rbp-16], xmm0
	
	mov edi, OFFSET FLAT:STR_2
	movss xmm0, [rbp-16]
	cvtss2sd xmm0, xmm0
	call printf
	
	leave
	ret
	
mul_test:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	movss xmm0, [flt_7]
	movss [rbp-8], xmm0
	
	movss xmm0, [flt_8]
	movss [rbp-12], xmm0
	
	movss xmm0, [rbp-8]
	mulss xmm0, [rbp-12]
	movss [rbp-16], xmm0
	
	movss xmm0, [rbp-16]
	addss xmm0, [flt_9]
	movss [rbp-16], xmm0
	
	mov edi, OFFSET FLAT:STR_3
	movss xmm0, [rbp-16]
	cvtss2sd xmm0, xmm0
	call printf
	
	leave
	ret
	
div_test:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	
	movss xmm0, [flt_10]
	movss [rbp-8], xmm0
	
	movss xmm0, [flt_11]
	movss [rbp-12], xmm0
	
	movss xmm0, [rbp-8]
	divss xmm0, [rbp-12]
	movss [rbp-16], xmm0
	
	movss xmm0, [rbp-16]
	addss xmm0, [flt_12]
	movss [rbp-16], xmm0
	
	mov edi, OFFSET FLAT:STR_4
	movss xmm0, [rbp-16]
	cvtss2sd xmm0, xmm0
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
	
