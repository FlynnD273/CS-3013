#include <stdio.h>
#include <stdlib.h>
#include "seed_reader.h"

int readseed(const char *path) {
	FILE *seedfile = fopen (path, "r");
	char buff[255];
	fscanf(seedfile, "%s", buff);
	fclose(seedfile);
	return atoi(buff);
}
