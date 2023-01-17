#include <stdio.h>
#include <stdlib.h>
//Reads a number from seed.txt and returns it as int
int seedToInt() {
	FILE* seedfile = fopen ("seed.txt", "r");
	char buff[255];
	fscanf(seedfile, "%s", buff);
	fclose(seedfile);
	return atoi(buff);
}

int main() {
	printf("%i\n", seedToInt()- 1234);
}
