#include "sched.h"
#include "irq.h"
#include "printf.h"


static struct task_struct init_task = INIT_TASK; // the very first task with its task_struct values
struct task_struct *current = &(init_task);		 // points to the currently running task. when kernel boots, sets to init_task
struct task_struct * task[NR_TASKS] = {&(init_task), }; // holds all task_strcuts. only has init_ask at beginning
int nr_tasks = 1;


void _schedule(void)
{
	int next, c;
	struct task_struct * p;
	while (1) {

		printf("\nEntering _schedule while loop\n");
		c = -1;	// the maximum counter found so far
		next = 0;

		/* Iterates over all tasks and tries to find a task in 
		TASK_RUNNING state with the maximum counter. If such 
		a task is found, we immediately break from the wh40" width="160" height="180" rx="5" fill="#E1F5FE" stroke="#0288D1" stroke-width="2"/>
  <rect x="320" y="ile loop 
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
			printf("x20           = 0x%x | value: %x\n", &(p->cpu_context.x20), p->cpu_context.x20);
			printf("x19           = 0x%x | value: %x\n", &(p->cpu_context.x19), p->cpu_context.x19);
			printf("current-------> 0x%x\n", current);
			printf("*******************************************************\n");

			if (p && p->state == TASK_RUNNING && p->counter > c) { /* NB: p->counter always be non negative */
				printf("task_%d qualified the following condition:\n", i);
				printf("- p\t\t\t\t!= NULL\n");
				printf("- p->state \t\t== TASK_RUNNING (p->state)\n");
				printf("- p->counter(%d)\t>  %d\n",p->counter, c);
				c = p->counter;
				// printf("c: %d\n", c);
				next = i;
			}else if(p->counter >! c){
				printf("task_%d counter value is not greater than task_%d counter value\n", i, i-1);
			}
			// printf("task_%d has qualified the if condition so far\n", next);
			// printf("Exiting for loop\n");
		}
		if (c) {	/* found a RUNNING/READY task w/ the most positive counter.  NB: c won't be -1 as counter always nonnegative */
			break;
		}
		printf("\nBack to INIT_TASK\n");

		/* If no such task is found, this is either because 
		i) 	no task is in TASK_RUNNING state
		ii)	all such tasks have 0 counters.
		in our current implemenation which misses TASK_WAIT, only condition ii) is possible. 
		Hence, we recharge counters. Bump counters for all tasks once. */
		printf("\n\nUPDATING COUNTER VALUES OF ALL TASKS:\n");
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			printf("\nP is pointing to task_%d present at: %x\n",i, task[i]);
			if (p) {
				printf("- p[%d]->priority (current priority):\t%d\n",i, p->priority);
				printf("- p[%d]->counter (before computation):\t%d\n",i, p->counter);
				p->counter = (p->counter >> 1) + p->priority; // The increment depends on a task's priority.
				printf("- p[%d]->counter (after computation):\t%d <-updated\n",i, p->counter);
			}
		}

		/* loops back to pick the next task */
	}
	printf("\ntask_%d present at 0x%x qualified conditons, hence we gonna context switch and execute task_%d\n", next, task[next], next);
	switch_to(task[next]);
}

void schedule(void)
{
	current->counter = 0;
	printf("\nInside schedule()\n");
	_schedule();
	printf("\nExiting schedule()\n");

}

// where the multitasking magic happens
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
	printf("pc            		= 0x%x | value: %d\t\tpc            		= 0x%x | value: %d \n", &(prev->cpu_context.pc), 	prev->cpu_context.pc, 	&(current->cpu_context.pc), 	current->cpu_context.pc);
	printf("sp            		= 0x%x | value: %d\t\tsp            		= 0x%x | value: %d \n", &(prev->cpu_context.sp), 	prev->cpu_context.sp, 	&(current->cpu_context.sp), 	current->cpu_context.sp);
	printf("fp            		= 0x%x | value: %d\t\t\tfp            		= 0x%x | value: %d \n", &(prev->cpu_context.fp), 	prev->cpu_context.fp, 	&(current->cpu_context.fp),		current->cpu_context.fp);
	printf("x28           		= 0x%x | value: %d\t\t\tx28           		= 0x%x | value: %d \n", &(prev->cpu_context.x28), 	prev->cpu_context.x28, 	&(current->cpu_context.x28), 	current->cpu_context.x28);
	printf("x27           		= 0x%x | value: %d -------> \tx27           		= 0x%x | value: %d \n", &(prev->cpu_context.x27), 	prev->cpu_context.x27, 	&(current->cpu_context.x27), 	current->cpu_context.x27);
	printf("x26           		= 0x%x | value: %d\t\t\tx26           		= 0x%x | value: %d \n", &(prev->cpu_context.x26), 	prev->cpu_context.x26, 	&(current->cpu_context.x26), 	current->cpu_context.x26);
	printf("x25           		= 0x%x | value: %d\t\t\tx25           		= 0x%x | value: %d \n", &(prev->cpu_context.x25), 	prev->cpu_context.x25, 	&(current->cpu_context.x25), 	current->cpu_context.x25);
	printf("x24           		= 0x%x | value: %d\t\t\tx24           		= 0x%x | value: %d \n", &(prev->cpu_context.x24), 	prev->cpu_context.x24, 	&(current->cpu_context.x24), 	current->cpu_context.x24);
	printf("x23           		= 0x%x | value: %d\t\t\tx23           		= 0x%x | value: %d \n", &(prev->cpu_context.x23), 	prev->cpu_context.x23, 	&(current->cpu_context.x23), 	current->cpu_context.x23);
	printf("x22           		= 0x%x | value: %d\t\t\tx22           		= 0x%x | value: %d \n", &(prev->cpu_context.x22), 	prev->cpu_context.x22, 	&(current->cpu_context.x22), 	current->cpu_context.x22);
	printf("x21           		= 0x%x | value: %d\t\t\tx21           		= 0x%x | value: %d \n", &(prev->cpu_context.x21), 	prev->cpu_context.x21, 	&(current->cpu_context.x21), 	current->cpu_context.x21);
	printf("x20 [args]			= 0x%x | value: %x\t\tx20 [args]			= 0x%x | value: %x \n", &(prev->cpu_context.x20), 	prev->cpu_context.x20, 	&(current->cpu_context.x20), 	current->cpu_context.x20);
	printf("x19 [process()]		= 0x%x | value: %x\t\tx19 [process()]		= 0x%x | value: %x \n", &(prev->cpu_context.x19), 	prev->cpu_context.x19,	&(current->cpu_context.x19), 						current->cpu_context.x19);
	printf("\ncurrent-------> 0x%x\n", current);
	printf("*****************************************************************************************************************************\n");
	cpu_switch_to(prev, next); // do context switch
}

void schedule_tail(void) {
	/* nothing */
}

