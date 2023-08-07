#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"

int lineno = 1;
int linepos = 0;

FILE *fp = NULL;

int main(int argc, char *argv[]){
	printf("%d--",sizeof('\t'));
	char *filename = argv[1];
  	FILE *f = fopen(argv[1], "r");

  	if(f == NULL){
	    //ERROR
	    printf("File not found!");
	    return -1;
  	}else{
  		fp = f;
  	}

  	while (1){
  
  		Token *token = lexer();
		if(token->kind == TK_EOF){
			printf("EOF reached");
			free(token);
			break;
		}else{
			printf("Token kind : %d\n", token->kind);
			if(token->kind == TK_INTLIT){
				printf("Intlit value: %d\n", token->value);
				printf("linepos: %d\n", token->linepos);
				printf("lineno: %d\n\n", token->lineno);
			}else if(token->kind == TK_KW || token->kind==TK_ID){
				printf("Token name: %s\n", token->name);
				printf("linepos: %d\n", token->linepos);
				printf("lineno: %d\n\n", token->lineno);
			}
			free(token);
		}
	}

	fclose(f);

	return 0;
}
