## Understanding ARMv8 Exception Levels

The ARMv8 architecture defines four exception levels (EL0 to EL3), each with increasing privileges:
- **EL0**: User mode for applications, with limited access to registers and memory.
- **EL1**: Kernel mode, suitable for operating systems, with access to system control registers.
- **EL2**: Hypervisor mode, used for virtual machine management.
- **EL3**: Secure monitor mode, for handling secure and non-secure world transitions (TrustZone).

Exception levels enforce isolation. For example, an EL0 application can’t access kernel memory or change the exception level directly. Transitions occur via exceptions—interrupts, system calls, or faults—managed by higher-level code. In our case, the Raspberry Pi boots in EL3 (or EL2 in QEMU), and we’ll manually switch to EL1 to run our kernel.

## The Boot Process

The boot process is handled in `boot.S`, an assembly file that initializes the processor and prepares it for C code execution. Let’s break it down.

### Processor ID Check

```assembly
mrs x0, mpidr_el1       // Get processor ID
and x0, x0, #0xFF
cbz x0, master          // If CPU 0, proceed
b proc_hang             // Else, hang

proc_hang:
    b proc_hang
```

The Raspberry Pi has multiple cores, but we only want the primary core (CPU 0) to execute our kernel. The `mpidr_el1` register provides the processor ID, and we check if it’s zero. Non-primary cores loop indefinitely in `proc_hang`.

### Configuring System Registers

Before switching to EL1, we configure several system registers to set up the processor state. These steps vary slightly depending on whether we’re running on QEMU (EL2 start) or real hardware (EL3 start).

#### SCTLR_EL1: System Control Register (EL1)

```assembly
ldr x0, =SCTLR_VALUE_MMU_DISABLED
msr sctlr_el1, x0
```

Defined in `sysregs.h`:

```c
#define SCTLR_RESERVED          (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN  (0 << 25)
#define SCTLR_I_CACHE_DISABLED  (0 << 12)
#define SCTLR_D_CACHE_DISABLED  (0 << 2)
#define SCTLR_MMU_DISABLED      (0 << 0)
#define SCTLR_VALUE_MMU_DISABLED (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)
```

- **Purpose**: Configures EL1 behavior.
- **Settings**: Disables the MMU and caches (for simplicity during boot), sets little-endian mode, and preserves reserved bits as per the ARMv8 spec.

#### HCR_EL2: Hypervisor Configuration Register (EL2)

```assembly
ldr x0, =HCR_VALUE
msr hcr_el2, x0
```

```c
#define HCR_RW      (1 << 31)
#define HCR_VALUE   HCR_RW
```

- **Purpose**: Ensures EL1 runs in AArch64 mode (not AArch32).
- **Context**: Required even without a hypervisor, as it affects lower levels.

#### SCR_EL3: Secure Configuration Register (EL3) (Real Hardware Only)

```assembly
ldr x0, =SCR_VALUE
msr scr_el3, x0
```

```c
#define SCR_RESERVED    (3 << 4)
#define SCR_RW          (1 << 10)
#define SCR_NS          (1 << 0)
#define SCR_VALUE       (SCR_RESERVED | SCR_RW | SCR_NS)
```

- **Purpose**: Sets EL2 to AArch64 and marks lower levels as non-secure (no TrustZone).
- **Note**: Skipped in QEMU, which starts at EL2.

#### SPSR_EL3 or SPSR_EL2: Saved Program Status Register

```assembly
#ifdef USE_QEMU
    ldr x0, =SPSR_VALUE
    msr spsr_el2, x0
#else
    ldr x0, =SPSR_VALUE
    msr spsr_el3, x0
#endif
```

```c
#define SPSR_MASK_ALL   (7 << 6)  // Mask SError, IRQ, FIQ
#define SPSR_EL1h       (5 << 0)  // Target EL1 with its own stack
#define SPSR_VALUE      (SPSR_MASK_ALL | SPSR_EL1h)
```

- **Purpose**: Defines the state restored by `eret`.
- **Settings**: Masks interrupts (to avoid crashes before handlers are set) and targets EL1 with a dedicated stack pointer.

#### ELR_EL3 or ELR_EL2: Exception Link Register

```assembly
#ifdef USE_QEMU
    adr x0, el1_entry
    msr elr_el2, x0
#else
    adr x0, el1_entry_another
    msr elr_el3, x0
#endif
```

- **Purpose**: Sets the address to jump to after `eret`.
- **Target**: `el1_entry` (QEMU) or `el1_entry_another` (real hardware).

#### Switching to EL1

```assembly
eret
```

The `eret` instruction uses `SPSR_ELx` and `ELR_ELx` to switch to EL1, jumping to the specified entry point.

## Setting Up the Environment in EL1

### Zeroing the BSS Section (Real Hardware)

```assembly
el1_entry_another:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero
```

```assembly
memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt memzero
    ret
```

- **Purpose**: Clears the BSS section (uninitialized data) to zero, a standard C runtime requirement.
- **How**: Loops over the memory range defined in the linker script, storing zeros.

### Setting the Stack and Jumping to C

```assembly
el1_entry:
    mov sp, #LOW_MEMORY
    bl kernel_main
    b proc_hang
```

- **Stack**: Sets the stack pointer to `LOW_MEMORY` (defined in `mm.h`), providing a stack for C code.
- **Jump**: Calls `kernel_main`, the C entry point. If it returns, the core hangs.

## Implementing UART Communication

The mini UART provides serial output on the Raspberry Pi. The code in `mini_uart.c` handles this:

### uart_init

```c
void uart_init(void) {
    unsigned int selector;
    selector = get32(GPFSEL1);
    selector &= ~(7<<12);  // Clear GPIO14
    selector |= 2<<12;     // Set ALT5 for GPIO14 (TXD1)
    selector &= ~(7<<15);  // Clear GPIO15
    selector |= 2<<15;     // Set ALT5 for GPIO15 (RXD1)
    put32(GPFSEL1, selector);

    put32(GPPUD, 0);
    delay(150);
    put32(GPPUDCLK0, (1<<14)|(1<<15));
    delay(150);
    put32(GPPUDCLK0, 0);

    put32(AUX_ENABLES, 1);      // Enable mini UART
    put32(AUX_MU_CNTL_REG, 0);  // Disable TX/RX during config
    put32(AUX_MU_IER_REG, 0);   // Disable interrupts
    put32(AUX_MU_LCR_REG, 3);   // 8-bit mode
    put32(AUX_MU_MCR_REG, 0);   // RTS high
    put32(AUX_MU_BAUD_REG, 270); // 115200 baud
    put32(AUX_MU_CNTL_REG, 3);  // Enable TX/RX
}
```

- **GPIO Setup**: Configures GPIO14 (TX) and GPIO15 (RX) for UART alternate function 5.
- **Pull Resistors**: Disables pull-up/down resistors.
- **UART Config**: Enables the mini UART, sets 8-bit mode, and configures a 115200 baud rate.

### uart_send and uart_recv

```c
void uart_send(char c) {
    while (!(get32(AUX_MU_LSR_REG) & 0x20)) {}
    put32(AUX_MU_IO_REG, c);
}

char uart_recv(void) {
    while (!(get32(AUX_MU_LSR_REG) & 0x01)) {}
    return get32(AUX_MU_IO_REG) & 0xFF;
}
```

- **Send**: Waits until the transmit buffer is ready, then sends a character.
- **Receive**: Waits for data, then reads a character.

### uart_send_string

```c
void uart_send_string(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        uart_send(str[i]);
    }
}
```

- **Purpose**: Sends a null-terminated string character by character.

## Integrating a Lightweight printf

The `printf.c` file provides a compact `printf` implementation, supporting `%d`, `%u`, `%x`, `%s`, and `%c`. It relies on a `putc` callback:

```c
void putc(void* p, char c) {
    uart_send(c);
}
```

In `kernel_main`:

```c
init_printf(0, putc);
```

- **Setup**: Initializes `printf` to use `putc`, linking it to the UART.

## The Kernel Main Function

```c
void kernel_main(void) {
    uart_init();
    init_printf(0, putc);
    int el = get_el();
    printf("Exception level: %d \r\n", el);
    while (1) {
        uart_send(uart_recv());
    }
}
```

- **Flow**:
  1. Initializes the UART.
  2. Sets up `printf`.
  3. Retrieves the current exception level with `get_el`:
     ```assembly
     get_el:
         mrs x0, CurrentEL
         lsr x0, x0, #2
         ret
     ```
     (`CurrentEL` holds the level in bits 3:2, shifted right to get an integer.)
  4. Prints the level (should be 1).
  5. Echoes received characters indefinitely.

## Building the Kernel

The `Makefile` compiles and links the project:

```makefile
ARMGNU ?= aarch64-linux-gnu
COPS = -Wall -Werror -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -g -O0 -DUSE_QEMU
ASMOPS = -Iinclude -g -DUSE_QEMU

all: kernel8.img

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o) $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

kernel8.img: $(SRC_DIR)/linker-qemu.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker-qemu.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img
```

- **Linker Script** (`linker-qemu.ld`):
  ```ld
  SECTIONS {
      . = 0x80000;
      .text.boot : { *(.text.boot) }
      .text : { *(.text) }
      .rodata : { *(.rodata) }
      .data : { *(.data) }
      . = ALIGN(0x8);
      bss_begin = .;
      .bss : { *(.bss*) }
      bss_end = .;
  }
  ```
  - Starts at 0x80000, organizes code and data sections, and defines BSS boundaries.

- **Build Command**: `make`

## Running the Kernel

### On QEMU

```sh
qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial null -serial stdio
```

- **Output**: 
  ```
  Exception level: 1
  ```
  Then echoes typed characters.
