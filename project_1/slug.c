#include "seed_reader.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define WRONG_ARG_COUNT -1
#define CHOICE_OUT_OF_RANGE -2

int main (int argc, char *argv[]) {
	if(argc != 2) return WRONG_ARG_COUNT;

	int choice = atoi(argv[1]);
	if (choice < 1 || choice > 4) return CHOICE_OUT_OF_RANGE;

	char path[16];
	sprintf(path, "seed_slug_%i.txt", choice);

	int seed = readseed(path);
	srand(seed);

	printf("[Slug PID: %i] Read seed value: %i\n", getpid(), seed);

	int wait = (rand() % 5) + 2;
	int isrunlast = rand() % 2;

	printf("[Slug PID: %i] Delay time is %i seconds. Coin flip: %i\n", getpid(), wait, isrunlast);
	printf("[Slug PID: %i] I'll get the job done eventually...\n", getpid());

	sleep(wait);


	if(isrunlast) {
		printf("[Slug PID: %i] Break time is over! I am running the \'id -group\' command.\n", getpid());

		char *argv[3];
		argv[0] = "id";
		argv[1] = "--group";
		argv[2] = NULL;
		execvp(argv[0], argv);
	}
	else {
		printf("[Slug PID: %i] Break time is over! I am running the \'last -i -x\' command.\n", getpid());

		char *argv[4];
		argv[0] = "last";
		argv[1] = "-i";
		argv[2] = "-x";
		argv[3] = NULL;
		execvp(argv[0], argv);
	}
}
