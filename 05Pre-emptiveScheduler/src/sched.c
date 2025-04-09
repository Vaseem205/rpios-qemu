#include "sched.h"
#include "irq.h"
#include "printf.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

void preempt_disable(void)
{
	printf("*********** CPU CONTEXT (before preempt_disable() ) ***********\n");
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
	printf("x20           = 0x%x | value: 0x%x\n", &(current->cpu_context.x20), current->cpu_context.x20);
	printf("x19           = 0x%x | value: 0x%x\n", &(current->cpu_context.x19), current->cpu_context.x19);
	printf("current-------> 0x%x\n", current);
	printf("*******************************************************\n");
	current->preempt_count++;
	
}

void preempt_enable(void)
{
	current->preempt_count--;
}


void _schedule(void)
{
	/* ensure no context happens in the following code region
		we still leave irq on, because irq handler may set a task to be TASK_RUNNING, which 
		will be picked up by the scheduler below */
	printf("\n\n_schedule()\n\n");
	preempt_disable(); 
	int next,c;
	struct task_struct * p;
	while (1) {
		c = -1; // the maximum counter of all tasks 
		next = 0;

		/* Iterates over all tasks and tries to find a task in 
		TASK_RUNNING state with the maximum counter. If such 
		a task is found, we immediately break from the while loop 
		and switch to this task. */

		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			printf("\nP is pointing to task_%d present at: %x\n",i, p);
			printf("*********** CPU CONTEXT (of task pointed by p: %d) ***********\n", i);
			printf("Preempt Count = 0x%x | value: %d\n", &(p->preempt_count), p->preempt_count);
			printf("Priority      = 0x%x | value: %d\n", &(p->priority), p->priority);
			printf("Counter       = 0x%x | value: %d\n", &(p->counter), p->counter);
			printf("State         = 0x%x | value: %d\n", &(p->state), p->state);
			printf("pc            = 0x%x | value: %d\n", &(p->cpu_context.pc), p->cpu_context.pc);
			printf("sp            = 0x%x | value: %d\n", &(p->cpu_context.sp), p->cpu_context.sp);
			printf("fp            = 0x%x | value: %d\n", &(p->cpu_context.fp), p->cpu_context.fp);
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
			printf("*******************************************************\n");
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c) {
			break;
		}

		/* If no such task is found, this is either because i) no 
		task is in TASK_RUNNING state or ii) all such tasks have 0 counters.
		in our current implemenation which misses TASK_WAIT, only condition ii) is possible. 
		Hence, we recharge counters. Bump counters for all tasks once. */
		
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}
	switch_to(task[next]);
	preempt_enable();
}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}

void switch_to(struct task_struct * next) 
{
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next;

	printf("\n******************************************************* CONTEXT SWITCH *******************************************************\n");
	printf("Preempt Count 		= 0x%x | value: %d\t\t\tPreempt Count 		= 0x%x | value: %d \n", &(prev->preempt_count),		prev->preempt_count, 	&(current->preempt_count), 		current->preempt_count);
	printf("Priority      		= 0x%x | value: %d\t\t\tPriority      		= 0x%x | value: %d \n", &(prev->priority), 			prev->priority, 		&(current->priority), 			current->priority);
	printf("Counter       		= 0x%x | value: %d\t\t\tCounter       		= 0x%x | value: %d \n", &(prev->counter), 			prev->counter,			&(current->counter), 			current->counter);
	printf("State         		= 0x%x | value: %d\t\t\tState         		= 0x%x | value: %d \n", &(prev->state), 				prev->state,			&(current->state), 				current->state);
	printf("pc            		= 0x%x | value: 0x%x\t\tpc            		= 0x%x | value: 0x%x \n", &(prev->cpu_context.pc), 	prev->cpu_context.pc, 	&(current->cpu_context.pc), 	current->cpu_context.pc);
	printf("sp            		= 0x%x | value: 0x%x\tsp            		= 0x%x | value: 0x%x \n", &(prev->cpu_context.sp), 	prev->cpu_context.sp, 	&(current->cpu_context.sp), 	current->cpu_context.sp);
	printf("fp            		= 0x%x | value: 0x%x\t\t\tfp            	= 0x%x | value: 0x%x \n", &(prev->cpu_context.fp), 	prev->cpu_context.fp, 	&(current->cpu_context.fp),		current->cpu_context.fp);
	printf("x28           		= 0x%x | value: %d\t\t\tx28           		= 0x%x | value: %d \n", &(prev->cpu_context.x28), 	prev->cpu_context.x28, 	&(current->cpu_context.x28), 	current->cpu_context.x28);
	printf("x27           		= 0x%x | value: %d -------> \tx27           = 0x%x | value: %d \n", &(prev->cpu_context.x27), 	prev->cpu_context.x27, 	&(current->cpu_context.x27), 	current->cpu_context.x27);
	printf("x26           		= 0x%x | value: %d\t\t\tx26           		= 0x%x | value: %d \n", &(prev->cpu_context.x26), 	prev->cpu_context.x26, 	&(current->cpu_context.x26), 	current->cpu_context.x26);
	printf("x25           		= 0x%x | value: %d\t\t\tx25           		= 0x%x | value: %d \n", &(prev->cpu_context.x25), 	prev->cpu_context.x25, 	&(current->cpu_context.x25), 	current->cpu_context.x25);
	printf("x24           		= 0x%x | value: %d\t\t\tx24           		= 0x%x | value: %d \n", &(prev->cpu_context.x24), 	prev->cpu_context.x24, 	&(current->cpu_context.x24), 	current->cpu_context.x24);
	printf("x23           		= 0x%x | value: %d\t\t\tx23           		= 0x%x | value: %d \n", &(prev->cpu_context.x23), 	prev->cpu_context.x23, 	&(current->cpu_context.x23), 	current->cpu_context.x23);
	printf("x22           		= 0x%x | value: %d\t\t\tx22           		= 0x%x | value: %d \n", &(prev->cpu_context.x22), 	prev->cpu_context.x22, 	&(current->cpu_context.x22), 	current->cpu_context.x22);
	printf("x21           		= 0x%x | value: %d\t\t\tx21           		= 0x%x | value: %d \n", &(prev->cpu_context.x21), 	prev->cpu_context.x21, 	&(current->cpu_context.x21), 	current->cpu_context.x21);
	printf("x20 [args]			= 0x%x | value: 0x%x\t\tx20 [args]			= 0x%x | value: 0x%x \n", &(prev->cpu_context.x20), 	prev->cpu_context.x20, 	&(current->cpu_context.x20), 	current->cpu_context.x20);
	printf("x19 [process()]		= 0x%x | value: 0x%x\t\tx19 [process()]		= 0x%x | value: 0x%x \n", &(prev->cpu_context.x19), 	prev->cpu_context.x19,	&(current->cpu_context.x19), 						current->cpu_context.x19);
	printf("\ncurrent-------> 0x%x\n", current);
	printf("*****************************************************************************************************************************\n");

	cpu_switch_to(prev, next);  /* will branch to @next->cpu_context.pc ...*/
}

void schedule_tail(void) {
	preempt_enable();
}


void timer_tick()
{
	--current->counter;
	if (current->counter > 0 || current->preempt_count > 0) 
		return;
	current->counter=0;

	/* Note: we just came from an interrupt handler and CPU just automatically disabled all interrupts. 
		Now call scheduler with interrupts enabled */
	enable_irq();
	_schedule();
	/* disable irq until kernel_exit, in which eret will resort the interrupt flag from spsr, which sets it on. */
	disable_irq(); 
}
