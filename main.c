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
		prompt[strcspn(prompt, "\n")] = '\0';

		char exec_path[BUFFER_SIZE] = {0};
		strcpy(exec_path, path);
		strcat(exec_path, "/");
		strcat(exec_path, prompt);
		pid_t childpid = fork();	

		if(childpid == 0) { // Child process
			char* args[] = { exec_path, NULL };
			execv(args[0], args);
			fprintf(stderr, "%s\n", ERROR_MESSAGE);
		}

		waitpid(childpid, NULL, 0);
	}
	free(prompt);
}
