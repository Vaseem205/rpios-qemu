#include "utils.h"
#include "printf.h"
#include "peripherals/timer.h"
#include "timer.h"


/*
xzl: around 1 sec (because we know that CPU is ticking 62.% Mil times per second). So we are trying to start from the number closer to
62.5 Mil. that'll be 1<<26.

So if we start from 67,108,864 (1<<27) down to zero, the entire cycle will take time of around 1 sec
*/ 

int interval = (1 << 26); 


/* 	These are for Arm generic timers. 
	They are fully functional on both QEMU and Rpi3. 
	Recommended.
*/

/* 
CNTFRQ_EL0 reports the frequency of the system count 
NOTE: 
CNTFRQ_EL0 reports the frequency of the system count. 
However, this register is not populated by hardware. 
The register is write-able at the highest implemented Exception level and readable at all Exception levels. 
Firmware, typically running at EL3, populates this register as part of early system initialization. 
Higher-level software, like an operating system, can then use the register to get the frequency.
*/
static unsigned int read_cntfrq(void)
{
	unsigned int val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val));	// the value of cntfrq_el0 (62500000) will be stored in val
  	return val;											// return val
}

void generic_timer_init ( void )
{
	unsigned int freq  = read_cntfrq();
	printf("System count freq (CNTFRQ) is: %u\n", freq);

	printf("interval is set to: %d\n", interval);
	gen_timer_init();
	gen_timer_reset(interval);
}

void handle_generic_timer_irq( void ) 
{
	printf("Timer interrupt received. next in %d ticks\n\r", interval);
	gen_timer_reset(interval);
}

