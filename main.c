#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main() {
	char* prompt = (char*)malloc(256 * sizeof(char));
	while(1) {
		fprintf(stdout, "%s", "wish> ");
		fgets(prompt, 256, stdin);
		fprintf(stdout, "%s", prompt);
	}
	free(prompt);
}
