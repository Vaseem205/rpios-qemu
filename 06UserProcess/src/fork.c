#include "mm.h"
#include "sched.h"
#include "printf.h"
#include "fork.h"
#include "entry.h"
#include "utils.h"

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long stack)
{
	preempt_disable();
	struct task_struct *p;

	p = (struct task_struct *) get_free_page();
	if (!p) {
		return -1;
	}

	struct pt_regs *childregs = task_pt_regs(p);
	memzero((unsigned long)childregs, sizeof(struct pt_regs));
	memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));

	if (clone_flags & PF_KTHREAD) { 
		/* kernel task: will start from within kernel code; stack will be empty then. no pt_regs to populate*/
		p->cpu_context.x19 = fn;
		p->cpu_context.x20 = arg;
	} else {	
		/* user task: will exit kernel to user level. So populate pt_regs which is at the task's kernel stack top */
		struct pt_regs * cur_regs = task_pt_regs(current);
		/* Copy cur_regs to childregs. It's a NOT bug, b/c it will be compiled to invocation of memcpy(), 
			and our memcpy() has src/dst reversed. To reverse below, memcpy() has to be fixed as well...*/
		*cur_regs = *childregs;
		childregs->regs[0] = 0; /* return val of the clone() syscall */ 
		childregs->sp = stack + PAGE_SIZE; 		/* the top of child's user-level stack */
		p->stack = stack;						/* also save it in order to do a cleanup after the task finishes */
	}
	p->flags = clone_flags;
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
	p->preempt_count = 1; //disable preemtion until schedule_tail

	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs;

	printf("\n*********** COPY PROCESS CONTEXT ***********\n");
	printf("Preempt Count = 0x%x | value: %d\n", &(p->preempt_count), p->preempt_count);
	printf("Priority      = 0x%x | value: %d\n", &(p->priority), p->priority);
	printf("Counter       = 0x%x | value: %d\n", &(p->counter), p->counter);
	printf("State         = 0x%x | value: %d\n", &(p->state), p->state);
	printf("pc            = 0x%x | value: 0x%x\n", &(p->cpu_context.pc), p->cpu_context.pc);
	printf("sp            = 0x%x | value: 0x%x\n", &(p->cpu_context.sp), p->cpu_context.sp);
	printf("fp            = 0x%x | value: 0x%x\n", &(p->cpu_context.fp), p->cpu_context.fp);
	printf("x28           = 0x%x | value: %d\n", &(p->cpu_context.x28), p->cpu_context.x28);
	printf("x27           = 0x%x | value: %d\n", &(p->cpu_context.x27), p->cpu_context.x27);
	printf("x26           = 0x%x | value: %d\n", &(p->cpu_context.x26), p->cpu_context.x26);
	printf("x25           = 0x%x | value: %d\n", &(p->cpu_context.x25), p->cpu_context.x25);
	printf("x24           = 0x%x | value: %d\n", &(p->cpu_context.x24), p->cpu_context.x24);
	printf("x23           = 0x%x | value: %d\n", &(p->cpu_context.x23), p->cpu_context.x23);
	printf("x22           = 0x%x | value: %d\n", &(p->cpu_context.x22), p->cpu_context.x22);
	printf("x21           = 0x%x | value: %d\n", &(p->cpu_context.x21), p->cpu_context.x21);
	printf("x20           = 0x%x | value: 0x%x\n", &(p->cpu_context.x20), p->cpu_context.x20);
	printf("x19           = 0x%x | value: 0x%x\n", &(p->cpu_context.x19), p->cpu_context.x19);
	printf("current-------> 0x%x\n", current);
	printf("*********************************************\n");

	int pid = nr_tasks++;
	task[pid] = p;	
	preempt_enable();
	return pid;
}

int move_to_user_mode(unsigned long pc)
{
	/* Convert a kernel task to a user task, which must have legit pt_regs which 
	are expected by kernel_exit() as we move to user level for 1st time */		

	printf("\n************ move_to_user_mode() ************\n");
		
	struct pt_regs *regs = task_pt_regs(current);

	printf("regs:\t\t\t0x%x\n", regs);
	printf("sizeof(*regs):\t0x%x\n", sizeof(*regs));

	/*
	
	it'll zero down 34 8-Bytes Block of memmory

	ex: 
	(unsigned long)regs = 0x400ef0	-> gets stored in register x0
	sizeof(*regs)		= 0x110		-> gets stored in register x1

	in memzero (asm macro), we will zero down 0x110 Bytes of memory in decrementing order,
	starting from 0x400ef0 to 0x400ff0, all values will be 0

	NOTE: 34 8-Bytes blocks will have 0s. 
	
	*/
	memzero((unsigned long)regs, sizeof(*regs)); 
	regs->pc = pc; /* points to the first instruction to be executed once the task lands in user mode via eret */
	regs->pstate = PSR_MODE_EL0t; /* to be copied by kernel_exit() to SPSR_EL1. then eret will take care of it */
	
	/* Allocate a new user stack, in addition to the task's existing kernel stack */
	unsigned long stack = get_free_page();
	if (!stack) {
		return -1;
	}
	regs->sp = stack + PAGE_SIZE; 
	

	printf("\n========= regs context =========\n");
	printf("regs->sp:\t\t0x%x (stack + PAGE_SIZE: 0x%x + %x)\n", regs->sp, stack, PAGE_SIZE);
	printf("regs->pc:\t\t0x%x\n", regs->pc);
	printf("regs->pstate:\t0x%x\n", regs->pstate);
	printf("\n");

	printf("Initial value of 'current->stack': 0x%x\n", current->stack);
	current->stack = stack;
	printf("Updated value of 'current->stack': 0x%x\n", current->stack);
	printf("\n*********************************************\n");

	return 0;
}

/* get a task's saved registers, which are at the top of the task's kernel page. 
   these regs are saved/restored by kernel_entry()/kernel_exit(). 
*/
struct pt_regs * task_pt_regs(struct task_struct *tsk) {

	printf("\n\t======== task_pt_regs() =========\n");

	printf("\ttsk:\t\t\t\t\t%x\n", tsk);
	printf("\tTHREAD_SIZE:\t\t\t%d\n", THREAD_SIZE);
	printf("\tsizeof(struct pt_regs):\t0x%x\n", sizeof(struct pt_regs));

	unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
	printf("\tp :\t\t\t\t\t\t0x%x\n", p);
	printf("\n");
	return (struct pt_regs *)p;
}
