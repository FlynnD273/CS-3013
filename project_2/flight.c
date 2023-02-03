#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdbool.h>
#include "seed_reader.h"

//just listing out all the possible paths a small or large plane could use to take off, and storing them in an array for easy random access
int longpathsp[][3] = { {1, 4, 6}, {6, 4, 1}, {2, 3, 5}, {5, 3, 2} };
int shortpaths[][2] = { {1, 2}, {1, 4}, {2, 1}, {2, 3}, {3, 2}, {3, 4}, {3, 5}, {4, 1}, {4, 3}, {4, 6}, {5, 3}, {5, 6}, {6, 4}, {6, 5} };

// Initialize to -1 at the start. Set this to the plane number that it's reserved for.
int regions[6] = {-1, -1, -1, -1, -1, -1};
#define LARGE_COUNT 15
#define SMALL_COUNT 30

pthread_t largethreads[LARGE_COUNT];
pthread_t smallthreads[SMALL_COUNT];

sem_t s;

int get_small_plane_number(int i) {
	return i + LARGE_COUNT;
}

int await_runway_large(int i) {
	return 0;
}

void idle_large(int i) {
	int delay = ((rand() % 50) + 5) * 1000;
	printf("Plane #%d (Large) is idling at the terminal for %d microseconds\n", i, delay);
	usleep(delay);
}

int await_takeoff_large(int i) {
	return await_runway_large(i);
}

void takeoff_large(int i, int path_index) {
	
}

void fly_large(int i) {
	int delay = ((rand() % 50) + 5) * 1000;
	printf("Plane #%d (Large) is flying for %d microseconds\n", i, delay);
	usleep(delay);
}

int await_land_large(int i) {
	return await_runway_large(i);
}

void land_large(int i, int path_index) {

}

void *run_large(void *arg) {
	int i = (long long int)arg;
	usleep(rand() % 1000);

	printf("This is large plane at index %d\n", i);
	int path_index;
	//while (true) {
		idle_large(i);
		path_index = await_takeoff_large(i);
		takeoff_large(i, path_index);
		fly_large(i);
		path_index = await_land_large(i);
		land_large(i, path_index);
	//}
}

int spawn_large(int i) {
	long long int index = i;
	return pthread_create(&largethreads[i], NULL, run_large, (void*)index);
}

int await_runway_small(int i) {
	int sequence = rand() % 14;
	printf("Plane #%d (Small) is waiting for runway regions %i and %i\n", get_small_plane_number(i), shortpaths[sequence][0], shortpaths[sequence][1]);
	bool clear = false;
	int sleeptime = 500000;
	while (!clear) {
		usleep(sleeptime); //need to rework this
		sem_wait(&s);
		clear = true;
		for (int j = 0; j < 2 && clear; j++)
			if (regions[shortpaths[sequence][j]] != -1)
				clear = false;
		if (clear)
			for (int j = 0; j < 2; j++)
				regions[shortpaths[sequence][j]] = get_small_plane_number(i);
		sem_post(&s);
		sleeptime = sleeptime - 50000 + (sleeptime == 50000) * 50000; //this line is objectively stupid, but i like stupid math
	}
	return sequence;
}

void idle_small(int i) {
	int delay = ((rand() % 50) + 5) * 1000;
	printf("Plane #%d (Small) is idling at the terminal for %d microseconds\n", get_small_plane_number(i), delay);
	usleep(delay);
}

int await_takeoff_small(int i) {
	printf("Plane #%d (Small) is preparing to take off\n", get_small_plane_number(i));
	int seq = await_runway_small(i);
	return seq;
}

void takeoff_small(int i, int path_index) {
	int delay;
	printf("Plane #%d (Small) is taking off on runway regions %i and %i\n", get_small_plane_number(i), shortpaths[path_index][0], shortpaths[path_index][1]);
	for (int j = 0; j < 2; j++) {
		printf("Plane #%i (Small) has entered region %i while taking off\n", get_small_plane_number(i), shortpaths[path_index][j]);
		delay = ((rand() % 50) + 5) * 1000;
		usleep(delay);
		regions[shortpaths[path_index][j]] = -1;
	}
	printf("Plane #%i (Small) has taken off\n", get_small_plane_number(i));
}

void fly_small(int i) {
	int delay = ((rand() % 50) + 5) * 1000;
	printf("Plane #%d (Small) is flying for %d microseconds\n", get_small_plane_number(i), delay);
	usleep(delay);
}

int await_land_small(int i) {
	printf("Plane #%d (Small) is preparing to land\n", get_small_plane_number(i));
	int seq = await_runway_small(i);
	return seq;
}

void land_small(int i, int path_index) {
	int delay;
	printf("Plane #%d (Small) is landing on runway regions %i and %i\n", get_small_plane_number(i), shortpaths[path_index][0], shortpaths[path_index][1]);
	for (int j = 0; j < 2; j++) {
		printf("Plane #%i (Small) has entered region %i while landing\n", get_small_plane_number(i), shortpaths[path_index][j]);
		delay = ((rand() % 50) + 5) * 1000;
		usleep(delay);
		regions[shortpaths[path_index][j]] = -1;
	}
	printf("Plane #%i (Small) has landed\n", get_small_plane_number(i));
}

void *run_small(void *arg) {
	int i = (long long int)arg;
	usleep(rand() % 1000);

	printf("This is small plane at index %d\n", get_small_plane_number(i));
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
	sem_init(&s, 0, 1);

	/*for(int i = 0; i < LARGE_COUNT; i++) {
		r = spawn_large(i);
		if (r != 0) return 1;
	}*/

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
