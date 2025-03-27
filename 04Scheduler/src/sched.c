#include "sched.h"
#include "irq.h"
#include "printf.h"


static struct task_struct init_task = INIT_TASK; // the very first task with its task_struct values
struct task_struct *current = &(init_task);		 // points to the currently running task. when kernel boots, sets to init_task
struct task_struct * task[NR_TASKS] = {&(init_task), }; // holds all task_strcuts. only has init_ask at beginning
int nr_tasks = 1;


void _schedule(void)
{
	printf("\n*********** CPU CONTEXT (before scheduling) ***********\n");
	printf("Preempt Count = 0x%x | value: %d\n", &(current->preempt_count), current->preempt_count);
	printf("Priority      = 0x%x | value: %d\n", &(current->priority), current->priority);
	printf("Counter       = 0x%x | value: %d\n", &(current->counter), current->counter);
	printf("State         = 0x%x | value: %d\n", &(current->state), current->state);
	printf("pc            = 0x%x | value: %d\n", &(current->cpu_context.pc), current->cpu_context.pc);
	printf("sp            = 0x%x | value: %d\n", &(current->cpu_context.sp), current->cpu_context.sp);
	printf("fp            = 0x%x | value: %d\n", &(current->cpu_context.fp), current->cpu_context.fp);
	printf("x28           = 0x%x | value: %d\n", &(current->cpu_context.x28), current->cpu_context.x28);
	printf("x27           = 0x%x | value: %d\n", &(current->cpu_context.x27), current->cpu_context.x27);
	printf("x26           = 0x%x | value: %d\n", &(current->cpu_context.x26), current->cpu_context.x26);
	printf("x25           = 0x%x | value: %d\n", &(current->cpu_context.x25), current->cpu_context.x25);
	printf("x24           = 0x%x | value: %d\n", &(current->cpu_context.x24), current->cpu_context.x24);
	printf("x23           = 0x%x | value: %d\n", &(current->cpu_context.x23), current->cpu_context.x23);
	printf("x22           = 0x%x | value: %d\n", &(current->cpu_context.x22), current->cpu_context.x22);
	printf("x21           = 0x%x | value: %d\n", &(current->cpu_context.x21), current->cpu_context.x21);
	printf("x20           = 0x%x | value: %x\n", &(current->cpu_context.x20), current->cpu_context.x20);
	printf("x19           = 0x%x | value: %x\n", &(current->cpu_context.x19), current->cpu_context.x19);
	printf("current-------> 0x%x\n", current);
	printf("*********************************************************\n");


	int next, c;
	struct task_struct * p;
	while (1) {

		printf("Entering _schedule while loop\n");
		c = -1;	// the maximum counter found so far
		next = 0;

		/* Iterates over all tasks and tries to find a task in 
		TASK_RUNNING state with the maximum counter. If such 
		a task is found, we immediately break from the while loop 
		and switch to this task. */

		for (int i = 0; i < NR_TASKS; i++){
			
			p = task[i];
			if (p && p->state == TASK_RUNNING && p->counter > c) { /* NB: p->counter always be non negative */
				c = p->counter;
				next = i;
				
			}
		}
		if (c) {	/* found a RUNNING/READY task w/ the most positive counter.  NB: c won't be -1 as counter always nonnegative */
			printf("found task\n");
			printf("Executing Task present at Index: %d\n", next);
			break;
		}
		printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX NO TASK FOUND XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");

		/* If no such task is found, this is either because i) no 
		task is in TASK_RUNNING state or ii) all such tasks have 0 counters.
		in our current implemenation which misses TASK_WAIT, only condition ii) is possible. 
		Hence, we recharge counters. Bump counters for all tasks once. */
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority; // The increment depends on a task's priority.
			}
		}

		/* loops back to pick the next task */
	}
	printf("while loop broke, and task is getting switched\n");
	switch_to(task[next]);

	printf("\n*********** CPU CONTEXT (after scheduling) ***********\n");
	printf("Preempt Count = 0x%x | value: %d\n", &(current->preempt_count), current->preempt_count);
	printf("Priority      = 0x%x | value: %d\n", &(current->priority), current->priority);
	printf("Counter       = 0x%x | value: %d\n", &(current->counter), current->counter);
	printf("State         = 0x%x | value: %d\n", &(current->state), current->state);
	printf("pc            = 0x%x | value: %d\n", &(current->cpu_context.pc), current->cpu_context.pc);
	printf("sp            = 0x%x | value: %d\n", &(current->cpu_context.sp), current->cpu_context.sp);
	printf("fp            = 0x%x | value: %d\n", &(current->cpu_context.fp), current->cpu_context.fp);
	printf("x28           = 0x%x | value: %d\n", &(current->cpu_context.x28), current->cpu_context.x28);
	printf("x27           = 0x%x | value: %d\n", &(current->cpu_context.x27), current->cpu_context.x27);
	printf("x26           = 0x%x | value: %d\n", &(current->cpu_context.x26), current->cpu_context.x26);
	printf("x25           = 0x%x | value: %d\n", &(current->cpu_context.x25), current->cpu_context.x25);
	printf("x24           = 0x%x | value: %d\n", &(current->cpu_context.x24), current->cpu_context.x24);
	printf("x23           = 0x%x | value: %d\n", &(current->cpu_context.x23), current->cpu_context.x23);
	printf("x22           = 0x%x | value: %d\n", &(current->cpu_context.x22), current->cpu_context.x22);
	printf("x21           = 0x%x | value: %d\n", &(current->cpu_context.x21), current->cpu_context.x21);
	printf("x20           = 0x%x | value: %x\n", &(current->cpu_context.x20), current->cpu_context.x20);
	printf("x19           = 0x%x | value: %x\n", &(current->cpu_context.x19), current->cpu_context.x19);
	printf("current-------> 0x%x\n", current);
	printf("*******************************************************\n");
}

void schedule(void)
{
	current->counter = 0;
	printf("going in\n");
	_schedule();
	printf("came out\n");

}

// where the multitasking magic happens
void switch_to(struct task_struct * next) 
{
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next); // do context switch
}

void schedule_tail(void) {
	/* nothing */
}

