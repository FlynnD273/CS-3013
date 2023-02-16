
#define MEM_SIZE 64
#define PAGE_SIZE 16

int table_loc[4]; // Index for the location of the page table for a given proccess ID
unsigned char memory[MEM_SIZE];

int map (int pid, int vadd, int iswriteable) {
	return -1;
}

int store (int pid, int vadd, int value) {
	return -1;
}

int load (int pid, int vadd) {
	return -1;
}


int main () {
	
}
