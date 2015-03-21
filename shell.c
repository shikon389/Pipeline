#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

tknzr* tokenizer;

/**
 *
 * This method will prompt the user for input and it will store
 * the input string within the tokenizer instance's input field.
 *
 **/
int promptUser(){
	printf("$ ");
	char prompt_input[400];
	scanf("%s", prompt_input);
	tokenizer->input = prompt_input;
	tokenizer->current_pos = 0;
	tokenizer->length_input = strlen(prompt_input);
	printf("%s\n", prompt_input);
	
	if(strcmp(prompt_input, "quit")){
		return 1;
	}else{
		tokenize();
		return 0;
	}
}

char** tokenize(){
	char* tokens[1024];
	
	int start_double_quote = -1;
	int start_single_quote = -1;
	int i = 0;
	int start_token = 0;

	for(; i < tokenizer->length_input; i++){
		if(tokenizer->input[i] == '\"'){
			if(start_double_quote >= 0){
				//take this subset of the string and store within the tokens array
				
				int token_length = i - start_double_quote;
				char strbuff[token_length];
				strncpy(strbuff, tokenizer->input + start_double_quote, token_length);
				tokens[tokenizer->current_pos] = strbuff;
				
				tokenizer->current_pos++;
				start_single_quote = -1;
				start_double_quote = -1;
			}else{
				start_double_quote = i;
			}
		}else if(tokenizer->input[i] == '\''){
			if(start_single_quote >= 0){
				
				int token_length = i - start_single_quote;
				char strbuff[token_length];
				strncpy(strbuff, tokenizer->input + start_single_quote, token_length);
				tokens[tokenizer->current_pos] = strbuff;
				
				tokenizer->current_pos++;
				start_single_quote = -1;
				start_double_quote = -1;
			}else{
				start_single_quote = i;
			}
		}else if(tokenizer->input[i] == ' ' && start_double_quote == -1 && start_single_quote == -1){
			int token_length = i - start_token;
			char strbuff[token_length];
			strncpy(strbuff, tokenizer->input + start_token, token_length);
			tokens[tokenizer->current_pos] = strbuff;
				
			tokenizer->current_pos++;
			start_token = i + 1;
		}else if(tokenizer->input[i] == '|'){
			
		}


	}
	
	return NULL;
}

int main(int argc, char** argv){
	tokenizer = (tknzr*) malloc(sizeof(tknzr)); 
	
	while(promptUser()){
	}
	
	printf("bye\n");
	free(tokenizer);
	return 0;
}
