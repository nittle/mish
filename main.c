#include<stddef.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<ctype.h>

#define BUFFER_SIZE 256
#define ERROR_MESSAGE "An error occurred"
#define INITIAL_PATH "/bin"

// Contains the path variable
char* path;

typedef struct {
	char** args;
	size_t argCount;
	char* output;
} CommandCall;

// Functions declaration
void tryExecutable(char** arg, size_t argCount);
void commandPath(char** arg, size_t argCount);
void commandExit(char** arg, size_t argCount);
size_t parseArgs(char* prompt, char** arg, char* output);
size_t parsePrompt(char* prompt, CommandCall* commands);
void handleCommand(CommandCall* command);

int main() {
	char* prompt = (char*)calloc(BUFFER_SIZE, sizeof(char));

	path = (char*)calloc(BUFFER_SIZE, sizeof(char));
	strcpy(path, INITIAL_PATH);

	while(1) {
		// Get prompt from the user and parse args
		fprintf(stdout, "%s", "mish> ");
		fgets(prompt, BUFFER_SIZE, stdin);
		
		CommandCall commandsCalled[BUFFER_SIZE];
		size_t commandsCount = parsePrompt(prompt, commandsCalled);
		
		for(size_t i = 0; i < commandsCount; i++) {
			handleCommand(&commandsCalled[i]);
		}

		while(wait(NULL) > 0);

		// Free the args memory
		for(size_t i = 0; i < commandsCount; i++) {
			free(commandsCalled[i].args);
			free(commandsCalled[i].output);
		}

		// Clear the prompt variable
		for(size_t i = 0; i < BUFFER_SIZE; i++)
			prompt[i] = '\0';
	}

	free(prompt);
	free(path);
}

void handleCommand(CommandCall* command) {
	// Create a copy of current stdout
	int stdout_copy = dup(1);

	char** arg = command->args;
	size_t argCount = command->argCount;
	
	// command->output is not `stdout`
	if(strcmp(command->output, "stdout"))
		freopen(command->output, "w", stdout);

	// Check for the blank command
	if (arg[0] == NULL) {
		// Do nothing
	} else if(!strcmp(arg[0], "exit")) { // Exit command
		commandExit(arg, argCount);
	} else if(!strcmp(arg[0], "path")) { // Path command
		commandPath(arg, argCount);
	} else { // Run an executable
		tryExecutable(arg, argCount);
	}

	// Flush stdout before restoring from copy
	fflush(stdout);
	// Restore stdout from copy
	dup2(stdout_copy, 1);
}

size_t parsePrompt(char* prompt, CommandCall* commands) {
	size_t lastCommand = 0;
	char* token = strsep(&prompt, "&");
	while(token != NULL) {
		char** args = calloc(sizeof(char), BUFFER_SIZE);
		char* output = calloc(sizeof(char), BUFFER_SIZE);
		size_t argCount = parseArgs(token, args, output);
		commands[lastCommand].argCount = argCount;
		commands[lastCommand].args = args;
		commands[lastCommand].output = output;
		lastCommand++;
		
		token = strsep(&prompt, "&");
	}

	return lastCommand;
}

size_t parseArgs(char* prompt, char** arg, char* output) {
	prompt[strcspn(prompt, "\n")] = '\0'; // Replace the endline character with terminator
	size_t lastArg = 0;

	char* left_expr = strsep(&prompt, ">");
	char* token = strsep(&left_expr, " ");
	while(token != NULL) {
		if(*token != '\0') {
			arg[lastArg++] = token;
		}

		token = strsep(&left_expr, " ");
	}
	
	// It is the part behind the ">" sign that `prompt` points to
	if(prompt) {
		while(isspace(*prompt))
			prompt++;

		char* end = prompt + strlen(prompt) - 1;
		while(end > prompt && isspace(*end))
			end--;
		end[1] = '\0';

		strcpy(output, prompt);
	} else {
		strcpy(output, "stdout");
	}

	return lastArg;
}

void commandExit(char** arg, size_t argCount) {
	fprintf(stdout, "%s\n", "Exitting...");
	exit(1);
}

void commandPath(char** arg, size_t argCount) {
	if(argCount > 1) {
		for(size_t i = 0; i < BUFFER_SIZE; i++) {
			path[i] = '\0';
		}
		for(size_t i = 1; i < argCount - 1; i++) {
			strcat(path, arg[i]);
			strcat(path, ":");
		}
		strcat(path, arg[argCount - 1]);
		fprintf(stdout, "New PATH: %s\n", path);
	} else {
		fprintf(stdout, "PATH is: %s\n", path);
	}
}

void tryExecutable(char** arg, size_t argCount) {
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
}
