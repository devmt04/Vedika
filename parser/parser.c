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
#include "../symtable/symtable.h"

extern SymbolTable *table; //global symbol table

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
			// uninitialized value in symbol table
			// currently we are in global scope so
			// symbol gonna add in global scope only
			// global symbol table
			free(nextToken);

			addSymbol(table, idName, VAR_ID, DT_NA); //?

			tree2->termnode = (Node *)malloc(sizeof(Node));

			tree2->termnode->LType = LF_NEWLINE;
			tree2->nontermnode = NULL;

			// return sub-ParseTree
			
			//TreeReturnNode *returnNode = (TreeReturnNode *)malloc(sizeof(TreeReturnNode));
			//returnNode->firstnode = tree0;
			//returnNode->tree2nd = tree1;
			//returnNode->lastnode = tree2;


			return tree0;

		}else if(nextToken->kind == TK_ASOP_EQ){

			free(nextToken);

			tree2->termnode = (Node *)malloc(sizeof(Node));
			tree2->termnode->LType = LF_ASOP_EQ;
			
			ParseTree *tree3 = (ParseTree *)malloc(sizeof(ParseTree));
		//	tree2->nontermnode = (ParseTree *)malloc(sizeof(ParseTree));
			tree2->nontermnode = tree3;

			// next token may contain expr or data
			// if it has expr we have to handle 
			// that with other funtion

			nextToken = lexer();
			if(nextToken->kind == TK_INTLIT){
				int data = nextToken->value;
				free(nextToken);
			}else{
				printf("Error, expecting a intlit");
				exit(-1);
			}
			

			tree3->termnode = (Node *)malloc(sizeof(Node));
			tree3->termnode->LType = LF_INTLIT;
			ParseTree *tree4 = (ParseTree *)malloc(sizeof(ParseTree));
			tree3->nontermnode = tree4;

			nextToken = lexer();
			if(nextToken->kind == TK_NEWLINE || nextToken->kind == TK_EOF){
				free(nextToken);
				// it is NOT a expr
				// data = token value is confirmed
				// save datatype, idName, data it holds
				// in symbol table
				addSymbol(table, idName, VAR_ID, DT_INT);

				// maybe we want to initilize each Node too?
				tree4->termnode = (Node *)malloc(sizeof(Node));
				tree4->termnode->LType = LF_NEWLINE;
				tree4->nontermnode = NULL;
				
				//TreeReturnNode *returnNode = (TreeReturnNode *)malloc(sizeof(TreeReturnNode));
				//returnNode->firstnode = tree0;
				//returnNode->lastnode = tree4;

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
			break;
		default:
			printf("No expected tokens, exiting\n");
			exit(-1);
			//more keyword starting from here according to grammer
		//case TK_ID:
	}
}