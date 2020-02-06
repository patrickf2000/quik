.data

.text
.global main
.extern puts
.extern printf
.extern exit
.extern fflush
.extern input_int
.extern print_int

main:
	push {fp, lr}
	add fp, sp, #4
	sub sp, sp, #16
	
	
	mov r2, #10
	str r2, [fp, #-8]
	
	mov r2, #5
	str r2, [fp, #-12]
	
	ldr r2, [fp, #-8]
	ldr r3, [fp, #-12]
	add r2, r2, r3
	str r2, [fp, #-16]
	
	ldr r0, [fp, #-16]
	bl print_int
	
	ldr r2, [fp, #-8]
	ldr r3, [fp, #-12]
	add r2, r2, r3
	mov r3, #10
	mul r4, r2, r3
	mov r2, r4
	str r2, [fp, #-20]
	
	ldr r0, [fp, #-20]
	bl print_int
	
	mov r2, #10
	ldr r3, [fp, #-8]
	add r2, r2, r3
	ldr r3, [fp, #-12]
	add r2, r2, r3
	sub r2, r2, #7
	str r2, [fp, #-24]
	
	ldr r0, [fp, #-24]
	bl print_int
	
	ldr r2, [fp, #-8]
	ldr r3, [fp, #-12]
	sub r2, r2, r3
	str r2, [fp, #-28]
	
	ldr r0, [fp, #-28]
	bl print_int
	
	ldr r2, [fp, #-8]
	ldr r3, [fp, #-12]
	mul r4, r2, r3
	mov r2, r4
	str r2, [fp, #-32]
	
	ldr r0, [fp, #-32]
	bl print_int
	
	ldr r2, [fp, #-8]
	ldr r3, [fp, #-12]
	mul r4, r2, r3
	mov r2, r4
	mov r3, #2
	mul r4, r2, r3
	mov r2, r4
	str r2, [fp, #-36]
	
	ldr r0, [fp, #-36]
	bl print_int
	
	sub sp, fp, #4
	pop {fp, pc}
	

