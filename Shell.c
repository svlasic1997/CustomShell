#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

int stringSplit(char* input, char** parts);
void waitBackground();


// Method to split string 
int stringSplit(char* input, char** parts){

	int index = 0;
	char* part = strtok(input, " ");
	while(part != NULL){
		parts[index] = part;
		index++;
		part = strtok(NULL, " ");
	}
	parts[index] = NULL;
	return index;
}

// Method for background executions
void waitBackground(){
	int status;
	int child2 = waitpid(-1, &status, WNOHANG);
	while(child2 > 0){
		printf(">>>[%d] Exit %d\n", child2, status);
		child2 = waitpid(-1, &status, WNOHANG);
	}
	
}


int main(int argc, char** argv){

	char input[200];
	char cwd[256];
	char* prompt; // Variable for the prompt
	char** split = malloc(sizeof(char*) * 100);
	int parts;

	// Checks for the prompt input 
	if(argc > 1 && strcmp(argv[1], "-p") == 0){
		prompt = argv[2];
	}
	
	while(1){
		// If-else statements print the prompt for either user input or default 308sh>
		if(prompt != NULL){
			printf("%s> ", prompt);
		}
		else{
			printf("308sh> ");
		}
 		
		// Gets the input of the user
		fgets(input, 200, stdin);
		
		// Check for empty command 
		waitBackground();

		// Special case if nothing is entered for input 
		if(input[0] == '\n'){
			continue;
		}
	
		int len = strlen(input);
		input[len-1] = '\0';
		parts = stringSplit(input, split);
		
		// Statements to print the pwd 
		if(strcmp(input, "pwd") == 0){
			if(getcwd(cwd, sizeof(cwd)) != NULL){
				printf("%s\n", cwd);
			}
		}
		
		// Statements for cd when cd has no other arguments
		else if(strncmp(input, "cd", 2) == 0){

			// When cd has no arguments, it goes back to user's home directory 
			if(split[1] == NULL){
				char* cdvar = getenv("HOME");
				chdir(cdvar);
			}
			
			// Otherwise go to the path directory
			else{
				chdir(split[1]);
			}

		}
		
		// Prints the PID number
		else if(strcmp(input, "pid") == 0){
			printf("The pid is %d\n", getpid());
		}

		// Prints the PPID number
		else if(strcmp(input, "ppid") == 0){
			printf("The ppid is %d\n", getppid());
		}
			
		// Exits the program 
		else if(strcmp(input, "exit") == 0){
			return 0;
		}
		
		else{
			// Fork before he execl so that the child gets killed not the parent
			int i = 0, child,status;
			// Checks to see if the process is in the background
			waitBackground();
			
			// Checks to see if & is used for background process
			if(strcmp(split[parts-1], "&") == 0){
				split[parts-1] = '\0';
				i = 1;
			}
		
			child = fork(); 
			// Add error to kill child if command is not recognized
			if(child == 0){
				printf(">>>[%d] %s\n" , getpid(), input);
				execvp(split[0], &split[0]);
				perror("Error");
				exit(1);
			}
			
			// Prints the exit status of the child process 
			else{
				usleep(1000);
				if(i == 0){
					waitpid(child, &status, 0); 
					printf(">>>[%d] %s Exit %d\n", child, input, status);
				}
				else{
					waitBackground();
				}
			}
		}

	}

}
