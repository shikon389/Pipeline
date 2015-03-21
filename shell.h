#include <stdio.h>
#include <stdlib.h>

typedef struct Tokenizer{
	char* input;
	int current_pos;
	int length_input;
} tknzr;

extern tknzr* tokenizer;

int promptUser();
char** tokenize();

