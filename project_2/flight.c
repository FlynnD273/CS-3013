#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include "seed_reader.h"

//just listing out all the possible paths a small or large plane could use to take off, and storing them in an array for easy random access
int longpathsp[][3] = { {1, 4, 6}, {6, 4, 1}, {2, 3, 5}, {5, 3, 2} };
int shortpaths[][2] = { {1, 2}, {1, 4}, {2, 1}, {2, 3}, {3, 2}, {3, 4}, {3, 5}, {4, 1}, {4, 3}, {4, 6}, {5, 3}, {5, 6}, {6, 4}, {6, 5} };

#define LARGE_COUNT 15
#define SMALL_COUNT 30

pthread_t largethreads[LARGE_COUNT];
pthread_t smallthreads[SMALL_COUNT];

sem_t printlock;

void *run_large_plane(void *arg) {
	long long int i = (long long int)arg;
	usleep(rand() % 1000);
	sem_wait(&printlock);
	printf("This is large plane at index %lld\n", i);
	sem_post(&printlock);
	return (void*)0;
}

int spawn_large_plane(long long int i) {
	long long int index = i;
	return pthread_create(&largethreads[i], NULL, run_large_plane, (void*)index);
}

void *run_small_plane(void *arg) {
	long long int i = (long long int)arg;
	usleep(rand() % 1000);
	sem_wait(&printlock);
	printf("This is small plane at index %lld\n", i);
	sem_post(&printlock);
	return (void*)0;
}

int spawn_small_plane(long long int i) {
	long long int index = i;
	return pthread_create(&smallthreads[i], NULL, run_small_plane, (void*)index);
}

int main () {
	srand(readseed("seed.txt"));

	sem_init(&printlock, 0, 1);

	int r = 0;

	for(long long int i = 0; i < LARGE_COUNT; i++) {
		r = spawn_large_plane(i);
		if (r != 0) return 1;
	}

	for(long long int i = 0; i < SMALL_COUNT; i++) {
		r = spawn_small_plane(i);
		if (r != 0) return 1;
	}

	for(int i = 0; i < LARGE_COUNT; i++) {
		pthread_join(largethreads[i], NULL);
	}

	for(int i = 0; i < SMALL_COUNT; i++) {
		pthread_join(smallthreads[i], NULL);
	}
}
