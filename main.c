/*
 * tokenizer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */

struct TokenizerT_ {
	char* mainString;
	int ctpos;
	int totalpos; 
	char* currenttoken[200];
};

typedef struct TokenizerT_ TokenizerT;

/*
 * TKCreate creates a new TokenizerT object for a given set of separator
 * characters (given as a string) and a token stream (given as a string).
 * 
 * TKCreate should copy the two arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate(char *ts) {
	TokenizerT* tempToken = (TokenizerT*)malloc(sizeof(TokenizerT));
	tempToken->mainString = ts;
	tempToken->ctpos = 0;
	int eq;  
	char* temphold = (char*)malloc(sizeof(char)*1024); //Holds the token in question.
	char* temphold2 = (char*)malloc(sizeof(char)*1024);
	int th = 0; // Keeps track of character position in temphold.
	int tspos; // Keeps track of character position in ts.
	int tempctpos = 0; // Keeps track of spots filled by tokens in currenttoken.
	int sepconf = 0;	

	for(tspos = 0; tspos  < strlen(ts); tspos++){ //runs through each character of the token string 
		if(ts[tspos] != ' '){ //found no seperator
		temphold[th] = ts[tspos]; //adds char to temphold
		th++;
		}
		else if(sts[tspos] != ' ' & strcmp(temphold, temphold2) == 0){
		} 
		else{
		temphold[th] = 0;
		tempToken->currenttoken[tempctpos] = (char*)malloc(sizeof(char)*1024);
		strcpy(tempToken->currenttoken[tempctpos], temphold); //copies token into an element in currenttoken[]
		strcpy(temphold2, temphold); //used in else if statement to avoid error
		tempctpos++; //moves to next position in currenttokenp[]
		sepconf = 0; //resetting variable
		th = 0; //resetting variable
		}	
	}
	temphold[th] = 0;
	tempToken->currenttoken[tempctpos] = (char*)malloc(sizeof(char)*1024);
	strcpy(tempToken->currenttoken[tempctpos], temphold);
	tempctpos++; 
	tempToken->totalpos = tempctpos;
	return tempToken;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy(TokenizerT *tk) {
	
//	free(tk->currenttoken);
	free(tk);	
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken(TokenizerT *tk) {
	char* nextToken = (char*)malloc(sizeof(char)*1024);
	nextToken = tk->currenttoken[tk->ctpos];
	tk->ctpos++;

  return nextToken;
}

/*
 * main will have two string arguments (in argv[1] and argv[2]).
 * The first string contains the separator characters.
 * The second string contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {
	char* sep;
	char* ts;
	sep = argv[1];
	ts = argv[2];
	int count1;
	int count2;
	char* token = (char*)malloc(sizeof(char)*1024);
	TokenizerT* mainToken;// = (TokenizerT*)malloc(sizeof(TokenizerT));
	mainToken = TKCreate(sep, ts);
	//printf("End TKCreate\n");
	for(count1 = 0; count1 < mainToken->totalpos; count1++){
		token = TKGetNextToken(mainToken);
		char* tempbeg= (char*) malloc(sizeof(char)*1024); //Stores beginning of token	
		for (count2=0; count2 < strlen(token); count2++){ //For loop used to check if there is an escape sequence in the token.
			if (token[count2]=='\\' && count2< (strlen(token)-1)){ //Checks to see if there is a backslash in the token and that it isn't the last char.
				char c=token[count2+1];
				if (c=='n'){
					printf("[0x%.2x]",c);	
					count2++;
				} else if (c=='t'){
					printf("[0x%.2x]",c);
					count2++;
				} else if (c=='v'){
					printf("[0x%.2x]",c);
					count2++;
				} else if (c=='b'){
					printf("[0x%.2x]",c);
					count2++;
				} else if (c=='r'){
					printf("[0x%.2x]",c);
					count2++;
				} else if (c=='f'){
					printf("[0x%.2x]",c);
					count2++;
				} else if (c=='a'){
					printf("[0x%.2x]",c);
					count2++;
				} else if (c=='\\'){
					printf("[0x%.2x]",c);
					count2++;
				} else if (c=='"'){
					printf("[0x%.2x]",c);
					count2++;
				}
					
			} 
				

		}

		//printf("%s\n", token);	
	}

	TKDestroy(mainToken);
  return 0;
}
