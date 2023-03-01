#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define MEM_SIZE 64
#define PAGE_SIZE 16
#define NUM_PROC 4
#define PROC_SIZE 64

#define PERM_MASK   0b10000000
#define PERM_SHIFT  7
#define PRES_MASK   0b01000000
#define PRES_SHIFT  6
#define VALID_MASK  0b00010000
#define VALID_SHIFT 4
#define VPN_MASK    0b00001100
#define VPN_SHIFT   2
#define FRAME_MASK  0b00000011
#define FRAME_SHIFT 0

int table_loc[NUM_PROC]; // Index for the location of the page table for a given process ID
// Page table entries should be the permission bit, the present bit, valid bit, virtual page number, then frame number
unsigned char memory[MEM_SIZE];


//storage space for size of each process, plus one extra page each for their page table
//unsigned char disk[NUM_PROC * (PROC_SIZE + PAGE_SIZE)];
//page table for a given process is stored at PID * (PROC_SIZE + PAGE_SIZE)
//data pages are stored following their respective page tables

FILE* disk;

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

int isgoodentry (int entry) {
	int present = gettablevalue(entry, PRES_MASK, PRES_SHIFT);
	int valid = !gettablevalue(entry, VALID_MASK, VALID_SHIFT);
	//inverted becasue 0 means valid
	
	return valid && present;
}

int maketableentry (int vpn, int frame, int perm) {
	int vpnbits = (vpn << VPN_SHIFT) & VPN_MASK;
	int framebits = (frame << FRAME_SHIFT) & FRAME_MASK;
	int permbits = (perm << PERM_SHIFT) & PERM_MASK;
	int presbits = (1 << PRES_SHIFT) & PRES_MASK;

	return vpnbits | framebits | permbits | presbits;
}

void storetodisk (int frame, int diskaddress) {
	disk = fopen("disk", "rb+");
	char* memstart = memory + (frame * PAGE_SIZE);
	fseek(disk, diskaddress, SEEK_SET);
	fwrite(memstart, 1, PAGE_SIZE, disk);
	fclose(disk);
}

void loadfromdisk (int frame, int diskaddress) {
	disk = fopen("disk", "rb");
	char* memstart = memory + (frame * PAGE_SIZE);
	fseek(disk, diskaddress, SEEK_SET);
	fread(memstart, 1, PAGE_SIZE, disk);
	fclose(disk);
}

void initfile () {
	disk = fopen("disk", "wb");
	int nothing = 0;
	for (int i = 0; i < NUM_PROC * (PROC_SIZE + PAGE_SIZE); i++) {
		fwrite(&nothing, 1, 1, disk);
	}
	fclose(disk);
}

void evictspecificpage(int pid, int vpn, int frame) {
	//int memstart = frame * PAGE_SIZE;
	int diskstart = (pid * (PROC_SIZE + PAGE_SIZE)) + ((vpn + 1) * PAGE_SIZE);
	storetodisk(frame, diskstart);
	/*for (int i = 0; i < PAGE_SIZE; i++) {
		disk[diskstart + i] = memory[memstart + i];
	}*/
	printf("Swapped frame %i to disk at swap slot %i\n", frame, diskstart / PAGE_SIZE);
}

int evictpt (int pid) {
	// Evict the page table from memory and store it in the swap file. This should be called from evictpage only.
	
	int tablestart = table_loc[pid] * PAGE_SIZE;
	
	//remove all pages associated with page table
	for (int i = 0; i < PAGE_SIZE && memory[tablestart+i] != 255; i++) {
		int entry = memory[tablestart+i];
		//check if entry currently points to a page in memory
		if (!isgoodentry(entry)) {
			continue;
		}
	
		//remove page associated with table entry
		int vpn = gettablevalue(entry, VPN_MASK, VPN_SHIFT);
		int frame = gettablevalue(entry, FRAME_MASK, FRAME_SHIFT);
		evictspecificpage(pid, vpn, frame);
		
		//update page table to indicate entry not present
		memory[tablestart+i] = entry ^ PRES_MASK;
	}
	
	//move page table to disk
	evictspecificpage(pid, -1, table_loc[pid]); //-1 can indicate page table with current implementation of evictspecificpage

	// Page table is present, but on disk
	table_loc[pid] = -2;
	return -1;
}

int evictpage (int pid) {
	// Evict a page from memory. Skip currentframe if it's the page table for the given PID
	int evictframe = currentframe % (MEM_SIZE / PAGE_SIZE);
	if (table_loc[pid] == evictframe){
		currentframe++;
		evictframe++;
	}
	currentframe++;
	
	int frameispagetable = -1;
	for (int i = 0; i < NUM_PROC && frameispagetable == -1; i++) {
		if (table_loc[i] == evictframe) {
			frameispagetable = i;
		}
	}
	
	//if evicting a data page:
	if (frameispagetable == -1) {
		//find the page table containing this page
		int ownerprocess = -1;
		int tableentrypos = -1;
		for (int i = 0; i < NUM_PROC && ownerprocess == -1; i++) {
			if (table_loc[i] < 0) {
				continue;
			}
			int memstart = table_loc[i] * PAGE_SIZE;
			for (int j = 0; j < PAGE_SIZE && memory[memstart+j] != 255; j++) {
				if (evictframe == gettablevalue(memory[memstart+j], FRAME_MASK, FRAME_SHIFT) && gettablevalue(memory[memstart+j], PRES_MASK, FRAME_MASK)) {
					//page table points to page being removed
					ownerprocess = i;
					tableentrypos = j;
					break;
				}
			}
		}
		if (ownerprocess != -1) {
			//get table entry for this page
			int entry = memory[table_loc[ownerprocess] * PAGE_SIZE + tableentrypos];
			int vpn = gettablevalue(entry, VPN_MASK, VPN_SHIFT);
			
			//move page to appropriate location in storage
			//int memstart = evictframe * PAGE_SIZE;
			int diskstart = (ownerprocess * (PROC_SIZE + PAGE_SIZE)) + ((vpn + 1) * PAGE_SIZE);
			storetodisk(evictframe, diskstart);
			/*for (int i = 0; i < PAGE_SIZE; i++) {
				disk[diskstart + i] = memory[memstart + i];
			}*/
			printf("Swapped frame %i to disk at swap slot %i\n", evictframe, diskstart / PAGE_SIZE);
			//update page table entry - flip present bit
			memory[table_loc[ownerprocess] * PAGE_SIZE + tableentrypos] = entry ^ PRES_MASK;
		}
		//if not in any page table in memory, this page is not in use
	}
	
	//if evicting a page table:
	else {
		evictpt(frameispagetable);
	}
	
	// Return the frame number
	return evictframe;
}

int loadpt (int pid) {
	// Load the page table for the given PID from the swap file, return the frame number that it was put in
	
	//make space for page table
	int frame = evictpage(pid);
	
	//move page table to memory in given frame
	//int memstart = frame * PAGE_SIZE;
	int diskstart = (pid * (PROC_SIZE + PAGE_SIZE));
	loadfromdisk(frame, diskstart);
	/* for (int i = 0; i < PAGE_SIZE; i++) {
		memory[memstart + i] = disk[diskstart + i];
	} */
	printf("Loaded page table for pid %i at frame %i from swap slot %i\n", pid, frame, diskstart / PAGE_SIZE);
	
	return frame;
}

int loadpage (int pid, int vpn) {
	// Load page from disk. Skip currentframe if it's the page table for the given PID
	//make space for page table
	int frame = evictpage(pid);
	
	//move page to memory in given frame
	//int memstart = frame * PAGE_SIZE;
	int diskstart = pid * (PROC_SIZE + PAGE_SIZE) + (vpn + 1) * PAGE_SIZE;
	loadfromdisk(frame, diskstart);
	/* deprecated
	for (int i = 0; i < PAGE_SIZE; i++) {
		memory[memstart + i] = disk[diskstart + i];
	}
	*/
	
	
	
	//update page table entry
	int tablestart = table_loc[pid] * PAGE_SIZE;
	for (int i = 0; i < PAGE_SIZE && memory[tablestart+i] != 255; i++) {
		int entry = memory[tablestart+i];
		//check if entry currently points to a page in memory
		if (isgoodentry(entry)) {
			continue; //current entry is already in memory, cannot be this
		}
	
		//check if entry points to same virtual page
		int entryvpn = gettablevalue(entry, VPN_MASK, VPN_SHIFT);
		if (entryvpn == vpn) {
			int perm = gettablevalue(entry, PERM_MASK, PERM_SHIFT);
			memory[tablestart+i] = maketableentry(vpn, frame, perm);
			break; //table is updated, no need to continue
		}
	}
	
	printf("Loaded swap slot %i (pid %i's virtual page %i) into frame %i\n", diskstart / PAGE_SIZE, pid, vpn, frame);
	
	// Return the frame that it was put into
	return frame;
}

int gettableentry (int pid, int vadd) {
	if (table_loc[pid] == -1) {
		printf("Error: page table not initialized for PID %i\n", pid);
		return -1;
	}
	if (table_loc[pid] == -2) {
		table_loc[pid] = loadpt(pid);
	}
	
	int memstart = table_loc[pid] * PAGE_SIZE;
	for (int i = 0; i < PAGE_SIZE && memory[memstart+i] != 255; i++) {
		if (gettablevalue(memory[memstart+i], VPN_MASK, VPN_SHIFT)  == vadd / PAGE_SIZE) {
			return memory[memstart+i];
		}
	}

	printf("Error: no table entry found for virtual address %i\n", vadd);
	return -1;
}

int getpadd (int pid, int vadd) {
	int entry = gettableentry(pid, vadd);
	if (!gettablevalue(entry, PRES_MASK, PRES_SHIFT)) {
		loadpage(pid, vadd / PAGE_SIZE);
		entry = gettableentry(pid, vadd);
	}
	
	int frame = gettablevalue(entry, FRAME_MASK, FRAME_SHIFT);
	if (frame == -1) return -1;

	return frame * PAGE_SIZE + vadd % PAGE_SIZE;
}

int initprocess (int pid) {
	if (table_loc[pid] != -1) return 0;

	int frame = currentframe;

	if (frame >= MEM_SIZE / PAGE_SIZE) {
		frame = evictpage(pid);
		//printf("Ran out of memory.\n");
		//return -1;
	} else {
		currentframe++;
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
	if (frame >= MEM_SIZE / PAGE_SIZE) {
		frame = evictpage(pid);
	} else {
		currentframe++;
	}
	int tableindex = table_loc[pid] * PAGE_SIZE;
	while (memory[tableindex] != 255 && tableindex < (table_loc[pid] + 1) * PAGE_SIZE) {
		int vpn = gettablevalue(memory[tableindex], VPN_MASK, VPN_SHIFT);

		if (vpn == vaddvpn) {
			frame = gettablevalue(memory[tableindex], FRAME_MASK, FRAME_SHIFT);
			currentframe--;
			break;
		}

		tableindex++;
	}

	if (tableindex == (table_loc[pid] + 1) * PAGE_SIZE) {
		printf("Ran out of space in page table\n");
		return -1;
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
	if (!gettablevalue(tableentry, PRES_MASK, PRES_SHIFT)) {
		loadpage(pid, vadd / PAGE_SIZE);
		tableentry = gettableentry(pid, vadd);
	}

	if (gettablevalue(tableentry, PERM_MASK, PERM_SHIFT) != 1) {
		printf("Error: writes are not allowed on page %i\n", gettablevalue(tableentry, VPN_MASK, VPN_SHIFT));
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
	
	//initialize disk
	initfile();

	while (1) {
		if (currentframe >= 2 * (MEM_SIZE / PAGE_SIZE)) {
			currentframe -= (MEM_SIZE / PAGE_SIZE);
		} //same modulo, but keeping above a minimum to ensure program knows all have been filled
		
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
