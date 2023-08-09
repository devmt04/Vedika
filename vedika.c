#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "symtable/symtable.h"
#include "parser/parser.h"
#include "ircodegen/ircodegen.h"

int lineno = 1;
int linepos = 0;

FILE *fp = NULL;
//char filename[128] = {0};
SymbolTable *table = NULL;	//global symbol table

void init_symtable(){
	table = createSymbolTable();
	enterScope(table);	//Global scope
}

int main(int argc, char *argv[]){
	
	//strcpy(filename, argv[1]);

  	FILE *f = fopen(argv[1], "r");

  	if(f == NULL){
	    //ERROR
	    printf("File not found!");
	    return -1;
  	}else{
  		fp = f;
  	}

  
  	init_symtable();

	while(1){

		Token *token = lexer();
		if(token->kind == TK_EOF){
			free(token);
			printf("EOF, exiting");
			break;
		}

		ParseTree *tree = Parser(token);
		/* IR CODEGEN STARTS FROM HERE */
  		IRGen(tree);
	}

	fclose(f);
	return 0;
}