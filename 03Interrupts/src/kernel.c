#include "printf.h"
#include "timer.h"
#include "irq.h"
#include "mini_uart.h"
#include "utils.h"


void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);
	printf("kernel boots...\n");

	irq_vector_init();						// loading "vectors (entry.S)" address in vbar_el1
	generic_timer_init();					// read freq, init timer, reset timer
	enable_interrupt_controller();			// Enables Core 0 Timers interrupt control for the generic timer
	enable_irq();							// enables IRQs by doing -> msr daifclr, #2

	// FL (2/1/23): the following are some ideas for triggering exceptions. But do they work? 
	// Validate your guess by searching into aarch64 manuals and/or debugging. 
	// Or come up with your own method...

	// a = a / 0; 
	// asm("mrs x0, elr_el2"); // will trigger exception at EL1
	// asm("hvc #0");
	// asm("msr	hcr_el2, x0");

	//printf("going to call wfi...");
	//asm("wfi");
	//printf("we're back!");

	while (1) {
		uart_send(uart_recv());
	}	
}
