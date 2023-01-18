#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include "seed_reader.h"

int main () {
	// Initialize the seed from the seed.txt file
	int seed = readseed("seed.txt");
	srand(seed);

	// Set the number of children to make, from the seeded value
	int childcount = (rand() % 8) + 5;

	while (childcount > 0) {
		pid_t childpid = fork();

		// Parent
		if (childpid != 0) {
			printf("[Parent, PID: %i]: I am waiting for PID %i to finish\n", getpid(), childpid);

			int status;
			waitpid(childpid, &status, WUNTRACED);

			printf("[Parent, PID: %i]: Child %i finished with status code %i. It's now my turn to exit.\n"
			, getpid(), childpid, WEXITSTATUS(status));
			break;
		}
		// Child
		else {
			printf("\t[Child, PID: %i]: I was called with descendant count=%i. I\'ll have %i descendants(s).\n"
			, getpid(), childcount, childcount - 1);
		}
		childcount--;
	}

	return childcount;
}
