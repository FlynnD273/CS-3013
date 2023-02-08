#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum algtype {
	fifo,
	sjf,
	rr
};

struct job {
	int id;
	int length;
	struct job *next;
};

struct job *readjobs (char *path) {
	struct job *firstjob = &(struct job) { 0, 0 };
	struct job *currentjob = firstjob;
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(path, "r");
	if (fp == NULL)
			exit(EXIT_FAILURE);

	int i = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
			currentjob->length = atoi(line);
			currentjob->id = i;
			currentjob->next = (struct job *)malloc(sizeof(struct job));
			currentjob = currentjob->next;
			++i;
	}

	fclose(fp);
	if (line)
		free(line);
	
	return firstjob;
}

int main(int argc, char **argv) {
	if (argc != 4) return -1;
	enum algtype alg;
	if (strcmp(argv[1], "FIFO") == 0) {
		alg = fifo;
	}
	else if (strcmp(argv[1], "SJF") == 0) {
		alg = sjf;
	}
	else if (strcmp(argv[1], "RR") == 0) {
		alg = rr;
	}
	else {
		return -2;
	}

	char *path = argv[2];
	int timeslice = atoi(argv[3]);

	printf("Algorithm: %d\nPath: %s\nTime Slice: %d\n", alg, path, timeslice);

	struct job *joblist = readjobs(path);
	struct job *current = joblist;
	do {
		printf("ID: %d-> Length: %d\n", current->id, current->length);
		current = current->next;
	}
	while (current->next);
}
