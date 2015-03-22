#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"
#include <unistd.h>

tknzr *tokenizer;
int pipes[50][2];

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
		nullTerminateCommands();
		executeCommands();
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
	/*int x = 0;
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
	}*/
}

void nullTerminateCommands()
{
	int c = 0;

	for (; c < tokenizer->current_pos; c++){
		tokenizer->tokens_list[c]->tokens[tokenizer->tokens_list[c]->currentToken] = 0;
		tokenizer->tokens_list[c]->currentToken++;
	}
}

void executeCommands()
{
	int numCommands = tokenizer->current_pos;
	
	char ***cmds = (char ***) calloc(numCommands, sizeof(char **));
   	 
	int r = 0;
	for(; r < numCommands; r++){
		int numArgs = tokenizer->tokens_list[r]->currentToken;
		cmds[r] = (char **) calloc(numArgs, sizeof(char **));
		int s = 0;
		for(; s < numArgs; s++){
			cmds[r][s] = tokenizer->tokens_list[r]->tokens[s];
		}
	}

	free(cmds);
	
	piper(cmds, numCommands);
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
				tokenizer->nullTermInput[start_double_quote] = '\0';
				tokenizer->nullTermInput[i] = '\0';

				tokenizer->tokens_list[tokenizer->
						       current_pos]->tokens
				    [tokenizer->tokens_list
				     [tokenizer->current_pos]->currentToken] =
				    &tokenizer->nullTermInput[start_double_quote + 1];

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
				tokenizer->nullTermInput[start_single_quote] = '\0';
				tokenizer->nullTermInput[i] = '\0';
				
				tokenizer->tokens_list[tokenizer->
						       current_pos]->tokens
				    [tokenizer->tokens_list
				     [tokenizer->current_pos]->currentToken] =
				    &tokenizer->nullTermInput[start_single_quote + 1];

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

			// printf("entered whitespace statement. (command %d)\n", tokenizer->current_pos);

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
	} else {
		tokenizer->current_pos++;
	}
	
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

void resetTokenizer()
{
	int b = 0;
	for (; b < 50; b++) {
		tokenizer->tokens_list[b]->currentToken = 0;
	}

	tokenizer->length_input = 0;
	tokenizer->current_pos = 0;
}

int main(int argc, char **argv)
{
	initTokenizer();

	while (promptUser()) {
		sleep(2);
	}

	printf("bye\n");
	free(tokenizer);
	return 0;
}
