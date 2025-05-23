#include "sys.h"

.globl call_sys_write
call_sys_write:
	mov w8, #SYS_WRITE_NUMBER	
	svc #0
	ret

.globl call_sys_malloc
call_sys_malloc:
	mov w8, #SYS_MALLOC_NUMBER	
	svc #0
	ret

.globl call_sys_exit
call_sys_exit:
	mov w8, #SYS_EXIT_NUMBER	
	svc #0
	ret

.globl call_sys_clone
call_sys_clone:
	/* Save args for the child. After we come from the syscall and back in the task, thread_start() 
		(see below) will pick up fn from x10 & arg from x11. 

		Reason that we cannot leave them in x0/x1/x2: the syscall handler (starting from el0_svc in 
		entry.S) does not have to preserve these regs per calling convention. */
	mov	x10, x0					/*fn*/
	mov	x11, x1					/*arg*/
	mov	x12, x2					/*stack*/

	/* Do the system call ...  */
	mov x0, x2					/* child's user-level stack, needed by the kernel to populate child's pt_regs & task_struct */
	mov	x8, #SYS_CLONE_NUMBER
	svc	0x0						/* ... goes to the kernel's syscall handler */

	/* ... return from the syscall */
	cmp	x0, #0
	beq	thread_start			/* child, jumps to below */
	ret							/*  we're the parent. syscall done. */

thread_start:					/* we're the child ... */
	/* clear FP, for correct stack unwinding at the user level. 
	   stack unwinding will stops when reaching FP=0 */
	mov	x29, 0					

	/* Pick the function arg and execute.  */
	mov	x0, x11
	blr	x10

	/* We are done, pass the return value through x0.  */
	mov	x8, #SYS_EXIT_NUMBER
	svc	0x0
