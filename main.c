#include<stddef.h>
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
			if(*token != '\0')
				arg[lastArg++] = token;

			token = strsep(&prompt, " ");
		}

		// Check for the blank command
		if (arg[0] == NULL) {
			// Do nothing
		} else if(!strcmp(arg[0], "exit")) { // Exit command
			fprintf(stdout, "%s\n", "Exitting...");
			exit(1);
		} else if(!strcmp(arg[0], "path")) {
			if(lastArg > 1) {
				for(size_t i = 0; i < BUFFER_SIZE; i++) {
					path[i] = '\0';
				}
				for(size_t i = 1; i < lastArg - 1; i++) {
					strcat(path, arg[i]);
					strcat(path, ":");
				}
				strcat(path, arg[lastArg - 1]);
				fprintf(stdout, "New PATH: %s\n", path);
			} else {
				fprintf(stdout, "PATH is: %s\n", path);
			}
		} else { // Run an executable
			pid_t childpid = fork();

			if(childpid == 0) { // Child process
				char* temp_path = (char*)malloc(BUFFER_SIZE * sizeof(char));
				strcpy(temp_path, path);

				char* try_path = calloc(sizeof(char), BUFFER_SIZE);
				char* token = strsep(&temp_path, ":");
				while(token != NULL) {
					strcat(try_path, token);
					strcat(try_path, "/");
					strcat(try_path, arg[0]);

					if(!access(try_path, X_OK)) {
						arg[0] = try_path;
						execv(arg[0], arg);
						break;
					}

					for(size_t i = 0; i < BUFFER_SIZE; i++) {
						try_path[i] = '\0';
					}
					token = strsep(&temp_path, ":");
				}
				fprintf(stderr, "%s\n", ERROR_MESSAGE);
				exit(-1);
			}
			// Continuing in the parent process
			// Waiting for the child process
			wait(NULL);
		}

		free(arg);
		for(size_t i = 0; i < BUFFER_SIZE; i++)
			prompt_start[i] = '\0';
		prompt = prompt_start;
	}

	free(prompt);
}
