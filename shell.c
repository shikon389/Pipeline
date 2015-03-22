#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include <unistd.h>

tknzr *tokenizer;

/**
 *
 * This method will prompt the user for input and it will store
 * the input string within the tokenizer instance's input field.
 *
 **/
int promptUser()
{
	resetTokenizer();
	
	printf("$ ");
	char prompt_input[400];
	fgets(prompt_input, 400, stdin);
	tokenizer->input = trimwhitespace(prompt_input);
	
	strcpy(tokenizer->nullTermInput, trimwhitespace(prompt_input));
	nullify(tokenizer->nullTermInput);
	tokenizer->length_input = strlen(prompt_input);

	if (!strcmp(tokenizer->input, "quit")) {
		return 0;
	} else {
		tokenize();
		printTokens();
		return 1;
	}
}

void nullify(char *string)
{
	int i = 0;
	int orig_length = strlen(string);

	for(; i < orig_length; i++){
		if(string[i] == ' '){
			string[i] = '\0';
		}
	}

}

char *trimwhitespace(char *str)
{
	char *end;

	// Trim leading space
	while (isspace(*str))
		str++;

	if (*str == 0) // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && isspace(*end))
		end--;

	// Write new null terminator
	*(end + 1) = 0;

	return str;
}

void printTokens()
{
	int x = 0;
	int y = 0;
	printf("# of commands: %d\n", tokenizer->current_pos);
	printf("# of tokens that make up first command: %d\n",
	       tokenizer->tokens_list[0]->currentToken);

	for (; x < tokenizer->current_pos; x++) {
		printf("\nCommand %d: %d tokens\n--------\n", x, tokenizer->tokens_list[x]->currentToken);
		for (; y < tokenizer->tokens_list[x]->currentToken; y++) {
			printf("%s\n", tokenizer->tokens_list[x]->tokens[y]);
		}
		y = 0;
	}
}

void removeNullTerminators(int start, int end){

	int m = start;
	for(; m < end; m++){
		if(tokenizer->input[m] == ' '){
			tokenizer->nullTermInput[m] = ' ';
		}
	}
	
}

/**
 * This does the main tokenizing. It will iterate through the string
 * character by character. On each character, it will check to see
 * if it is a double quote, single quote, white space, or pipeline character.
 *
 * It will store pointers of the tokens within tokens array contained in the
 * Tokens_list struct. The main for loop is looping through the normal input
 * while the pointers to the tokens are within the nullTermInput string. The
 * nullTermInput string is the same as tokenizer->input except each whitespace
 * character has been swapped with a '\0' character.
 *
 **/
void tokenize()
{
	int start_double_quote = -1;
	int start_single_quote = -1;
	int i = 0;
	int start_token = 0;

	for (; i < tokenizer->length_input; i++) {
		if (tokenizer->input[i] == '\"') {
			if (start_double_quote >= 0) {
				
				removeNullTerminators(start_double_quote, i);
				tokenizer->tokens_list[tokenizer->
						       current_pos]->tokens
				    [tokenizer->tokens_list
				     [tokenizer->current_pos]->currentToken] =
				    &tokenizer->nullTermInput[start_double_quote];

				tokenizer->tokens_list[tokenizer->
						       current_pos]->currentToken++;
				start_single_quote = -1;
				start_double_quote = -1;
				
			} else {
				start_double_quote = i;
			}
		} else if (tokenizer->input[i] == '\'') {
			if (start_single_quote >= 0) {

				removeNullTerminators(start_single_quote, i);
				tokenizer->tokens_list[tokenizer->
						       current_pos]->tokens
				    [tokenizer->tokens_list
				     [tokenizer->current_pos]->currentToken] =
				    &tokenizer->nullTermInput[start_single_quote];

				tokenizer->tokens_list[tokenizer->
						       current_pos]->currentToken++;
				start_single_quote = -1;
				start_double_quote = -1;
			} else {
				start_single_quote = i;
			}
		} else if (tokenizer->input[i] == ' '
			   && tokenizer->input[i - 1] != ' '
			   && start_double_quote == -1
			   && start_single_quote == -1
			   && tokenizer->input[i - 1] != '\"'
			   && tokenizer->input[i - 1] != '\'') {
			// What if user types in: ls    |      wc -l, hence tokenizer->input[i-1] != ' '

			printf("entered whitespace statement. (command %d)\n", tokenizer->current_pos);
			tokenizer->tokens_list[tokenizer->current_pos]->tokens
			    [tokenizer->tokens_list
			     [tokenizer->current_pos]->currentToken] = &tokenizer->nullTermInput[start_token];

			tokenizer->tokens_list[tokenizer->
					       current_pos]->currentToken++;
			while (i++ < tokenizer->length_input) {
				if (tokenizer->input[i] != ' ') {
					start_token = i;
					break;
				}
				// Looping until we hit a non-whitespace character
			}

			i--;	// decrementing by one since we have encountered a character 
		} else if (tokenizer->input[i] == '|') {
			//Do NOT consider | as a token

			tokenizer->current_pos++;
			while (i++ < tokenizer->length_input) {
				if (tokenizer->input[i] != ' ') {
					start_token = i;
					break;
				}
				// Looping until we hit a non-whitespace character
			}

			i--;
		} else if (tokenizer->input[i] == ' ') { 
			// Not a token
			while (i++ < tokenizer->length_input) {
				if (tokenizer->input[i] != ' ') {
					start_token = i;
					break;
				}
				// Looping until we hit a non-whitespace character
			}

			i--;
		}
	}

	if (tokenizer->input[i-1] != '\'' && tokenizer->input[i - 1] != '\"'){
	tokenizer->tokens_list[tokenizer->current_pos]->tokens[tokenizer->
							       tokens_list
							       [tokenizer->
								current_pos]->
							       currentToken] =
	    &tokenizer->nullTermInput[start_token];
	tokenizer->tokens_list[tokenizer->current_pos]->currentToken++;
	tokenizer->current_pos++;
	}
	
	printf("Reached end of tokenize\n");
}

/**
 * Initializes the tokenizer variable and sets its fields
 * to their proper initial values.
 **/
void initTokenizer()
{
	tokenizer = (tknzr *) malloc(sizeof(tknzr));
	tokenizer->current_pos = 0;
	tokenizer->nullTermInput = malloc(sizeof(char) * 400);

	int z = 0;
	for (; z < 50; z++) {
		tokenizer->tokens_list[z] =
		    (struct Tokens_list *)malloc(sizeof(struct Tokens_list));
		tokenizer->tokens_list[z]->currentToken = 0;
	}
}

/* Megan attempts to execute the tokenized commands
*
* Three cases:  First item
*				Middle Item(s)
*				Last Item
*
* Whatever is written to fd[1] will be read from fd[0].
* 			[(0) INPUT]  [(1) OUTPUT]
*/
//char *cmds[][] = { {"ls", 0}, {"find", "main.c", 0}, { "find", "shell.c", 0}};
char *cmd1[] = { "ls", 0 };
char *cmd2[] = { "grep", "s", 0 };
char *cmd3[] = { "grep", ".c", 0 };

//right now this runs first, middle, and last.
//this needs to be set up in a way that:
//	if args = 1, only run last
//  if args = 2, only run first and last
//  if args >= 3, run first, then run middle until there is only one arg left, then run last on it

void runfirst(int pfd1[]);
void runmiddle(int pfd1[], int pfd2[]);
void runlast(int pfd[]);

void
runfirst(int pfd1[])	/* run the first part of the pipeline, cmd1 */
{
	int pid;	/* we don't use the process ID here, but you may wnat to print it for debugging */

	switch (pid = fork()) {

	case 0: /* child */
		//dup2(pfd[0], 0);  /*Put the first command here just to print it to the screen */
		//execvp(cmd1[0], cmd1);  /*Run it so we know what to print*/

		dup2(pfd1[1], 1);	/* now we actually put it where we need it to be */
		close(pfd1[0]); 		/* this process don't need the other end */
		close(pfd1[1]);
		execvp(cmd1[0], cmd1);	/* run the command */
		perror(cmd1[0]);	/* if it failed */
		break;

	default: /* parent does nothing */
		//printf("%d\n", pid );
		break;

	case -1:
		perror("fork");
		exit(1);
	}
}

void
runmiddle(int pfd1[], int pfd2[])	/* run the first part of the pipeline, cmd2 */
{
	int pid;	/* we don't use the process ID here, but you may wnat to print it for debugging */

	switch (pid = fork()) {

	case 0: /* child */
		//dup2(pfd[0], 0);  /*Put the first command here just to print it to the screen */
		//execvp(cmd2[0], cmd2);  /*Run it so we know what to print*/

		dup2(pfd1[0], 0);	/* now we actually put it where we need it to be */
		dup2(pfd2[1], 1);
		close(pfd1[0]);
		close(pfd1[1]);
		close(pfd2[0]);
		close(pfd2[1]);
		execvp(cmd2[0], cmd2);	/* run the command */
		perror(cmd2[0]);	/* if it failed */
		break; 

	default: /* parent does nothing */
		//printf("%d\n", pid );
		break;

	case -1:
		perror("fork");
		exit(1);
	}
}

void
runlast(int pfd[])	/* last thing in the pipeline (cmd3) */
{
	int pid;

	switch (pid = fork()) {

	case 0: /* child */
		dup2(pfd[0], 0);	/* this end of the pipe becomes the standard input */
		close(pfd[0]);
		close(pfd[1]);		/* this process doesn't need the other end */
		execvp(cmd3[0], cmd3);	/* run the command */
		perror(cmd3[0]);	/* if it failed! */
		break;

	default: /* parent does nothing */
		//printf("%d\n", pid );
		break;

	case -1:
		perror("fork");
		exit(1);
	}
}

void resetTokenizer()
{
	int b = 0;
	int c = 0;
	for (; b < 50; b++) {
		tokenizer->tokens_list[b]->currentToken = 0;
		for (; c < 1024; c++) {
			tokenizer->tokens_list[b]->tokens[c] = '\0';
		}
		c = 0;
	}

	tokenizer->length_input = 0;
	tokenizer->current_pos = 0;
}

int main(int argc, char **argv)
{
/*	int pid, status;
	int fd1[2];
	int fd2[2];


	pipe(fd1);
	pipe(fd2);

	runfirst(fd1);

	runmiddle(fd1, fd2);

	close(fd1[0]); 
	close(fd1[1]); //this is important! close both file descriptors on the pipe 

	runlast(fd2);


	close(fd2[0]);
	close(fd2[1]);

	while ((pid = wait(&status)) != -1)	// dead kids 
		fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
	exit(0);
*/

	initTokenizer();

	while (promptUser()) {
	}

	printf("bye\n");
	free(tokenizer);
	return 0;
}
