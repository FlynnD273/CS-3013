#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdbool.h>
#include "seed_reader.h"

//just listing out all the possible paths a small or large plane could use to take off, and storing them in an array for easy random access
int longpaths[][3] = { {0, 3, 5}, {5, 3, 0}, {1, 2, 4}, {4, 2, 1} };
int shortpaths[][2] = { {0, 1}, {0, 3}, {1, 0}, {1, 2}, {2, 1}, {2, 3}, {2, 4}, {3, 0}, {3, 2}, {3, 5}, {4, 2}, {4, 5}, {5, 3}, {5, 4} };

//region locks
sem_t regions [6];

#define LARGE_COUNT 15
#define SMALL_COUNT 30

pthread_t largethreads[LARGE_COUNT];
pthread_t smallthreads[SMALL_COUNT];

int get_small_plane_number(int i) {
	return i + LARGE_COUNT;
}

int await_runway_large(int i) {
	int sequence = rand() % 4;
	printf("Plane #%d (Large) is waiting for runway regions %i, %i, and %i\n", i, longpaths[sequence][0]+1, longpaths[sequence][1]+1, longpaths[sequence][2]+1);
	bool clear = false;
	while (!clear) {
		sem_wait(&regions[longpaths[sequence][0]]);
		if (sem_trywait(&regions[longpaths[sequence][1]]) == 0) {
			if (sem_trywait(&regions[longpaths[sequence][2]]) == 0) {
				clear = true;
			}
			else {
				sem_post(&regions[longpaths[sequence][0]]);
				sem_post(&regions[longpaths[sequence][1]]);
			}
		}
		else {
			sem_post(&regions[longpaths[sequence][0]]);
		}
	} 
	
	
	return sequence;
}

void idle_large(int i) {
	int delay = (rand() % 4) + 1;
	printf("Plane #%d (Large) is idling at the terminal for %d seconds\n", i, delay);
	sleep(delay);
}

int await_takeoff_large(int i) {
	printf("Plane #%d (Large) is preparing to take off\n", i);
	int seq = await_runway_large(i);
	return seq;
}

void takeoff_large(int i, int path_index) {
	int delay;
	printf("Plane #%d (Large) is taking off on runway regions %i, %i, and %i\n", i, longpaths[path_index][0]+1, longpaths[path_index][1]+1, longpaths[path_index][2]+1);
	for (int j = 0; j < 3; j++) {
		delay = (rand() % 4) + 1;
		printf("Plane #%i (Large) has entered region %i while taking off\nWaiting %i seconds\n", i, longpaths[path_index][j]+1, delay);
		sleep(delay);
		sem_post(&regions[longpaths[path_index][j]]);
	}
	printf("Plane #%i (Large) has taken off\n", i);
}

void fly_large(int i) {
	int delay = (rand() % 4) + 4;
	printf("Plane #%d (Large) is flying for %d seconds\n", i, delay);
	sleep(delay);
}

int await_land_large(int i) {
	printf("Plane #%d (Large) is preparing to land\n", i);
	int seq = await_runway_large(i);
	return seq;
}

void land_large(int i, int path_index) {
	int delay;
	printf("Plane #%d (Large) is landing on runway regions %i, %i, and %i\n", i, longpaths[path_index][0]+1, longpaths[path_index][1]+1, longpaths[path_index][2]+1);
	for (int j = 0; j < 3; j++) {
		delay = (rand() % 4) + 1;
		printf("Plane #%i (Large) has entered region %i while landing\nWaiting for %i seconds\n", i, longpaths[path_index][j]+1, delay);
		sleep(delay);
		sem_post(&regions[longpaths[path_index][j]]);
	}
	printf("Plane #%i (Large) has landed\n", i);
}

void *run_large(void *arg) {
	int i = (long long int)arg;

	int path_index;
	while (true) {
		idle_large(i);
		path_index = await_takeoff_large(i);
		takeoff_large(i, path_index);
		fly_large(i);
		path_index = await_land_large(i);
		land_large(i, path_index);
	}
}

int spawn_large(int i) {
	long long int index = i;
	return pthread_create(&largethreads[i], NULL, run_large, (void*)index);
}

int await_runway_small(int i) {
	int sequence = rand() % 14;
	printf("Plane #%d (Small) is waiting for runway regions %i and %i\n", get_small_plane_number(i), shortpaths[sequence][0]+1, shortpaths[sequence][1]+1);
	bool clear = false;
	while (!clear) {
		sem_wait(&regions[shortpaths[sequence][0]]);
		if (sem_trywait(&regions[shortpaths[sequence][1]]) == 0) {
			clear = true;
		}
		else {
			sem_post(&regions[shortpaths[sequence][0]]);
		}
	} 
	return sequence;
}

void idle_small(int i) {
	int delay = (rand() % 4) + 1;
	printf("Plane #%d (Small) is idling at the terminal for %d seconds\n", get_small_plane_number(i), delay);
	sleep(delay);
}

int await_takeoff_small(int i) {
	printf("Plane #%d (Small) is preparing to take off\n", get_small_plane_number(i));
	int seq = await_runway_small(i);
	return seq;
}

void takeoff_small(int i, int path_index) {
	int delay;
	printf("Plane #%d (Small) is taking off on runway regions %i and %i\n", get_small_plane_number(i), shortpaths[path_index][0]+1, shortpaths[path_index][1]+1);
	for (int j = 0; j < 2; j++) {
		delay = (rand() % 4) + 1;
		printf("Plane #%i (Small) has entered region %i while taking off\nWaiting %i seconds\n", get_small_plane_number(i), shortpaths[path_index][j]+1, delay);
		sleep(delay);
		sem_post(&regions[shortpaths[path_index][j]]);
	}
	printf("Plane #%i (Small) has taken off\n", get_small_plane_number(i));
}

void fly_small(int i) {
	int delay = (rand() % 4) + 1;
	printf("Plane #%d (Small) is flying for %d seconds\n", get_small_plane_number(i), delay);
	sleep(delay);
}

int await_land_small(int i) {
	printf("Plane #%d (Small) is preparing to land\n", get_small_plane_number(i));
	int seq = await_runway_small(i);
	return seq;
}

void land_small(int i, int path_index) {
	int delay;
	printf("Plane #%d (Small) is landing on runway regions %i and %i\n", get_small_plane_number(i), shortpaths[path_index][0]+1, shortpaths[path_index][1]+1);
	for (int j = 0; j < 2; j++) {
		delay = (rand() % 4) + 1;
		printf("Plane #%i (Small) has entered region %i while landing\nWaiting for %i seconds\n", get_small_plane_number(i), shortpaths[path_index][j]+1, delay);
		sleep(delay);
		sem_post(&regions[shortpaths[path_index][j]]);
	}
	printf("Plane #%i (Small) has landed\n", get_small_plane_number(i));
}

void *run_small(void *arg) {
	int i = (long long int)arg;
	int path_index;
	while (true) {
		idle_small(i);
		path_index = await_takeoff_small(i);
		takeoff_small(i, path_index);
		fly_small(i);
		path_index = await_land_small(i);
		land_small(i, path_index);
	}
}

int spawn_small(int i) {
	long long int index = i;
	return pthread_create(&smallthreads[i], NULL, run_small, (void*)index);
}

int main () {
	srand(readseed("seed.txt"));

	int r = 0;
	for (int i = 0; i < 6; i++)
		sem_init(&regions[i], 0, 1);

	for(int i = 0; i < LARGE_COUNT; i++) {
		r = spawn_large(i);
		if (r != 0) return 1;
	}

	for(int i = 0; i < SMALL_COUNT; i++) {
		r = spawn_small(i);
		if (r != 0) return 1;
	}

	for(int i = 0; i < LARGE_COUNT; i++) {
		pthread_join(largethreads[i], NULL);
	}

	for(int i = 0; i < SMALL_COUNT; i++) {
		pthread_join(smallthreads[i], NULL);
	}
}
