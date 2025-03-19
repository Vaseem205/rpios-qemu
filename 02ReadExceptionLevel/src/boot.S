#include "arm/sysregs.h"
#include "mm.h"

.section ".text.boot"
.globl _start
_start:
    mrs x0, mpidr_el1       // Get processor ID
    and x0, x0, #0xFF
    cbz x0, master          // If CPU 0, proceed
    b proc_hang             // Else, hang

proc_hang:
    b proc_hang

master:

	/*
	Reserved Bits: 	Bits (28–29, 22–23, 20, 11) must be set to specific values as required by the AArch64 specification.
	Endianness: 	Configured for little-endian operation.
	Caches & MMU: 	Both instruction and data caches are disabled, and the MMU is turned off. 
					This is common in early boot to avoid complexities with virtual memory until the OS reconfigures them.
	*/
    ldr x0, =SCTLR_VALUE_MMU_DISABLED
    msr sctlr_el1, x0 

	/*
	Setting this bit forces EL1 to use the AArch64 execution state (instead of AArch32).
	INFO:The Execution state for EL0 is determined by the current value of PSTATE.nRW when executing at EL0.
	*/
    ldr x0, =HCR_VALUE
    msr hcr_el2, x0         // Hypervisor configuration

#ifdef USE_QEMU
    ldr x0, =SPSR_VALUE
    msr spsr_el2, x0        // Set up for EL1
    adr x0, el1_entry		// Load the address of el1_entry in x0
    msr elr_el2, x0
#else
    ldr x0, =SCR_VALUE
    msr scr_el3, x0			// Setting the NS (Non-Secure) bit to ensure the kernel runs in the non-secure world.
    ldr x0, =SPSR_VALUE
    msr spsr_el3, x0
    adr x0, el1_entry_another
    msr elr_el3, x0
#endif
    eret                    // Switch to EL1 because in SPSR_VALUE has set the [3:0] bit to point to EL1_SP

el1_entry_another:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero

el1_entry:
    mov sp, #LOW_MEMORY
    bl kernel_main
    b proc_hang
