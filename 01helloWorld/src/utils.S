.globl put32
put32:
	str w1,[x0]	/* stores the 32-bit value from register w1 into the memory location pointed to by x0. */
	ret

.globl get32
get32:
	ldr w0,[x0]	/* stores the 32-bit value from register w0 into the memory location pointed to by x0. */
	ret

/*
Before calling delay, you would load x0 with this calculated number.
The function will then loop, decrementing x0 until it reaches zero, effectively introducing delay in execution (as CPU is busy in executing this instructions)
*/
.globl delay
delay:
	subs x0, x0, #1
	bne delay	/* if the substraction is non-zero itll branch to delay.  */
	ret
