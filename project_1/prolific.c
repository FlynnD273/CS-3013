#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "seed_reader.h"

int main() {
	int parentpid = getpid();

	int seed = readseed("seed.txt");
	srand(seed);
	int numchildren = rand() % 6 + 8;
	
	printf("Seed value: %i\n", seed);
	printf("Number of children: %i\n", numchildren);
	
	int* kids = malloc(sizeof(int) * numchildren);
	for (int i = 0; i < numchildren; i++) {
		kids[i] = rand();
	}
	
	for (int currentchild = 0; currentchild < numchildren; currentchild++) {
		pid_t child = fork();
		//parent
		if (child != 0) {
			printf("[Parent]: I am waiting for PID %i to finish.\n", child);
			
			int status;
			waitpid(child, &status, WUNTRACED);
			
			printf("[Parent]: Child %i finished with status code %i. Onward!\n", child, WEXITSTATUS(status));
		}
		
		else {
			int waittime = kids[currentchild] % 3 + 1;
			int exitcode = kids[currentchild] % 50 + 1;
			printf("\t[Child, PID: %i]: I am the child and I will wait %i seconds and exit with code %i.\n", getpid(), waittime, exitcode);
			sleep(waittime);
			printf("\t[Child, PID: %i]: Now exiting...\n", getpid());
			return exitcode;
		}
	}
}
