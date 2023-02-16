
#define MEM_SIZE 64
#define PAGE_SIZE 16

int table_loc[4]; // Index for the location of the page table for a given proccess ID
// Page table entries should be the page number then the frame number then the permission bits.
// Example:
// memory[0] = 2; // Page 2
// memory[1] = 3; // Maps to physical frame 3
// memory[2] = 1; // For read and write privilages
unsigned char memory[MEM_SIZE];

int map (int pid, int vadd, int iswriteable) {
	return -1;
}

int store (int pid, int vadd, int value) {
	return -1;
}

int load (int pid, int vadd) {
	int frame = -1;
	for (int i = table_loc[pid]; i < PAGE_SIZE - 2; i += 3) {
		if (memory[i] == vadd / PAGE_SIZE) {
			frame = memory[i+1];
			break;
		}
	}

	if (frame == -1) return -1; // No page allocated for this address

	int padd = frame * PAGE_SIZE + vadd % PAGE_SIZE;
	int value = memory[padd];
	printf("The value %i is at virtual address %i (physical address %i)\n", value, vadd, padd);
	return 0;
}


int main () {

}
