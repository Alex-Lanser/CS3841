/*************************************
 * 
 * Class: CS3840 - YOUR SECTION HERE
 * 
 * Name(s): YOUR NAME(S) HERE
 * 
 * Lab: Teeny Tiny Shell
 *
 *************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define INPUT_MAX 256
#define CMD_MAX 5

/* read_cmd_string()
 *     Reads a line of text from the user
 *         Parameters:  dest - target location for the input
 *         Returns: int - 0 on success or 
 *                        -1 on error
 */
int read_cmd_string(char dest[INPUT_MAX])
{
	// Read user input
	if(fgets(dest, INPUT_MAX, stdin) == NULL) {
		fprintf(stderr, "Unable to read user input\n");
		return -1;
	}

	// Remove trailing return character
	int len = strlen(dest);
	if(dest[len-1] == '\n') {
		dest[len - 1] = '\0';
	}

	return 0;
}

/* parse_commands()
 *     Reads a line of text from the user
 *         Parameters:  input - string containing user input
 *                      cmd_strs - the target array for command strings
 *         Returns: int - The number of commands found or
 *                        -1 on error
 */
int parse_commands(char input[INPUT_MAX], char cmd_strs[CMD_MAX][INPUT_MAX])
{
	// Chop the input into command strings
	int cmd_count = 0;
	char* cmd_ptr = strtok(input, ";");
	while(cmd_ptr) {
		if(cmd_count >= CMD_MAX) {
			fprintf(stderr, "Too many commands\n");
			return -1;
		}
		strncpy(cmd_strs[cmd_count], cmd_ptr, INPUT_MAX);
		cmd_count++;
		cmd_ptr = strtok(NULL, ";");
	}

	return cmd_count;
}

int main()
{
	while(1){

		char user_input[INPUT_MAX];
		char cmd_strs[CMD_MAX][INPUT_MAX];

		// TODO need to be able to get input from
		//    the user in a loop

		// Print the input prompt
		printf("$> ");

		// Read user input
		if(read_cmd_string(user_input) == -1) {
			return 1;
		}

		//Quit command
		if(strcmp(user_input, "quit") == 0){
			return 0;
		}

		// Chop the input into command strings
		int cmd_count = parse_commands(user_input, cmd_strs);
		if(cmd_count == -1) {
			return 1;
		}

		// Chop the commands into arguments and execute one at a time
		for(int i = 0; i < cmd_count; i++) {
			//cmd_strs[i] is the seperated commands from ';'
			//printf("%s%s\n", "command:",cmd_strs[i]);
			// TODO: 
			//    1) Chop the command into command line arguments
			//        need to handle up to 10 arguments
			//        NOTE: the command name is always the first argument
			//    2) fork a process
			//    3) execute the command with execvp

			char *token[12];
			int x = 0;
			int argCount = 0;
			//Get command from full line
			token[argCount] = strtok(cmd_strs[i], " "); 
			//Separate the args
			while(token[argCount] != NULL){
				token[++argCount] = strtok(NULL, " ");
			}

			if(argCount > 10){
				printf("%s\n", "The argument count cannot exceed 10.");
			}
			else{
				if(fork() == 0){
				for(int v = 0; v < argCount; v++){
					execvp(token[0], token);
				}
			}
				else
					wait(NULL);
			}

		}
	}
	return 0;
}
