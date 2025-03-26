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
		for (int i = 0; i < 5; i++){
			uart_send(array[i]);
			delay(CHAR_DELAY);
		} 
		printf("|");
		schedule(); // yield
	}

	// For now, all the tasks run in an infinite loop and never returns. 
	// We will handle task termination in future experiments
}

void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);

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

	printf("Creating Tasks:\n");

	printf("kernel.c\t-> creating process with value '12345' and '%x' is the address of passed process\n", process);
	int res = copy_process((unsigned long)&process, (unsigned long)"12345");
	
	// printf("value of process (in kernel.c): %c\n", arg);
	if (res != 0) {
		printf("error while starting process 1");
		return;
	}
	printf("\n");
	printf("kernel.c\t-> creating process with value 'abcde' and '%x' is the address of passed process\n", process);	
	res = copy_process((unsigned long)&process, (unsigned long)"abcde");
	if (res != 0) {
		printf("error while starting process 2");
		return;
	}

	printf("\n");
	printf("\n");

	printf("Tasks are created and contexts for both tasks are set. Its time to schedule: \n");

	while (1) {
		schedule();
	}	
}
