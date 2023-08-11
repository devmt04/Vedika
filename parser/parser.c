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

ParseTree *int_var_decl(Token *token);
ParseTree *var_init(Token *token);
ParseTree *func_decl(Token token);

/*
Our parser will fetch up the 1st token of an statement or declaration ot code block
from main program and then build parse-tree for whole block of code
in a single run.
*/

ParseTree *Parser(Token *token){
	switch (token->kind){
		case TK_KW:
			if(token->value == KW_INT){
				return int_var_decl(token);
			}
			else if(token->value == KW_DEF){
				return func_decl(token);
			}

		case TK_ID:
			/* VAR ASSIGMENT or INITILISATION */
			// or
			// funtion calling
			return var_init(token);
		default:
			printf("No expected tokens, exiting\n");
			exit(-1);
			//more keyword starting from here according to grammer
		//case TK_ID:
	}
}


ParseTree *func_decl(Token token){
	/*
		Tree will be like:
				o
			   / \
		   "def"  o
			 	 / \
	    returntype  o
				   / \
	    	  idName  o
	    	  		 / \
	    	  	   '('  o
	    	  	       / \
	    	  (stmts) o   \
					 / \   \
			 (stmt) o   o   \
				   /   / \   \
				  /	stmt ...  \
				 /             \
				o               o
			   / \ 			   / \
		     type idname     ')'  o
				           	     / \
			       			   ':'  o
								   / \
                          (decls) o   \
								 / \   \
						        /   o   \
						       /   / \   \
						(decl)o  decl ... \
						  	 / \           \
                             ...            \
                                             o
											/ \
										  ';'  o
										      / \
									'\n' | EOF	 NULL

	*/

	ParseTree *tree0 = (ParseTree *)malloc(sizeof(ParseTree));
	tree0->termnode = (Node *)malloc(sizeof(Node));
	
	tree0->termnode->TType = TREE_FUNDECL;
	tree0->termnode->value = KW_DEF;
	tree0->termnode->lineno = token->lineno;
	tree0->termnode->linepos = token->linepos;
	free(token);

	Token *nextToken = lexer();

	if (nextToken->kind == TK_KW || nextToken->kinf == TK_ID) //OR OTHER DATATYPE OR USERDEFINED DATATYPES
		{
			ParseTree *tree1 = (ParseTree *)malloc(sizeof(ParseTree));
			tree0->nontermnode = tree1;
			tree1->termnode = (Node *)malloc(sizeof(Node));

			// -----------------------------
			tree1->termnode->LType = LF_KW;
			tree1->termnode->value = DT_INT;
			// -----------------------------
			tree1->termnode->lineno = nextToken->lineno;
			tree1->termnode->linepos = nextToken->linepos;

			free(nextToken);
			nextToken = lexer();

			if (nextToken->kind == TK_ID){
				ParseTree *tree2 = (ParseTree *)malloc(sizeof(ParseTree));
				tree1->nontermnode = tree2;
				tree2->termnode = (Node *)malloc(sizeof(Node));

				tree2->termnode->LType = LF_ID;
				memcpy(tree2->termnode->name, nextToken->name, strlen(nextToken->name)+1);
				tree2->termnode->lineno = nextToken->lineno;
				tree2->termnode->linepos = nextToken->linepos;
				
				free(nextToken);
				nextToken = lexer();

				if(nextToken->kind == TK_LPAREN){
					ParseTree *tree3 = (ParseTree *)malloc(sizeof(ParseTree));
					tree2->nontermnode = tree3;
					tree3->termnode = (Node *)malloc(sizeof(Node));

					tree3->termnode->LType = LF_LPAREN;
					tree3->termnode->lineno = nextToken->lineno;
					tree3->termnode->linepos = nextToken->linepos;

					free(nextToken);

					nextToken = lexer();

					// handle stmts
					ParseTree *tree4 = (ParseTree *)malloc(sizeof(ParseTree));
					tree3->nontermnode = tree4;
					
					ParseTree *subnt = (ParseTree *)malloc(sizeof(ParseTree));
					tree4->subnontermnode = subnt;
					
					if(nextToken->kind != TK_RPAREN){
						// declare all statements here
						// MAYBE WE WILL LOOK FOR CONTION IN SYMBOL TABLE
						// MAYBE WE WILL MAKE A SEPRATE SYMTABLE FOR 
						// KEYWORD AND USER DEFIEND DATATYPES
						// either we can check for condtion here in parser
						// or in sematic analysis
						// but we gonna do it here for now 
						while(1){

							if(nextToken->kind == TK_KW ){ // or userdefined datatype
								// LET ASSUME THE ARG DATATYPE IS int
								int tempTokenType = DT_INT;
								free(nextToken);							
								nextToken = lexer();
								if(nextToken->kind == TK_ID){
									subnt->subnontermnode->termnode = (Node *)malloc(sizeof(Node));
									subnt->subnontermnode->termnode->LType = LF_KW;
									subnt->subnontermnode->termnode->value = tempTokenType;
									subnt->subnontermnode->termnode->linepos = nextToken->linepos;
									subnt->subnontermnode->termnode->lineno = nextToken->lineno;
									

									subnt->subnontermnode->subtermnode = (Node *)malloc(sizeof(Node));
									subnt->subnontermnode->termnode->LType = LF_KW;
									subnt->subnontermnode->termnode->value = tempTokenType;
									subnt->subnontermnode->termnode->linepos = nextToken->linepos;
									subnt->subnontermnode->termnode->lineno = nextToken->lineno;
									


								}else{
									printf("Syntax Errir: invalid name of identifer in funtion arguments at %d:%d\n", nextToken->linepos, nextToken->lineno);
									free(nextToken);
									exit(-1);
								}

							}else{
								printf("Unrecognized Datatype for funtion arguments at %d:%d\n", nextToken->linepos, nextToken->lineno);
								free(nextToken);
								exit(-1);
							}
						}


					}else if(nextToken->kind == TK_RPAREN)
					{
						// furter chk here
						tree4->subnontermnode->subnontermnode = NULL;
						tree4->subnontermnode->nontermnode = NULL;

					}else{
						printf("Syntax ERROR: invalid token after '(' at %d:%d\n", nextToken->linepos, nextToken->lineno);
						free(nextToken);
						exit(-1);
					}

					ParseTree *tree5 = (ParseTree *)malloc(sizeof(ParseTree));
					tree5->termnode = (Node *)malloc(sizeof(Node));
					tree4->nontermnode = tree5;

					tree5->termnode->LType = LF_RPAREN;
					tree5->termnode->lineno = nextToken->lineno;
					tree5->termnode->linepos = nextToken->linepos;

					free(nextToken);

					nextToken = lexer();

					if(nextToken->kind == TK_COLON){
						// Futer defination here
					}else{
						printf("Syntax Error: expecting a : in funtion declaration at %d:%d\n", nextToken->linepos, nextToken->lineno);
						free(nextToken);
						exit(-1);
					}

				}else{
					printf("Syntax error after funtion-name in funtion declaration at %d:%d expecting a '('\n", nextToken->linepos, nextToken->lineno);
					free(nextToken);
					exit(-1);
				}

			}else{
				printf("Token in funtion defination at %d:%d has an invalid identifer name\n", nextToken->lineno, nextToken->linepos);
				free(nextToken);
				exit(-1);
			}
		}else{
			printf("Invalid return-type defined for funtion at %d:%d is not recognized by compiler\n");
			free(nextToken);
			exit(-1);
		}

	



}


/*

							ParseTree *nextnt = (ParseTree *)malloc(sizeof(ParseTree));
							ParseTree *subnt = tree4->subnontermnode;
							
							if(a == 0){
								tree4->subnontermnode = nextnt;
							}else{
								subnt->nontermnode = nextnt;
							}
							tree4->subnontermnode->subnontermnode = (ParseTree *)malloc(sizeof(ParseTree));
						
							tree4->subnontermnode->subnontermnode->termnode = (Node *)malloc(sizeof(Node));
							tree4->subnontermnode->subnontermnode->subtermnode = (Node *)malloc(sizeof(Node));

							tree4->subnontermnode->subnontermnode->termnode->value = KW_INT; //nextToken->value;
							memcpy(tree4->subnontermnode->subnontermnode->subtermnode->name, nextToken->name, strlen(nextToken->name)+1);
							
							free(nextToken);
							nextToken = lexer();

							// ----------------------
							if(nextToken->kind == TK_COMMA){
								subnt->nontermnode = (ParseTree *)malloc(sizeof(ParseTree));

							}

*/


/*


					if(nextToken->kind == TK_RPAREN){
						ParseTree *tree5 = (ParseTree *)malloc(sizeof(ParseTree));
						tree3->nontermnode = tree4;
						tree5->termnode = (Node *)malloc(sizeof(Node));

						tree4->termnode->LType = LF_RPAREN;
						tree4->termnode->lineno = nextToken->lineno;
						tree4->termnode->linepos = nextToken->linepos;

						free(nextToken);

						nextToken = lexer();

						if(nextToken->kind == TK_COLON){
							ParseTree *tree5 = (ParseTree *)malloc(sizeof(ParseTree));
							tree5->termnode = (Node *)malloc(sizeof(Node));
							tree4->nontermnode = tree5;

							tree5->LType = LF_COLON;
							tree5->termnode->lineno = nextToken->lineno;
							tree5->termnode->linepos = nextToken->linepos;
							
							free(nextToken);

							//handle decls
							nextToken = lexer();


						}else if(nextToken->kind == TK_SEMICOLON){
							// THIS WILL BE A FUNTION PROTOTYPE THEN
						}else{
							printf("Syntax Error: invalid token type after ')' in funtion defination at %d:%d\n", nextToken->linepos, nextToken->lineno);
							free(nextToken);
							exit(-1);
						}


					}else{
						printf("SyntaxError: was expecting a ')' in funtion declartion at %d:%d\n", nextToken->linepos, nextToken->lineno);
						free(nextToken);
						exit(-1);
					}
*/

ParseTree *var_init(Token *token){
	char idName[strlen(token->name)+1];
	memcpy(idName, token->name, strlen(token->name)+1);
	
	/* Grammer: 
		varinit --> idName "=" data ("\n"|EOF) */

	/*
		Tree Strcutre:

	*/
	ParseTree *tree0 = (ParseTree *)malloc(sizeof(ParseTree));
	tree0->termnode = (Node *)malloc(sizeof(Node));
	
	tree0->termnode->TType = TREE_INITVAR;
	memcpy(tree0->termnode->name, idName, sizeof(idName));
	tree0->termnode->lineno = token->lineno;
	tree0->termnode->linepos = token->linepos;
	free(token);
	ParseTree *tree1 = (ParseTree *)malloc(sizeof(ParseTree));
	tree0->nontermnode = tree1;

	
	Token *nextToken = lexer();

	if(nextToken->kind == TK_ASOP_EQ){
		
		tree1->termnode = (Node *)malloc(sizeof(Node));
		tree1->termnode->LType = LF_ASOP_EQ;

		tree1->termnode->lineno = nextToken->lineno;
		tree1->termnode->linepos = nextToken->linepos;
		free(nextToken);
		
		ParseTree *tree2 = (ParseTree *)malloc(sizeof(ParseTree));
		tree1->nontermnode = tree2;

		
		nextToken = lexer();

		if(nextToken->kind==TK_INTLIT){ //or other datatype
			int value = nextToken->value;
			tree2->termnode->lineno = nextToken->lineno;
			tree2->termnode->linepos = nextToken->linepos;

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
				
				tree3->termnode = (Node *)malloc(sizeof(Node));
				tree3->termnode->LType = LF_DECL_TERM;
				tree1->termnode->lineno = nextToken->lineno;
				tree1->termnode->linepos = nextToken->linepos;

				free(nextToken);
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
		// EXPECT A FUNTION CALL
	}

}

ParseTree *int_var_decl(Token *token){

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
	tree0->termnode->lineno = token->lineno;
	tree0->termnode->linepos = token->linepos;

	ParseTree *tree1 = (ParseTree *)malloc(sizeof(ParseTree));
	tree0->nontermnode = tree1;

	free(token);
	Token *nextToken = lexer();

	if(nextToken->kind == TK_ID){
	
		char idName[strlen(nextToken->name)+1];
		memcpy(idName, nextToken->name, strlen(nextToken->name)+1);
		
		

		tree1->termnode = (Node *)malloc(sizeof(Node));
		tree1->termnode->LType = LF_ID;
		
		memcpy(tree1->termnode->name, idName, sizeof(idName));
		tree1->termnode->lineno = nextToken->lineno;
		tree1->termnode->linepos = nextToken->linepos;

		ParseTree *tree2 = (ParseTree *)malloc(sizeof(ParseTree));
		tree1->nontermnode = tree2;

		free(nextToken);
		nextToken = lexer();

		if(nextToken->kind == TK_NEWLINE || nextToken->kind == TK_EOF){

			tree2->termnode = (Node *)malloc(sizeof(Node));

			tree2->termnode->LType = LF_DECL_TERM;
			tree2->termnode->lineno = nextToken->lineno;
			tree2->termnode->linepos = nextToken->linepos;
			tree2->nontermnode = NULL;

			// return sub-ParseTree
			free(nextToken);
			return tree0;

		}

		else if(nextToken->kind == TK_ASOP_EQ){

			tree2->termnode = (Node *)malloc(sizeof(Node));
			tree2->termnode->LType = LF_ASOP_EQ;
			tree2->termnode->lineno = nextToken->lineno;
			tree2->termnode->linepos = nextToken->linepos;

			ParseTree *tree3 = (ParseTree *)malloc(sizeof(ParseTree));
			tree2->nontermnode = tree3;

			// next token may contain expr or data
			// if it has expr we have to handle 
			// that with other funtion

			free(nextToken);

			nextToken = lexer();
			int data = nextToken->value;

			if(nextToken->kind != TK_INTLIT){
				printf("ERROR, expecting a intlit at %d:%d in file.c\n",nextToken->lineno, nextToken->linepos);
				exit(-1);
			}

			tree3->termnode = (Node *)malloc(sizeof(Node));
			tree3->termnode->LType = LF_INTLIT;
			tree3->termnode->value = data;
			tree3->termnode->lineno = nextToken->lineno;
			tree3->termnode->linepos = nextToken->linepos;

			ParseTree *tree4 = (ParseTree *)malloc(sizeof(ParseTree));
			tree3->nontermnode = tree4;

			free(nextToken);
			nextToken = lexer();

			if(nextToken->kind == TK_NEWLINE || nextToken->kind == TK_EOF){
				
				// it is NOT a expr
				// data = token value is confirmed

				// maybe we want to initilize each Node too?
				tree4->termnode = (Node *)malloc(sizeof(Node));
				tree4->termnode->LType = LF_DECL_TERM;
				tree4->termnode->lineno = nextToken->lineno;
				tree4->termnode->linepos = nextToken->linepos;
				tree4->nontermnode = NULL;

				free(nextToken);

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
