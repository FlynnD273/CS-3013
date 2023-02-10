#include <unistd.h>
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

	int progress;
	int starttime;
	int lasttime;
	int waittime;

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

void runorderedjob(struct job *joblist) {
	int time = 0;

	for (struct job *current = joblist->next; current != NULL; current = current->next) {
		printf("Job %i ran for: %i\n", current->id, current->length);
		current->progress = current->length;
		current->starttime = time;
		current->waittime += time - current->lasttime;
		time += current->length;
		current->lasttime = time;
	}
}

void runfifo(struct job *joblist) {
	printf("Execution trace with FIFO:\n");
	runorderedjob(joblist);	
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
	runorderedjob(joblist);	
	printf("End of execution with SJF.\n");
}

void runrr(struct job *joblist, int timeslice) {
	printf("Execution trace with RR:\n");
	int cont = 1;
	int time = 0;

	while (cont) {
		cont = 0;

		for (struct job *current = joblist->next; current != NULL; current = current->next) {
			if (current->progress >= current->length) continue;

			if (current->progress == 0) {
				current->starttime = time;
			}

			current->waittime += time - current->lasttime;

			if (current->length - current->progress <= timeslice) {
				printf("Job %i ran for: %i\n", current->id, current->length - current->progress);
				current->progress = current->length;
				time += current->length;
			}
			else {
				printf("Job %i ran for: %i\n", current->id, timeslice);
				current->progress += timeslice;
				time += timeslice;
				cont = 1;
			}

			current->lasttime = time;
		}
	}

	printf("End of execution with RR.\n");

}

void runanalysis(struct job *joblist, enum algtype alg) {
	switch (alg) {
		case fifo:
			printf("Begin analyzing FIFO:\n");
			break;
		case sjf:
			printf("Begin analyzing SJF:\n");
			break;
		case rr:
			printf("Begin analyzing RR:\n");
			break;
	}
	int response, turnaround, wait, id;
	int totalresponse = 0, totalturnaround = 0, totalwait = 0, totalcount = 0;
	for (struct job *current = joblist->next; current != NULL; current = current->next) {
		id = current->id;
		response = current->starttime;
		turnaround = current->lasttime;
		wait = current->waittime;
		printf("Job %i -- Response time: %i  Turnaround: %i  Wait: %i\n", id, response, turnaround, wait);
		totalresponse += response;
		totalturnaround += turnaround;
		totalwait += wait;
		totalcount += 1;
	}
	float avgresponse, avgturnaround, avgwait;
	avgresponse = (float)totalresponse/totalcount;
	avgturnaround = (float)totalturnaround/totalcount;
	avgwait = (float)totalwait/totalcount;
	printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n", avgresponse, avgturnaround, avgwait);
	switch (alg) {
		case fifo:
			printf("End analyzing FIFO.\n");
			break;
		case sjf:
			printf("End analyzing SJF.\n");
			break;
		case rr:
			printf("End analyzing RR.\n");
			break;
	}
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
	runanalysis(joblist, alg);
}
