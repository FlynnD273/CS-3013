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
	
	struct timespec lastprint;
	clock_gettime(CLOCK_REALTIME, &lastprint);

	int racingchildren = 4;
	while (racingchildren > 0) {
		int st = waitpid(-1, NULL, WNOHANG);
		struct timespec end;
		clock_gettime(CLOCK_REALTIME, &end);

		if (st <= 0) {
			// check for last print time, if it's larger than 0.33 seconds

			float lastprintdelta = end.tv_sec - lastprint.tv_sec + (end.tv_nsec - lastprint.tv_nsec) / 1000000000.0;
			if (lastprintdelta >= 0.33) {
				clock_gettime(CLOCK_REALTIME, &lastprint);

				// print all running children
				printf("The race is ongoing. The following children are still racing: ");
				for (int i = 0; i < numchildren; i++) {
					if (kids[i] != 0) {
						printf("%d ", kids[i]);
					}
				}
				printf("\n");
			}

			continue;
		}

		--racingchildren;

		for (int i = 0; i < numchildren; i++) {
			if (kids[i] == st) {
				kids[i] = 0;
			}
		}

		float delta = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1000000000.0;

		printf("Child %d has crossed the finish line! It took %f seconds\n", st, delta);
	}
}
