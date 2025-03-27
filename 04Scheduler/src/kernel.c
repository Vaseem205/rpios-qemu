#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"

#define CHAR_DELAY (5 * 5000000)


void process(char *array)
{
	
	

	while (1){
		// printf("\n------------------------------------------> entered process\n");
		// printf("\n***** CPU CONTEXT *****\n");
		// printf("x19 = %d\n", current->cpu_context.x19);
		// printf("x20 = %d\n", current->cpu_context.x20);
		// printf("x21 = %d\n", current->cpu_context.x21);
		// printf("x22 = %d\n", current->cpu_context.x22);
		// printf("x23 = %d\n", current->cpu_context.x23);
		// printf("x24 = %d\n", current->cpu_context.x24);
		// printf("x25 = %d\n", current->cpu_context.x25);
		// printf("x26 = %d\n", current->cpu_context.x26);
		// printf("x27 = %d\n", current->cpu_context.x27);
		// printf("x28 = %d\n", current->cpu_context.x28);
		// printf("fp  = %d\n", current->cpu_context.fp);
		// printf("sp  = %d\n", current->cpu_context.sp);
		// printf("pc  = %d\n", current->cpu_context.pc);
		// printf("State         = %d\n", current->state);
		// printf("Counter       = %d\n", current->counter);
		// printf("Priority      = %d\n", current->priority);
		// printf("Preempt Count = %d\n", current->preempt_count);
		// printf("***********************\n");

		printf("\n*********** CPU CONTEXT ***********\n");
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
		printf("***********************************\n");

		for (int i = 0; i < 9; i++){
			// printf("\naddress of i: %x and letter is: %c\n\n", &i, array[i]);
			uart_send(array[i]);
			delay(CHAR_DELAY);
		} 
		printf("\n");
		schedule(); // yield
		// printf("\n++++++++++++++++++++++++++++++++++++++++++> exiting process\n");
	}

	// For now, all the tasks run in an infinite loop and never returns. 
	// We will handle task termination in future experiments
}

void kernel_main(void)
{
	

	uart_init();
	init_printf(0, putc);

	
	printf("\n*********** CPU CONTEXT ***********\n");
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
    printf("x20           = 0x%x | value: %d\n", &(current->cpu_context.x20), current->cpu_context.x20);
    printf("x19           = 0x%x | value: %d\n", &(current->cpu_context.x19), current->cpu_context.x19);
	printf("current-------> 0x%x\n", current);
	printf("***********************************\n");

	printf("KERNEL BOOTING UP....\r\n");
	printf("\n");
	printf("\n");	

	// Below, irq is off by default, b/c it is not needed for cooperative scheduling as
	// in the proj description. but to implement things like sleep() it will be needed. 
	// if so, enable irq and take care of irq handling
	irq_vector_init();
	generic_timer_init();
	enable_interrupt_controller();
	disable_irq();			

	printf("Creating Tasks....\n");

	// printf("kernel.c\t-> creating process with value '12345' and '%x' is the address of passed process\n", process);
	int res = copy_process((unsigned long)&process, (unsigned long)"123456789");
	
	// printf("value of process (in kernel.c): %c\n", arg);
	if (res != 0) {
		printf("error while starting process 1");
		return;
	}
	printf("\n*********** CPU CONTEXT (after task_1) ***********\n");
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
	printf("\n");
	// printf("kernel.c\t-> creating process with value 'abcde' and '%x' is the address of passed process\n", process);	
	res = copy_process((unsigned long)&process, (unsigned long)"abcdefghi");
	if (res != 0) {
		printf("error while starting process 2");
		return;
	}
	printf("\n*********** CPU CONTEXT (after task_2) ***********\n");
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

	printf("\n");
	printf("\n");

	printf("Tasks are created and contexts for both tasks are set. Its time to schedule: \n");

	while (1) {
		schedule();
	}	
}
