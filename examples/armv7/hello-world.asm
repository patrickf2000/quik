.data
	.d_STR0: .ascii "Hello, world!\000"

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
	
	
	ldr r0, .STR0
	bl puts
	
	sub sp, fp, #4
	pop {fp, pc}
	

.STR0: .word .d_STR0
