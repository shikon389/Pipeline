#include <stdio.h>
#include <stdlib.h>

typedef struct Tokens_list {
	char *tokens[1024];
	int currentToken;
} tkns_list;

typedef struct Tokenizer{
	char* input;
	char* nullTermInput;
	struct Tokens_list *tokens_list[50];
	int current_pos; //command #
	int length_input;
} tknzr;

extern tknzr* tokenizer;
extern int pipes[50][2];

int promptUser();
void nullify(char *string);
char* trimwhitespace(char *str);
void printTokens();
void nullTerminateCommands();
void executeCommands();
void removeNullTerminators(int start, int end);
int tokenize();
void initTokenizer();
void resetTokenizer();

int piper(char **cmds[], int numCommands);
