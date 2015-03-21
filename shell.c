#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

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
		return 1;
	} else {
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

int main(int argc, char **argv)
{
	initTokenizer();

	while (promptUser()) {
	}

	printf("bye\n");
	free(tokenizer);
	return 0;
}
