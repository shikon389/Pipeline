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
	printf("$ ");
	char prompt_input[400];
	scanf("%s", prompt_input);
	tokenizer->input = prompt_input;
	tokenizer->length_input = strlen(prompt_input);
	printf("%s\n", prompt_input);

	if (strcmp(prompt_input, "quit")) {
		printf("if %s\n", prompt_input);
		return 1;
	} 
	else {
		printf("else %s\n", prompt_input);
		tokenize();
		return 0;
	}
}

char **tokenize()
{

	int start_double_quote = -1;
	int start_single_quote = -1;
	int i = 0;
	int start_token = 0;

	for (; i < tokenizer->length_input; i++) {
		if (tokenizer->input[i] == '\"') {
			if (start_double_quote >= 0) {

				int token_length = i - start_double_quote + 1;
				char strbuff[token_length];
				strncpy(strbuff,
					tokenizer->input + start_double_quote,
					token_length);
				tokenizer->tokens_list[tokenizer->
						       current_pos]->tokens
				    [tokenizer->tokens_list
				     [tokenizer->current_pos]->currentToken] =
				    strbuff;

				tokenizer->tokens_list[tokenizer->
						       current_pos]->currentToken++;
				start_single_quote = -1;
				start_double_quote = -1;
			} else {
				start_double_quote = i;
			}
		} else if (tokenizer->input[i] == '\'') {
			if (start_single_quote >= 0) {

				int token_length = i - start_single_quote + 1;
				char strbuff[token_length];
				strncpy(strbuff,
					tokenizer->input + start_single_quote,
					token_length);
				tokenizer->tokens_list[tokenizer->
						       current_pos]->tokens
				    [tokenizer->tokens_list
				     [tokenizer->current_pos]->currentToken] =
				    strbuff;

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

			int token_length = i - start_token;
			char strbuff[token_length];
			strncpy(strbuff, tokenizer->input + start_token,
				token_length);
			tokenizer->tokens_list[tokenizer->
					       current_pos]->tokens
			    [tokenizer->tokens_list
			     [tokenizer->current_pos]->currentToken] = strbuff;

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

		}

	}

	return NULL;
}

void initTokenizer()
{
	tokenizer = (tknzr *) malloc(sizeof(tknzr));
	tokenizer->current_pos = 0;

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














int main(int argc, char **argv)
{
	int pid, status;
	int fd1[2];
	int fd2[2];


	pipe(fd1);
	pipe(fd2);

	runfirst(fd1);

	runmiddle(fd1, fd2);

	close(fd1[0]); 
	close(fd1[1]); /*this is important! close both file descriptors on the pipe */

	runlast(fd2);


	close(fd2[0]);
	close(fd2[1]);

	while ((pid = wait(&status)) != -1)	/* dead kids */
		fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
	exit(0);







	initTokenizer();

	while (promptUser()) {
	}

	printf("bye\n");
	free(tokenizer);
	return 0;
}
