#include <stddef.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>

const static size_t BUFFER_SIZE = 256;
const static char ERROR_MESSAGE[] = "An error occurred";
const static char INITIAL_PATH[] = "/bin";

int main() {
	char* prompt = (char*)calloc(BUFFER_SIZE, sizeof(char));
	char* path = (char*)calloc(BUFFER_SIZE, sizeof(char));

	strcpy(path, INITIAL_PATH);

	while(1) {
		fprintf(stdout, "%s", "wish> ");
		fgets(prompt, BUFFER_SIZE, stdin);
		prompt[strcspn(prompt, "\n")] = '\0'; // Replace the endline character with terminator
		char** arg = calloc(BUFFER_SIZE, sizeof(char*));
		size_t lastArg = 0;
		char* prompt_start = prompt;
		char* token = strsep(&prompt, " ");
		while(token != NULL) {
			arg[lastArg++] = token;
			token = strsep(&prompt, " ");
		}


		char* exec_path = (char*)calloc(BUFFER_SIZE, sizeof(char));
		strcat(exec_path, path);
		strcat(exec_path, "/");
		strcat(exec_path, arg[0]);
		arg[0] = exec_path;

		pid_t childpid = fork();

		if(childpid == 0) { // Child process
			execv(arg[0], arg);
			fprintf(stderr, "%s\n", ERROR_MESSAGE);
		}
		// Continuing in the parent process
		free(arg);
		for(size_t i = 0; i < BUFFER_SIZE; i++)
			prompt_start[i] = '\0';
		prompt = prompt_start;
		// Waiting for the child process
		wait(NULL);
	}
	free(prompt);
}
