# ARM Generic Timer

The ARM generic timer is a hardware feature in ARM processors designed to provide a consistent timekeeping mechanism across the system. It consists of a **system counter**, a global clock that ticks at a fixed frequency, and **per-core timer** that leverage this system-counter to schedule interrupts.

## System Counter

- **Purpose**: A global clock that provides a steady timebase for the entire system.
- **Frequency**: In this project, it ticks at **62.5 MHz** (62,500,000 ticks per second).
- **Register**: The frequency is stored in the `CNTFRQ_EL0` register, which the code reads to determine the tick rate.
- **Note**: This frequency is fixed and set by firmware at boot time; the code does not configure it, only reads it.

## Per-Core Timer

- **Description**: Each CPU core has its own **physical timer** that operates using the system counter.
- **Functionality**: The timer can be programmed to count down from a specified number of ticks down to zero. And when it reaches zero, it triggers an interrupt.
- **Usage**: In this project, we use the EL1 physical timer to generate periodic interrupts on core 0.

## How the Code Sets This Up

The code configures the ARM generic timer in three main steps: timer initialization, interrupt setup, and interrupt handling.

### 1. Timer Initialization (`generic_timer_init`)

This function configures the EL1 physical timer:

- **Read the Frequency**:
  - `read_cntfrq()` reads the `CNTFRQ_EL0` register to retrieve the system counter's frequency (e.g., 62.5 MHz).
  - This informs the code how many ticks occur per second.

- **Enable the Timer**:
  - `gen_timer_init()` writes `1` to the `CNTP_CTL_EL0` register, the control register for the EL1 physical timer.
  - Bit 0 is the **ENABLE** bit, so setting it to 1 activates the timer.

- **Set the Interval**:
  - `gen_timer_reset(interval)` writes the interval value (`1 << 26 = 67,108,864` ticks) to the `CNTP_TVAL_EL0` register, the Timer Value register.
  - This defines how many ticks the timer counts down before triggering an interrupt.
  - **Timing Calculation**:
    - With a system counter frequency of 62.5 MHz (62,500,000 ticks/second), `67,108,864 ticks ÷ 62,500,000 ticks/second ≈ 1.07 seconds`.
    - The code aims for an interval of "around 1 second," and this value aligns closely.
  - **Operation**: At each tick of the system counter, the `CNTP_TVAL_EL0` value decrements by 1. When it reaches 0, the timer triggers an interrupt specific to its core.

### 2. Interrupt Setup (`kernel_main`)

This step prepares the system to handle timer interrupts:

- **Vector Table**:
  - `irq_vector_init()` sets the `vbar_el1` register to point to the exception vector table (defined in `entry.S`).
  - This table contains the addresses of interrupt handlers, including the timer interrupt handler.

- **Enable Timer Interrupts**:
  - `enable_interrupt_controller()` writes to `TIMER_INT_CTRL_0`, a Raspberry Pi-specific register for core 0.
  - This enables the EL1 physical timer interrupt, allowing it to reach the processor.

- **Enable IRQs**:
  - `enable_irq()` clears the **I bit** in the DAIF mask using the instruction `msr daifclr, #2`.
  - This allows the processor to respond to interrupt requests (IRQs).

### 3. Interrupt Handling

When the timer counts down to zero, the following occurs:

- **Interrupt Generation**: The EL1 physical timer generates an interrupt.
- **Vector Table Lookup**: The processor jumps to the IRQ handler in the exception vector table, which invokes `handle_irq()`.
- **Identify Source**:
  - `handle_irq()` reads the `INT_SOURCE_0` register to determine the interrupt source.
  - If the source matches `GENERIC_TIMER_INTERRUPT`, it indicates a timer interrupt.
- **Handle Timer Interrupt**:
  - `handle_generic_timer_irq()` is called, which:
    - Prints `"Timer interrupt received..."` to indicate the interrupt was processed.
    - Resets the timer by calling `gen_timer_reset(interval)` to restart the countdown.

## Overall Flow

- **System Counter**: Ticks continuously at 62.5 MHz, providing a consistent timebase.
- **EL1 Timer**: Counts down from 67,108,864 ticks (approximately 1.07 seconds), triggers an interrupt when it reaches zero, and resets to repeat the cycle.
- **Interrupt Controller**: Routes the timer interrupt to core 0.
- **Handler**: Processes the interrupt by printing a message and restarting the timer.

This setup results in periodic interrupts roughly every second, demonstrating the ARM generic timer's use in scheduling tasks within the project.
