/*
The memzero function is designed to clear the BSS section by starting at the address in x0 (which is the beginning of the BSS)
and writing zeros in 64-bit (8-byte) blocks. The register x1 holds the total size (in bytes) of the BSS section that needs to
be cleared. Each iteration of the loop writes an 8-byte zero and decreases x1 by 8 until the entire region is zeroed.
*/

.globl memzero
memzero:
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero
	ret
