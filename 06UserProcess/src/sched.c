#include "sched.h"
#include "irq.h"
#include "printf.h"
#include "fork.h"
#include "utils.h"
#include "mm.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

void preempt_disable(void)
{
	current->preempt_count++;
}

void preempt_enable(void)
{
	current->preempt_count--;
}


void _schedule(void)
{
	printf("Entered _schedule() [sched.c]\n");
	preempt_disable();
	int next,c;
	struct task_struct * p;
	while (1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}
	printf("\nSwitching Task: %d -> %d\n", next-1, next);
	switch_to(task[next], next);
	printf("\nback to _schedule()\n");
	preempt_enable();
}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}


void switch_to(struct task_struct * next, int index) 
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


	cpu_switch_to(prev, next);
	printf("return from process execution\n");
}

void schedule_tail(void) {
	preempt_enable();
}

void timer_tick()
{
	--current->counter;
	if (current->counter>0 || current->preempt_count >0) {
		return;
	}
	current->counter=0;
	enable_irq();
	_schedule();
	disable_irq();
}

void exit_process(){
	preempt_disable();
	for (int i = 0; i < NR_TASKS; i++){
		if (task[i] == current) {
			task[i]->state = TASK_ZOMBIE;
			break;
		}
	}
	if (current->stack) {
		free_page(current->stack);
	}
	preempt_enable();
	schedule();
}
