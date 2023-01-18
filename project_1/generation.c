#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include "seed_reader.h"

int main () {
	int seed = readseed();
	srand(seed);
	int childCount = (rand() % 8) + 5;
	printf("seed: %i\ncount: %i\n", seed, childCount);

	int status;

	while (childCount > 0) {
		childCount--;
		pid_t child = fork();
		if (child == 0) {
			waitpid(child, &status, WUNTRACED);
			break;
		}
	}
	printf("childCount: %i\n", childCount);
	return childCount;
}
