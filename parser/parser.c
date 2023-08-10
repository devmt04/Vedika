/*
Current Progress:
	Detecting onlt Var declaration of type int.
*/



/*
EXAMPLE SYNTAX:
	int ID;
	ID = NUM_LIT;
	ID = ID + ID;

GRAMMER:
	program --> DeclList
	DeclList --> decls
	decls --> vardecl
	
	vardecl -->   type ID
				| type ID = data
				| type ID = exprs
				| ID = data
				| ID = exprs

	data --> (0-9)*

	exprs -->  ID + ID

	for ID = exprs, we have to calculate it like:
		
		ID = calc(exprs);
		func calc(exprs){
			if exprs == data:
				assign data to ID
			else if exprs == ID + ID:
				assign calc(ID+ID)

	}

Tree strcture is like:
	
			Program ---> MainTree
			  /\
			 /  \
	   TermNode NonTermNode        ------->
(Decide the             /\				|--->
type of subtree        /  \				|--->
i.e, vardecl)         /    \			|--->  SUBTREES
				TermND     NonTermNode	|--->
				   |		/\			|--->
				   V	   /  \			|--->
				 Leaves       ^
				 			 /|
				 			 /
							 |
							 V
						  Brances

each subtree will terminated with a /n(for statements) or ;(for blocks)

the 1st terminal node will decide the type of sub-tree, e.g
vardecl, fundecl, etc

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../lexer/lexer.h"
#include "parser.h"

ParseTree *int_var_decl();
ParseTree *var_init(Token *token);

/*
Our parser will fetch up the 1st token of an statement or declaration ot code block
from main program and then build parse-tree for whole block of code
in a single run.
*/

ParseTree *Parser(Token *token){
	switch (token->kind){
		case TK_KW:
			if(token->value == KW_INT){
				free(token);
				return int_var_decl();
			}

		case TK_ID:
			/* VAR ASSIGMENT or INITILISATION */
			return var_init(token);
		default:
			printf("No expected tokens, exiting\n");
			exit(-1);
			//more keyword starting from here according to grammer
		//case TK_ID:
	}
}


ParseTree *var_init(Token *token){
	char idName[strlen(token->name)+1];
	memcpy(idName, token->name, strlen(token->name)+1);
	free(token);

	/* Grammer: 
		varinit --> idName "=" data ("\n"|EOF) */

	/*
		Tree Strcutre:

	*/
	ParseTree *tree0 = (ParseTree *)malloc(sizeof(ParseTree));
	tree0->termnode = (Node *)malloc(sizeof(Node));
	tree0->termnode->TType = TREE_INITVAR;

	memcpy(tree0->termnode->name, idName, sizeof(idName));

	ParseTree *tree1 = (ParseTree *)malloc(sizeof(ParseTree));
	tree0->nontermnode = tree1;

	Token *nextToken = lexer();

	if(nextToken->kind == TK_ASOP_EQ){
		free(nextToken);
		tree1->termnode = (Node *)malloc(sizeof(Node));
		tree1->termnode->LType = LF_ASOP_EQ;
		
		ParseTree *tree2 = (ParseTree *)malloc(sizeof(ParseTree));
		tree1->nontermnode = tree2;

		nextToken = lexer();

		if(nextToken->kind==TK_INTLIT){ //or other datatype
			int value = nextToken->value;
			
			switch(nextToken->kind){
			
				case TK_INTLIT:
					free(nextToken);
					tree2->termnode = (Node *)malloc(sizeof(Node));
					tree2->termnode->LType = LF_INTLIT;
					tree2->termnode->value = value;
					break;
			}

			ParseTree *tree3 = (ParseTree *)malloc(sizeof(ParseTree));
			tree2->nontermnode = tree3;

			nextToken = lexer();
			if(nextToken->kind == TK_NEWLINE || nextToken->kind == TK_EOF){
				free(token);
				tree3->termnode = (Node *)malloc(sizeof(Node));
				tree3->termnode->LType = LF_DECL_TERM;
				tree3->nontermnode = NULL;

				return tree0;
			}else{
				// expr
				printf("expecting a newline charct at the end of expr");
			}

		}else{
			printf("Invalid token type after '='");
		}

	}else{
		printf("Syntax error, expecting a '=' after name of identifier");
	}

}




ParseTree *int_var_decl(){

	/*
				firstnode
				  /\
				 /  \
			  tree  tree2nd
					  /\
					 /  \
                       ....
                       ....
						/\
					   /  \
                          lastnode
							/\
						   /  \
					(/n or ;)  tree(for next declaration)

	*/

	/*
	TType and LLtype both are terminal nodes or leafs,
	only diffremce is TType represent whole tree of a code block
	or declaration.
	*/
	ParseTree *tree0 = (ParseTree *)malloc(sizeof(ParseTree));
	tree0->termnode = (Node *)malloc(sizeof(Node));
	tree0->termnode->TType = TREE_VARDECL;
	
	// there are 2 possible way according to our grammer
	// to declare variables that is
	// type ID | type ID = data

	tree0->termnode->value = DT_INT;
	ParseTree *tree1 = (ParseTree *)malloc(sizeof(ParseTree));
	tree0->nontermnode = tree1;

	//DataTypes datatype = DT_INT;
	Token *nextToken = lexer();

	if(nextToken->kind == TK_ID){
	
		char idName[strlen(nextToken->name)+1];
		memcpy(idName, nextToken->name, strlen(nextToken->name)+1);
		
		//strcpy(idName, nextToken->name);
		free(nextToken);

		tree1->termnode = (Node *)malloc(sizeof(Node));
		tree1->termnode->LType = LF_ID;
		//strcpy(tree1->termnode->name, idName);
		memcpy(tree1->termnode->name, idName, sizeof(idName));
		//tree1->termnode->name = idName;

		ParseTree *tree2 = (ParseTree *)malloc(sizeof(ParseTree));
		tree1->nontermnode = tree2;

		nextToken = lexer();

		if(nextToken->kind == TK_NEWLINE || nextToken->kind == TK_EOF){
			free(nextToken);
	
			tree2->termnode = (Node *)malloc(sizeof(Node));

			tree2->termnode->LType = LF_DECL_TERM;
			tree2->nontermnode = NULL;

			// return sub-ParseTree
			
			return tree0;

		}

		else if(nextToken->kind == TK_ASOP_EQ){

			free(nextToken);

			tree2->termnode = (Node *)malloc(sizeof(Node));
			tree2->termnode->LType = LF_ASOP_EQ;
			
			ParseTree *tree3 = (ParseTree *)malloc(sizeof(ParseTree));
			tree2->nontermnode = tree3;

			// next token may contain expr or data
			// if it has expr we have to handle 
			// that with other funtion

			nextToken = lexer();

			int data = nextToken->value;
			if(nextToken->kind == TK_INTLIT){
				free(nextToken);
			}else{
				printf("Error, expecting a intlit");
				exit(-1);
			}
			

			tree3->termnode = (Node *)malloc(sizeof(Node));
			tree3->termnode->LType = LF_INTLIT;
			tree3->termnode->value = data;

			ParseTree *tree4 = (ParseTree *)malloc(sizeof(ParseTree));
			tree3->nontermnode = tree4;

			nextToken = lexer();
			
			if(nextToken->kind == TK_NEWLINE || nextToken->kind == TK_EOF){
				free(nextToken);
				// it is NOT a expr
				// data = token value is confirmed

				// maybe we want to initilize each Node too?
				tree4->termnode = (Node *)malloc(sizeof(Node));
				tree4->termnode->LType = LF_DECL_TERM;
				tree4->nontermnode = NULL;

				return tree0;

			}else{
				printf("SYNTAX-ERROR : EXPECTING A NL CHARCTER");
				exit(-1);
			}
		}
		else{
			printf("Syntax error expecting a assigment operator after identifier or a new line charcter");
			exit(-1);
		}
	}else{
		printf("INVALID SYNTAX, EXPECTING A IDENTIFIER AFTER 'int'");
		exit(-1);
	}

}



/*

			
*/