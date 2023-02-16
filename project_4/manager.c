#include <stdio.h>
#include <string.h>

#define MEM_SIZE 64
#define PAGE_SIZE 16
#define NUM_PROC 4

int table_loc[NUM_PROC]; // Index for the location of the page table for a given proccess ID
// Page table entries should be the page number then the frame number then the permission bits.
// Example:
// memory[0] = 2; // Page 2
// memory[1] = 3; // Maps to physical frame 3
// memory[2] = 1; // For read and write privilages
unsigned char memory[MEM_SIZE];

int gettableentry (int pid, int vadd) {
	for (int i = table_loc[pid]; i < table_loc[pid] + PAGE_SIZE - 2; i += 3) {
		if (memory[i] == vadd / PAGE_SIZE) {
			return i;
		}
	}

	printf("Error: no table entry found for virtual address %i\n", vadd);
	return -1;
}

int getframe (int pid, int vadd) {
	int tableentry = gettableentry(pid, vadd);
	if (tableentry == -1) return -1;

	return memory[tableentry + 1];
}

int getpadd (int pid, int vadd) {
	int frame = getframe(pid, vadd);
	if (frame == -1) return -1;

	return frame * PAGE_SIZE + vadd % PAGE_SIZE;
}

int initprocess (int pid) {
	if (table_loc[pid] != -1) return 0;

	int frame = 0;

	for (int i = 0; i < NUM_PROC; i++) {
		if (table_loc[i] < 0) continue;
		if (table_loc[i] == frame) {
			frame++;
			i = -1;
			continue;
		}

		for (int j = table_loc[i]; j < table_loc[i] + PAGE_SIZE - 2; j += 3) {
			if (memory[j + 1] == frame) {
				frame++;
				i = -1;
				continue;
			}
		}
	}

	table_loc[pid] = frame;

	printf("Put page table for PID %i into physical frame %i\n", pid, frame);
}

int map (int pid, int vadd, int iswriteable) {
	initprocess(pid);

	int frame = 1;

	for (int i = 0; i < NUM_PROC; i++) {
		if (table_loc[i] < 0) continue;
		if (table_loc[i] == frame) {
			frame++;
			i = -1;
			continue;
		}

		for (int j = table_loc[i]; j < table_loc[i] + PAGE_SIZE - 2; j += 3) {
			if (memory[j + 1] == frame) {
				frame++;
				i = -1;
				continue;
			}
		}
	}

	if (frame >= MEM_SIZE / PAGE_SIZE) {
		// No more physical memory
		printf("Ran out of physical memory\n");
		return -1;
	}

	int tableindex = table_loc[pid];
	while (memory[tableindex] != 0 && tableindex < table_loc[pid] + PAGE_SIZE - 2) {
		tableindex += 3;
	}
	if (tableindex == table_loc[pid] + PAGE_SIZE - 2) {
		printf("Ran out of space in page table\n");
		return -1;
	}

	memory[tableindex] = vadd / PAGE_SIZE;
	memory[tableindex + 1] = frame;
	memory[tableindex + 2] = iswriteable;

	printf("Mapped virtual address %i (page %i) into physical frame %i\n", vadd, vadd / PAGE_SIZE, frame);
	return 0;
}

int store (int pid, int vadd, int value) {
	initprocess(pid);

	int tableentry = gettableentry(pid, vadd);
	if (tableentry == -1) return -1;

	if (memory[tableentry + 2] != 1) {
		printf("Error: write are not allowed on page %i\n", memory[tableentry]);
		return -1;
	}

	int padd = getpadd(pid, vadd);
	if (padd == -1) return -1;

	memory[padd] = value;
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

void dump () {
	for (int i = 0; i < NUM_PROC; i++) {
		printf("%i: %i\n", i, table_loc[i]);
	}
	printf("Memory:\n");
	for (int i = 0; i < MEM_SIZE; i++) {
		printf("%i: %i\n", i, memory[i]);
	}
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
	}
}
