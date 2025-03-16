## The Build Process

The `Makefile` orchestrates the compilation and linking:

```makefile
ARMGNU ?= aarch64-linux-gnu

COPS = -Wall -Werror -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -g -O0 -DUSE_QEMU
ASMOPS = -Iinclude -g -DUSE_QEMU

BUILD_DIR = build
SRC_DIR = src

all: kernel8.img

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	$(ARMGNU)-gcc $(COPS) -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -c $< -o $@

kernel8.img: $(SRC_DIR)/linker-qemu.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker-qemu.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img
```

- **Toolchain**: Uses `aarch64-linux-gnu` for 64-bit ARM.
- **Flags**: `-nostdlib`, `-nostartfiles`, and `-ffreestanding` ensure no OS dependencies.
- **Output**: `kernel8.img`, a binary image loadable by QEMU or a real Raspberry Pi.

Run `make` to build, and `make clean` to start fresh.

## Booting Up

The kernel begins in `boot.S`, written in ARM assembly:

```assembly
.section ".text.boot"

.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    cbz x0, master
    b proc_hang

proc_hang:
    b proc_hang

master:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero

    mov sp, #LOW_MEMORY
    bl kernel_main
    b proc_hang
```

### Multi-Core Handling

The Raspberry Pi 3 has four cores, all starting simultaneously. We want only one (core 0) to run our kernel:

- `mrs x0, mpidr_el1`: Reads the core ID.
- `and x0, x0, #0xFF`: Extracts the core number (0–3).
- `cbz x0, master`: Core 0 proceeds to `master`; others loop in `proc_hang`.

### BSS Initialization

The BSS section holds uninitialized global variables, which must be zeroed:

- `adr x0, bss_begin` and `adr x1, bss_end`: Get the BSS boundaries from the linker script.
- `sub x1, x1, x0`: Calculate the size.
- `bl memzero`: Call the zeroing function.

The `memzero` function (in `utils.S`) clears memory in 8-byte chunks:

```assembly
.globl memzero
memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt memzero
    ret
```

### Stack Setup

- `mov sp, #LOW_MEMORY`: Sets the stack pointer to 4MB (defined in `mm.h` as `2 * SECTION_SIZE`, where `SECTION_SIZE` is 2MB).
- `bl kernel_main`: Jumps to the C code.

## The Linker Script

The `linker-qemu.ld` file defines the memory layout:

```ld
SECTIONS
{
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

- **Start Address**: `0x80000`, where QEMU loads the kernel.
- **Sections**: Organizes boot code, regular code, read-only data, data, and BSS.

## Kernel Logic

In `kernel.c`, the main functionality unfolds:

```c
#include "mini_uart.h"

void kernel_main(void)
{
    uart_init();
    uart_send_string("Hello, Wrold!\r\n");

    while (1) {
        uart_send(uart_recv());
    }
}
```

- `uart_init()`: Configures the mini UART.
- `uart_send_string("Hello, World!\r\n")`: Outputs the greeting.
- `while (1)`: Loops forever, echoing received characters.

## Mini UART Setup

The `mini_uart.c` file handles UART operations:

```c
void uart_init(void)
{
    unsigned int selector = get32(GPFSEL1);
    selector &= ~(7<<12); // Clear GPIO14
    selector |= 2<<12;    // Set ALT5 (TXD1)
    selector &= ~(7<<15); // Clear GPIO15
    selector |= 2<<15;    // Set ALT5 (RXD1)
    put32(GPFSEL1, selector);

    put32(GPPUD, 0);
    delay(150);
    put32(GPPUDCLK0, (1<<14)|(1<<15));
    delay(150);
    put32(GPPUDCLK0, 0);

    put32(AUX_ENABLES, 1);
    put32(AUX_MU_CNTL_REG, 0);
    put32(AUX_MU_IER_REG, 0);
    put32(AUX_MU_LCR_REG, 3);  // 8-bit mode
    put32(AUX_MU_MCR_REG, 0);
    put32(AUX_MU_BAUD_REG, 270); // 115200 baud
    put32(AUX_MU_CNTL_REG, 3);   // Enable TX and RX
}

void uart_send(char c)
{
    while (!(get32(AUX_MU_LSR_REG) & 0x20)) {}
    put32(AUX_MU_IO_REG, c);
}

char uart_recv(void)
{
    while (!(get32(AUX_MU_LSR_REG) & 0x01)) {}
    return get32(AUX_MU_IO_REG) & 0xFF;
}

void uart_send_string(char* str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        uart_send(str[i]);
    }
}
```

- **GPIO Setup**: Configures pins 14 and 15 for UART (ALT5 function).
- **UART Config**: Sets 8-bit mode, 115200 baud, and enables transmission/reception.
- **Send/Receive**: Polls status registers to send or receive one character at a time.

Utility functions in `utils.S` (`put32`, `get32`, `delay`) support these operations by accessing memory and introducing timing delays.

## Running the Kernel

1. **Build**: Run `make` to create `kernel8.img`.
2. **Emulate**: Use QEMU:
   ```bash
   qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial null -serial stdio
   ```
   - `-M raspi3`: Emulates Raspberry Pi 3.
   - `-serial stdio`: Connects UART to your terminal.

You’ll see "Hello, World!" printed, and typing characters will echo them back.

## Documentation

- [ARMv8-A developer's guide](https://developer.arm.com/documentation)
- [SoC Manual](https://github.com/raspberrypi/documentation/files/1888662/BCM2837-ARM-Peripherals.-.Revised.-.V2-1.pdf)
- [QEMU Documentation](https://www.qemu.org/docs/master/)

Happy coding, and enjoy the power of controlling the metal!