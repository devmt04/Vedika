#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "symtable/symtable.h"

int lineno = 1;
int linepos = 0;

FILE *fp = NULL;
SymbolTable *table = NULL;	//global symbol table

void init_symtable(){
	table = createSymbolTable();
	enterScope(table);	//Global scope
}

int main(int argc, char *argv[]){

	char *filename = argv[1];
  	FILE *f = fopen(argv[1], "r");

  	if(f == NULL){
	    //ERROR
	    printf("File not found!");
	    return -1;
  	}else{
  		fp = f;
  	}

  
  	init_symtable();

  	ParseTree *ParentTree = (ParseTree *)malloc(sizeof(ParseTree));
  	Token *token = lexer();

	if(token->kind == TK_EOF){
		free(token);
		printf("EOF, exiting");
		return 0;
	}

  	TreeReturnNode *subtree = Parser(token);
	ParentTree->termnode = subtree->firstnode->termnode;
	ParentTree->nontermnode = subtree->firstnode->nontermnode;

	while(1){
		
		Token *token = lexer();

		if(token->kind == TK_EOF){
			free(token);
			printf("EOF, exiting");
			break;
		}

		ParseTree *lastNode = (ParseTree *)malloc(sizeof(ParseTree));
  		lastNode = subtree->lastnode;

		TreeReturnNode *subtree = Parser(token);
		lastNode->nontermnode = subtree->firstnode;
	}

	fclose(f);
	return 0;
}

