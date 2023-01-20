#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "seed_reader.h"

int main() {
	char* directories[6];
	directories[0] = "/home";
	directories[1] = "/proc";
	directories[2] = "/proc/sys";
	directories[3] = "/usr";
	directories[4] = "/usr/bin";
	directories[5] = "/bin";
	
	
	int seed = readseed("seed.txt");
	srand(seed);
	printf("Seed value: %i\n", seed);
	
	char* command_info[3];
	command_info[0] = "ls";
	command_info[1] = "-tr";
	command_info[2] = NULL;
	
	for (int i = 0; i < 5; i++) {
		int location = rand() % 6;
		char* current_directory = *(directories + location);
		pid_t child = fork();
		
		//child
		if (child == 0) {
			pid_t mypid = getpid();
			printf("\t[Child, PID: %i]: Executing 'ls -tr' command...\n", mypid);
			int return_code = execvp(command_info[0], command_info);
			exit(return_code);
		}
		
		//parent
		else {
			printf("Selection #%i: %s\n", i+1, current_directory);
			chdir(current_directory);
			char* cwd_return = malloc(sizeof(char) * 255);
			getcwd(cwd_return, 255);
			printf("Current reported directory: %s\n", cwd_return);
			printf("[Parent]: I am waiting for PID %i to finish.\n", child);
			int status;
			waitpid(child, &status, WUNTRACED);
			printf("[Parent]: Child %i finished with status code %i. Onward!\n", child, WEXITSTATUS(status));
		}
	}
}
