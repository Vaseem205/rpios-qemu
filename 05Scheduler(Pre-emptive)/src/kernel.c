#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"

void process(char *array)
{
	while (1) {
		for (int i = 0; i < 5; i++){
			uart_send(array[i]);
			delay(5000000);
		}
	}
}

void process2(char *array)
{
	while (1) {
		for (int i = 0; i < 5; i++){
			uart_send(array[i]);
			delay(5000000);
		}
	}
}

void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);

	printf("\n*********** INITIAL CPU CONTEXT ***********\n");
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
    printf("x20           = 0x%x | value: 0x%d\n", &(current->cpu_context.x20), current->cpu_context.x20);
    printf("x19           = 0x%x | value: 0x%d\n", &(current->cpu_context.x19), current->cpu_context.x19);
	printf("current-------> 0x%x\n", current);
	printf("***********************************\n");

	printf("kernel boots\n");

	irq_vector_init();
	generic_timer_init();
	enable_interrupt_controller();
	enable_irq();

	int res = copy_process((unsigned long)&process, (unsigned long)"12345");
	if (res != 0) {
		printf("error while starting process 1");
		return;
	}
	res = copy_process((unsigned long)&process2, (unsigned long)"abcde");
	if (res != 0) {
		printf("error while starting process 2");
		return;
	}

	while (1){
		schedule();
	}	
}
