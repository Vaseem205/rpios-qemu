#include "entry.h"

/*
 * Exception Handling in ARM64 Kernel
 *
 * This file defines macros and exception vector tables for handling exceptions 
 * in the kernel. Each exception vector jumps to a default handler that saves 
 * the processor state, logs the error, and halts the system.
 *
 * The ARM64 architecture mandates that each exception vector occupies a maximum 
 * of 0x80 bytes (achieved using .align 7 in assembly).
 */

/*
 * Macro: handle_invalid_entry
 * -----------------------------
 * Default exception handler that:
 *  - Sets the exception type in x0.
 *  - Loads the Exception Syndrome Register (ESR) into x1.
 *  - Loads the Exception Link Register (ELR) into x2.
 *  - Calls a function to display the exception message.
 *  - Branches to err_hang to stop further execution.
 *
 * Arguments:
 *  - type: Exception type defined in entry.h.
 */
.macro handle_invalid_entry type
    kernel_entry             			// Save processor state (x0-x30)
    mov   x0, #\type         			// Load exception type into x0
    mrs   x1, esr_el1        			// Load Exception Syndrome Register into x1
    mrs   x2, elr_el1        			// Load Exception Link Register into x2
    bl    show_invalid_entry_message 	// Call function to display error message
    b     err_hang           			// Halt the system
.endm

/*
 * Macro: ventry
 * -------------
 * Aligns the exception vector to the required boundary and branches to the specified label.
 *
 * Arguments:
 *  - label: The target label to branch to.
 */
.macro ventry label
    .align 7                // Align to 0x80 bytes boundary for exception vectors
    b      \label           // Branch to the provided label
.endm


/*

Lets suppose SP = 1000
after "sub   sp, sp, #S_FRAME_SIZE" SP = 744

Keeping this SP (744) as base, we have offset which is multiple of 16bytes (for 2 register worth 8 bytes)
So SP is unchanged, while storing the value on stack, which helps it while restoring as SP would still be pointint to 744.
Its just that we add offset to SP (i.e. multiple of 16)

*/

/*
 * Macro: kernel_entry
 * -------------------
 * Saves the CPU state by subtracting the size of the register frame from the stack pointer
 * and storing registers x0 to x30 into memory.
 */
.macro kernel_entry
    sub   sp, sp, #S_FRAME_SIZE    	// The instruction subtracts 256 from the current stack pointer and make up 256 bytes on the stack where the register values will be stored.
    stp   x0,  x1,  [sp, #16 * 0]	// stp stores two registers (each register is 8 bytes on a 64-bit architecture) and uses an offset of #16 * n.
    stp   x2,  x3,  [sp, #16 * 1]
    stp   x4,  x5,  [sp, #16 * 2]
    stp   x6,  x7,  [sp, #16 * 3]
    stp   x8,  x9,  [sp, #16 * 4]
    stp   x10, x11, [sp, #16 * 5]
    stp   x12, x13, [sp, #16 * 6]
    stp   x14, x15, [sp, #16 * 7]
    stp   x16, x17, [sp, #16 * 8]
    stp   x18, x19, [sp, #16 * 9]
    stp   x20, x21, [sp, #16 * 10]
    stp   x22, x23, [sp, #16 * 11]
    stp   x24, x25, [sp, #16 * 12]
    stp   x26, x27, [sp, #16 * 13]
    stp   x28, x29, [sp, #16 * 14]
    str   x30,      [sp, #16 * 15]
.endm



/*
 * Macro: kernel_exit
 * ------------------
 * Restores the CPU state from the stack and returns from the exception using the ERET instruction.
 */
.macro kernel_exit
    ldp   x0,  x1,  [sp, #16 * 0]
    ldp   x2,  x3,  [sp, #16 * 1]
    ldp   x4,  x5,  [sp, #16 * 2]
    ldp   x6,  x7,  [sp, #16 * 3]
    ldp   x8,  x9,  [sp, #16 * 4]
    ldp   x10, x11, [sp, #16 * 5]
    ldp   x12, x13, [sp, #16 * 6]
    ldp   x14, x15, [sp, #16 * 7]
    ldp   x16, x17, [sp, #16 * 8]
    ldp   x18, x19, [sp, #16 * 9]
    ldp   x20, x21, [sp, #16 * 10]
    ldp   x22, x23, [sp, #16 * 11]
    ldp   x24, x25, [sp, #16 * 12]
    ldp   x26, x27, [sp, #16 * 13]
    ldp   x28, x29, [sp, #16 * 14]
    ldr   x30,      [sp, #16 * 15]
    add   sp, sp, #S_FRAME_SIZE    // Restore the stack pointer
    eret                          // Return from exception
.endm

/*
 * Exception Vector Table for EL1
 *
 * This table defines the branch instructions for various exception types
 * for EL1. Exceptions are categorized based on the execution state and mode:
 *  - EL1t: EL1 exceptions when SP is shared with EL0.
 *  - EL1h: EL1 exceptions with a dedicated SP.
 *  - EL0_64: Exceptions from 64-bit user programs.
 *  - EL0_32: Exceptions from 32-bit user programs.
 */
.align 11
.globl vectors
vectors:
    // EL1t exception vectors
    ventry sync_invalid_el1t     // Synchronous exception in EL1t
    ventry irq_invalid_el1t      // IRQ exception in EL1t
    ventry fiq_invalid_el1t      // FIQ exception in EL1t
    ventry error_invalid_el1t    // Error exception in EL1t

    // EL1h exception vectors
    ventry sync_invalid_el1h     // Synchronous exception in EL1h
    ventry el1_irq               // IRQ exception in EL1h
    ventry fiq_invalid_el1h      // FIQ exception in EL1h
    ventry error_invalid_el1h    // Error exception in EL1h

    // EL0 exception vectors for 64-bit mode
    ventry sync_invalid_el0_64   // Synchronous exception in 64-bit EL0
    ventry irq_invalid_el0_64    // IRQ exception in 64-bit EL0
    ventry fiq_invalid_el0_64    // FIQ exception in 64-bit EL0
    ventry error_invalid_el0_64  // Error exception in 64-bit EL0

    // EL0 exception vectors for 32-bit mode
    ventry sync_invalid_el0_32   // Synchronous exception in 32-bit EL0
    ventry irq_invalid_el0_32    // IRQ exception in 32-bit EL0
    ventry fiq_invalid_el0_32    // FIQ exception in 32-bit EL0
    ventry error_invalid_el0_32  // Error exception in 32-bit EL0

// Exception Handlers for EL1 and EL0 exceptions

sync_invalid_el1t:
    handle_invalid_entry SYNC_INVALID_EL1t

irq_invalid_el1t:
    handle_invalid_entry IRQ_INVALID_EL1t

fiq_invalid_el1t:
    handle_invalid_entry FIQ_INVALID_EL1t

error_invalid_el1t:
    handle_invalid_entry ERROR_INVALID_EL1t

sync_invalid_el1h:
    handle_invalid_entry SYNC_INVALID_EL1h

fiq_invalid_el1h:
    handle_invalid_entry FIQ_INVALID_EL1h

error_invalid_el1h:
    handle_invalid_entry ERROR_INVALID_EL1h

sync_invalid_el0_64:
    handle_invalid_entry SYNC_INVALID_EL0_64

irq_invalid_el0_64:
    handle_invalid_entry IRQ_INVALID_EL0_64

fiq_invalid_el0_64:
    handle_invalid_entry FIQ_INVALID_EL0_64

error_invalid_el0_64:
    handle_invalid_entry ERROR_INVALID_EL0_64

sync_invalid_el0_32:
    handle_invalid_entry SYNC_INVALID_EL0_32

irq_invalid_el0_32:
    handle_invalid_entry IRQ_INVALID_EL0_32

fiq_invalid_el0_32:
    handle_invalid_entry FIQ_INVALID_EL0_32

error_invalid_el0_32:
    handle_invalid_entry ERROR_INVALID_EL0_32


/*
 * EL1 IRQ Handler
 *
 * This handler is invoked for EL1 IRQ exceptions.
 * It saves the processor state, calls the IRQ handling routine, and then
 * restores the state before returning.
 */
el1_irq:
    kernel_entry
    bl handle_irq   // Call the IRQ handler function (handle_irq in irq.c) 
	# bl error_invalid_el0_32
    kernel_exit

/*
 * Infinite Loop for Unhandled Exceptions
 *
 * This loop halts the system in case of an unrecoverable exception.
 */
.globl err_hang
err_hang:
    b err_hang

/*
 * Try-to-Return Routine
 *
 * This routine attempts to resume execution by:
 *  - Saving the processor state.
 *  - Incrementing the exception return address (ELR) by 4 to skip the problematic instruction.
 *  - Restoring the state and returning from the exception.
 */
try_to_return:
    kernel_entry
    mrs  x1, elr_el1   // Load current exception return address
    add  x1, x1, 4     // Increment address to skip the faulting instruction
    msr  elr_el1, x1   // Store the new return address
    kernel_exit
