#include <stdio.h>
#include <stdlib.h>
#include "seed_reader.h"

int readseed() {
	FILE* seedfile = fopen ("seed.txt", "r");
	char buff[255];
	fscanf(seedfile, "%s", buff);
	fclose(seedfile);
	return atoi(buff);
}
