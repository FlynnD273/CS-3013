#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "seed_reader.h"

int main() {
	int seed = readseed("seed.txt");
	srand(seed);
	int numchildren = 4;
	if (numchildren > 9) return -1;

	printf("Number of children: %i\n", numchildren);
	
	int* kids = malloc(sizeof(int) * numchildren);
	for (int i = 0; i < numchildren; i++) {
		pid_t child = fork();
		kids[i] = child;

		if (child != 0) {
			printf("[Parent]: I forked off child %i.\n", child);
		}
		else {
			pid_t mypid = getpid();
			printf("\t[Child, PID: %i]: Executing \'./slug %d\' command...\n", mypid, i + 1);
			char *argv[3];
			argv[0] = "./slug";

			char num[2];
			sprintf(num, "%d", i + 1);
			argv[1] = num;
			argv[3] = NULL;

			return execvp(argv[0], argv);
		}
	}
	
	
	struct timespec start;
	clock_gettime(CLOCK_REALTIME, &start);

	int racingchildren = 4;
	while (racingchildren > 0) {
		int st = waitpid(-1, NULL, WNOHANG);
		if (st <= 0) {
			// check for last print time, if it's larger than 0.33 seconds

			// print all running children
		}

		--racingchildren;

		struct timespec end;
		clock_gettime(CLOCK_REALTIME, &end);
		float delta = end.tv_sec - start.tv_sec + (end.tv_nsec - end.tv_nsec) / 1000000000.0;

		printf("Child %d has crossed the finish line! It took %f seconds\n", st, delta);
	}
}
