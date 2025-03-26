#include "mm.h"
#include "sched.h"
#include "entry.h"
#include "printf.h"

// @fn: a function to execute in a new task 
// @arg: an argument passed to this function

// Example: copy_process((unsigned long)&process, (unsigned long)"12345");

int copy_process(unsigned long fn, unsigned long arg)
{
	struct task_struct *p;


	printf("Setting the context of process:\n");
	printf("fork.c\t\t-> address of passed function (process): %x\n", fn);
	printf("fork.c\t\t-> value of passed function (process): %s\n", arg);

	// Next, a new page is allocated. At the bottom of this page, we are putting 
	// the `task_struct` for the newly created task. The rest of this page will be used as the task stack.
	p = (struct task_struct *) get_free_page(); // returns a 4KB page (of which starting bytes are occupied by task_struct)
	if (!p)
		return 1;
	printf("p is pointing to: %x", p);
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;	/* inherit priority. default: 1. see sched.h */

	// Here `cpu_context` is initialized. 
	p->cpu_context.x19 = fn;
	p->cpu_context.x20 = arg;
	p->cpu_context.pc = (unsigned long)ret_from_fork;	// pc is pointing to assemlby code, in which it loads the process function with arguments
	// The stack pointer is set to the top of the newly allocated memory page
	p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;

	
	int pid = nr_tasks++;
	task[pid] = p;	
	return 0;
}
