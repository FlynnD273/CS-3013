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
	struct job *head = (struct job *)malloc(sizeof(struct job));
	head->id = -1;
	head->length = -1;

	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(path, "r");
	if (fp == NULL)
			exit(EXIT_FAILURE);

	int i = 0;
	struct job *currentjob = head;

	while ((read = getline(&line, &len, fp)) != -1) {
			currentjob->next = (struct job *)malloc(sizeof(struct job));
			currentjob = currentjob->next;

			currentjob->length = atoi(line);
			currentjob->id = i++;
	}

	fclose(fp);
	if (line)
		free(line);
	
	return head;
}

void runfifo(struct job *joblist) {
	printf("Execution trace with FIFO:\n");
	struct job *current = joblist->next;
	while (current != NULL) {
		printf("Job %i ran for: %i\n", current->id, current->length);
		current = current->next;
	}

	printf("End of execution with FIFO.\n");
}

void swap(struct job *a, struct job *b) {
	struct job temp = *a;
	a->id = b->id;
	a->length = b->length;
	b->id = temp.id;
	b->length = temp.length;
}

void sortlist(struct job *joblist) {
	int cont = 1;
	while (cont == 1) {
		cont = 0;
		for (struct job *current = joblist->next; current->next != NULL; current = current->next) {
			if (current->length > current->next->length) {
				swap(current, current->next);
				cont = 1;
			}
		}
	}
}

void runsjf(struct job *joblist) {
	sortlist(joblist);
	printf("Execution trace with SJF:\n");
	struct job *current = joblist->next;
	while (current != NULL) {
		printf("Job %i ran for: %i\n", current->id, current->length);
		current = current->next;
	}

	printf("End of execution with SJF.\n");
}

void runrr(struct job *joblist, int timeslice) {
	printf("Execution trace with RR:\n");
	while (joblist->next != NULL) {
		struct job *current = joblist->next;
		struct job *prev = joblist;
		while (current != NULL) {
			if (current->length <= timeslice) {
				printf("Job %i ran for: %i\n", current->id, current->length);
				prev->next = current->next;
				free(current);
				current = prev;
			}
			else {
				printf("Job %i ran for: %i\n", current->id, timeslice);
				current->length -= timeslice;
			}
			prev = current;
			current = current->next;
		}
	}

	printf("End of execution with RR.\n");

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

	struct job *joblist = readjobs(path);
	switch (alg) {
		case fifo:
			runfifo(joblist);
			break;
		case sjf:
			runsjf(joblist);
			break;
		case rr:
			runrr(joblist, timeslice);
			break;
	}
}
