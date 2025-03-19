#ifndef _SYSREGS_H
#define _SYSREGS_H

// ***************************************
// SCTLR_EL1, System Control Register (EL1), Page 2654 of AArch64-Reference-Manual.
// ***************************************
#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN         (0 << 24)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)
#define SCTLR_MMU_ENABLED               (1 << 0)
#define SCTLR_VALUE_MMU_DISABLED	    (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)


// ***************************************
// HCR_EL2, Hypervisor Configuration Register (EL2), Page 2487 of AArch64-Reference-Manual.
// ***************************************
#define HCR_RW	    		(1 << 31)
#define HCR_VALUE			HCR_RW


// ***************************************
// SCR_EL3, Secure Configuration Register (EL3), Page 2648 of AArch64-Reference-Manual.
// ***************************************
#define SCR_RESERVED	    (3 << 4)
#define SCR_RW				(1 << 10)
#define SCR_NS				(1 << 0)
#define SCR_VALUE	    	(SCR_RESERVED | SCR_RW | SCR_NS)


// ***************************************
// SPSR_EL3, Saved Program Status Register (EL3) Page 389 of AArch64-Reference-Manual.
// ***************************************

/*
In SPSR_VALUE (SPSR_MASK_ALL = 7 << 6), bits 8:6 are set to 1, masking (disabling) SError (system errors), IRQ (interrupts), and FIQ (fast interrupts).
When eret restores the state from spsr_el2 or spsr_el3, EL1 starts with these exceptions disabled.
The exception masking is about timing, because:
->  Interrupt handlers aren’t set up.
->  The exception vector table isn’t initialized.
->  The system is in a fragile state, configuring basic hardware. 
    If an interrupt or error occurred, the processor might jump to an undefined handler, crashing the boot process. 
    Masking these exceptions ensures uninterrupted execution until kernel_main sets up proper interrupt handling and possibly enables them later.
*/
#define SPSR_MASK_ALL 		(7 << 6)
/*
When an exception occurs, the processor saves the current state in SPSR_EL2, and when returning from the exception (eret), this saved state is restored.
The saved exception state includes information about: Which exception level to return to and which stack pointer to use
*/
#define SPSR_EL1h			(5 << 0)

#define SPSR_VALUE			(SPSR_MASK_ALL | SPSR_EL1h)

#endif
