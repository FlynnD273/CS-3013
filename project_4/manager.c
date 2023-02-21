#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MEM_SIZE 64
#define PAGE_SIZE 16
#define NUM_PROC 4

#define PERM_MASK   0b10000000
#define PERM_SHIFT  7
#define PRES_MASK   0b01000000
#define PRES_SHIFT  6
#define VPN_MASK    0b00001100
#define VPN_SHIFT   2
#define FRAME_MASK  0b00000011
#define FRAME_SHIFT 0

int table_loc[NUM_PROC]; // Index for the location of the page table for a given process ID
// Page table entries should be the page number then the frame number then the permission bits.
// Example:
// memory[0] = 2; // Page 2
// memory[1] = 3; // Maps to physical frame 3
// memory[2] = 1; // For read and write privilages
unsigned char memory[MEM_SIZE];
int currentframe = 0;

void printbinary(int value) {
	for (int j = 0; j < 8; j++) {
		int n = value >> 7 - j;
		printf("%i", n & 1);
	}
}

void dump () {
	for (int i = 0; i < NUM_PROC; i++) {
		printf("%i: %i\n", i, table_loc[i]);
	}
	printf("Memory:\n");
	for (int i = 0; i < MEM_SIZE; i++) {
		printf("%i: %i (", i, memory[i]);
		printbinary(memory[i]);
		printf(")\n");
	}
}

int gettablevalue (int value, int mask, int shift) {
	if (value == -1) return -1;

	return (value & mask) >> shift;
}

int maketableentry (int vpn, int frame, int perm) {
	int vpnbits = (vpn << VPN_SHIFT) & VPN_MASK;
	int framebits = (frame << FRAME_SHIFT) & FRAME_MASK;
	int permbits = (perm << PERM_SHIFT) & PERM_MASK;

	return vpnbits | framebits | permbits;
}

int loadpt (int pid) {
	// Load the page table for the given PID from the swap file, return the frame number that it was put in
	return -1;
}

int evictpage (int pid) {
	// Evict a page from memory. Skip currentframe if it's the page table for the given PID
	// Return the frame number
	return -1;
}

int loadpage (int pid, int vpn) {
	// Load page from disk. Skip currentframe if it's the page table for the given PID
	// Return the frame that it was put into
	return;
}

int gettableentry (int pid, int vadd) {
	if (table_loc[pid] == -1) {
		printf("Error: page table not initialized for PID %i\n", pid);
		return -1;
	}
	if (table_loc[pid] == -2) {
		table_loc[pid] = loadpt(pid);
	}

	for (int i = table_loc[pid]; i < table_loc[pid] + PAGE_SIZE; i++) {
		if (gettablevalue(memory[i], VPN_MASK, VPN_SHIFT)  == vadd / PAGE_SIZE) {
			return memory[i];
		}
	}

	printf("Error: no table entry found for virtual address %i\n", vadd);
	return -1;
}

int getpadd (int pid, int vadd) {
	int frame = gettablevalue(gettableentry(pid, vadd), FRAME_MASK, FRAME_SHIFT);
	if (frame == -1) return -1;

	return frame * PAGE_SIZE + vadd % PAGE_SIZE;
}

int initprocess (int pid) {
	if (table_loc[pid] != -1) return 0;

	int frame = currentframe;
	currentframe++;

	if (frame >= MEM_SIZE / PAGE_SIZE) {
		frame = evictpage(pid);
		printf("Ran out of memory.\n");
		return -1;
	}

	table_loc[pid] = frame;

	for (int i = frame * PAGE_SIZE; i < (frame + 1) * PAGE_SIZE; i++) {
		memory[i] = -1;
	}

	printf("Put page table for PID %i into physical frame %i\n", pid, frame);
}

int map (int pid, int vadd, int iswriteable) {
	initprocess(pid);

	int frame = currentframe;
	int vaddvpn = vadd / PAGE_SIZE;

	currentframe++;

	if (frame >= MEM_SIZE / PAGE_SIZE) {
		printf("Ran out of physical memory\n");
		return -1;
	}

	int tableindex = table_loc[pid] * PAGE_SIZE;
	while (memory[tableindex] != 255 && tableindex < table_loc[pid] + PAGE_SIZE) {
		int vpn = gettablevalue(memory[tableindex], VPN_MASK, VPN_SHIFT);

		if (vpn == vaddvpn) {
			frame = gettablevalue(memory[tableindex], FRAME_MASK, FRAME_SHIFT);
			currentframe--;
			break;
		}

		tableindex++;
	}

	if (tableindex == table_loc[pid] + PAGE_SIZE) {
		printf("Ran out of space in page table\n");
		return -1;
	}

	if (frame >= MEM_SIZE / PAGE_SIZE) {
		frame = evictpage(pid);
	}

	memory[tableindex] = maketableentry(vadd / PAGE_SIZE, frame, iswriteable);

	printf("Mapped virtual address %i (page %i) into physical frame %i\n", vadd, vadd / PAGE_SIZE, frame);
	return 0;
}

int store (int pid, int vadd, int value) {
	initprocess(pid);

	int tableentry = gettableentry(pid, vadd);
	if (tableentry == -1) {
		map(pid, vadd, 1);
		tableentry = gettableentry(pid, vadd);

		if (tableentry == -1) return -1;
	}

	if (gettablevalue(tableentry, PERM_MASK, PERM_SHIFT) != 1) {
		printf("\nError: writes are not allowed on page %i\n", gettablevalue(tableentry, VPN_MASK, VPN_SHIFT));
		return -1;
	}

	int padd = getpadd(pid, vadd);
	if (padd == -1) return -1;

	memory[padd] = (uint8_t)value;
	printf("Stored value %i at virtual address %i (physical address %i)\n", value, vadd, padd);
	return 0;
}

int load (int pid, int vadd) {
	initprocess(pid);

	int padd = getpadd(pid, vadd);
	if (padd == -1) return -1;

	int value = memory[padd];
	printf("The value %i is at virtual address %i (physical address %i)\n", value, vadd, padd);
	return 0;
}

int main () {
	// Initialize the page table locations to be blank
	for (int i = 0; i < NUM_PROC; i++) { table_loc[i] = -1; }

	while (1) {
		printf("Instruction? ");
		char input[50];
		fgets(input, 50, stdin);

		int pid, vadd, value;
		char type[50];

		int i = sscanf(input, "%i,%50[^,],%i,%i", &pid, type, &vadd, &value);
		if (i != 4) {
			printf("Invalid input. %i variables filled. \"%i\", \"%s\", \"%i, \"%i\"\n", i, pid, type, vadd, value);
			continue;
		}
		
		if (pid < 0 || pid > 3) {
			printf("PID is out of range.\n");
			continue;
		}
		if (vadd < 0 || vadd > 63) {
			printf("Virtual address is out of range.\n");
			continue;
		}
		if (value < 0 || value > 255) {
			printf("Value is out of range.\n");
			continue;
		}
		
		if (strcmp(type, "map") == 0) {
			map(pid, vadd, value);
		}
		else if (strcmp(type, "store") == 0) {
			store(pid, vadd, value);
		}
		else if (strcmp(type, "load") == 0) {
			load(pid, vadd);
		}

		// dump();
	}
}