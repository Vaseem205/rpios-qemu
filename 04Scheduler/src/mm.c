#include "mm.h"

/*

mem_map[PAGING_PAGES]: To track which pages are free or allocated.
- 0: The page is free and available for allocation.
- 1: The page is allocated and in use.

NOTE: PAGING_PAGES = 257,024 (these many 4KB pages can be created)

So, mem_map[257,024] = {1,1,1,1,1,0,0,....}

*/

static unsigned short mem_map[PAGING_PAGES] = {0,}; //Initialization: All entries start as 0, meaning all pages are initially free.


/*

- Loops through mem_map from index 0 to PAGING_PAGES - 1.
- Checks each entry for a 0 (free page).
- When a free page is found:
	- Sets mem_map[i] = 1 to mark it as allocated.
	- Returns the page’s physical address: LOW_MEMORY + i * PAGE_SIZE.
- If no free pages are found, returns 0 to indicate memory exhaustion.

LOW_MEMORY = 4,194,304 -> 4MB

Example: 

LOW_MEMORY = 0x400000 (4 MB) and PAGE_SIZE = 4096:
- First page: 0x400000.
- Second page: 0x401000.
- Third page: 0x402000, and so on.

*/
unsigned long get_free_page(){

	for (int i = 0; i < PAGING_PAGES; i++){
		if (mem_map[i] == 0)
		{
			mem_map[i] = 1;
			return LOW_MEMORY + i*PAGE_SIZE;
		}
	}
	return 0;
}

/*

How It Works:
- Takes the page address p as input.
- Calculates the page’s index in mem_map: (p - LOW_MEMORY) / PAGE_SIZE.
- Sets mem_map[index] = 0 to mark the page as free.

Example:
- If p = 0x401000 and LOW_MEMORY = 0x400000:
- Index = (0x401000 - 0x400000) / 0x1000 = 1
- Sets mem_map[1] = 0, freeing the second page.

*/

void free_page(unsigned long p){
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}

