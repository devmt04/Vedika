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
ParseTree *func_decl(Token *token);

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


ParseTree *func_decl(Token *token){
	/*
		Tree will be like:
				o   0 (tree0)
			   / \
		   "def"  o   1 
			 	 / \ 
	    returntype  o   2
				   / \
	    	  idName  o   3
	    	  		 / \
	    	  	   '('  o   4
	    	  	       / \
	    	  (stmts) o   \
					 / \   \
			 (stmt) o   o   \
				   /   / \   \
				  /	stmt ...  \
				 /             \
				o               o  5
			   / \ 			   / \
		     type idname     ')'  o  6
				           	     / \
			       			   ':'  o  7
								   / \
                          (decls) o   \
								 / \   \
						        /   o   \
						       /   / \   \
						(decl)o   ;	 NULL \
						  	 / \           \
                             ...            \
                                           NULL(End of funtion)

	*/

	ParseTree *tree0 = (ParseTree *)malloc(sizeof(ParseTree));
	tree0->termnode = (Node *)malloc(sizeof(Node));
	
	tree0->termnode->TType = TREE_FUNDECL;
	tree0->termnode->value = KW_DEF;
	
	// THERE IS NO NEED TO ADD LINENO AND LINEPOS FOR THOSE
	// WHICH ARE RELATED TO SYNTAX AS WE WILL DO
	// SYNTAX CHECKING HERE, THE ONE HAVB CONCERN WITH SEMATIC
	// HAS TO BE ADDED FOR IT

	//tree0->termnode->lineno = token->lineno;
	//tree0->termnode->linepos = token->linepos;
	free(token);

	Token *nextToken = lexer();

	if (nextToken->kind == TK_KW || nextToken->kind == TK_ID) //OR OTHER DATATYPE OR USERDEFINED DATATYPES
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
					//tree3->termnode->lineno = nextToken->lineno;
					//tree3->termnode->linepos = nextToken->linepos;

					free(nextToken);

					nextToken = lexer();

					// handle stmts
					int isAnyArgs = 0;
					ParseTree *tree4 = (ParseTree *)malloc(sizeof(ParseTree));
					tree3->nontermnode = tree4;
					
					if(nextToken->kind != TK_RPAREN){
						// declare all statements here
						// MAYBE WE WILL LOOK FOR CONTION IN SYMBOL TABLE
						// MAYBE WE WILL MAKE A SEPRATE SYMTABLE FOR 
						// KEYWORD AND USER DEFIEND DATATYPES
						// either we can check for condtion here in parser
						// or in sematic analysis
						// but we gonna do it here for now 

						isAnyArgs = 1;

						ParseTree *subnt = (ParseTree *)malloc(sizeof(ParseTree));
						tree4->subnontermnode = subnt;
						
						while(1){

							if(nextToken->kind == TK_KW ){ // or userdefined datatype
								// LET ASSUME THE ARG DATATYPE IS int
								int tempTokenType = DT_INT;
								int lineno = nextToken->lineno;
								int linepos = nextToken->linepos;
								free(nextToken);							
								nextToken = lexer();
								if(nextToken->kind == TK_ID){
									subnt->subnontermnode->termnode = (Node *)malloc(sizeof(Node));
									subnt->subnontermnode->termnode->LType = LF_KW;
									subnt->subnontermnode->termnode->value = tempTokenType;
									subnt->subnontermnode->termnode->linepos = linepos;
									subnt->subnontermnode->termnode->lineno = lineno;
									

									subnt->subnontermnode->subtermnode = (Node *)malloc(sizeof(Node));
									subnt->subnontermnode->subtermnode->LType = LF_ID;
									memcpy(subnt->subnontermnode->subtermnode->name, nextToken->name, strlen(nextToken->name)+1);
									subnt->subnontermnode->subtermnode->linepos = nextToken->linepos;
									subnt->subnontermnode->subtermnode->lineno = nextToken->lineno;
									
									free(nextToken);

									nextToken = lexer();
									if(nextToken->kind == TK_COMMA){
										free(nextToken);
										nextToken = lexer();
										subnt = subnt->nontermnode;
										continue;
									}else if(nextToken->kind == TK_RPAREN){
										subnt->nontermnode = NULL;
										break;
									}else{
										printf("Syntax ERROR at %d:%d, expecting a ',' or a ')' in funtion declaration\n", nextToken->linepos, nextToken->lineno);
										free(nextToken);
										exit(-1);
									}


								}else{
									printf("Syntax Error: invalid name of identifer in funtion arguments at %d:%d\n", nextToken->linepos, nextToken->lineno);
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
						// No args present
						if(isAnyArgs == 0){
							tree4->subnontermnode = NULL;
						}
						
					}else{
						printf("Syntax ERROR: invalid token after '(' at %d:%d\n", nextToken->linepos, nextToken->lineno);
						free(nextToken);
						exit(-1);
					}

					ParseTree *tree5 = (ParseTree *)malloc(sizeof(ParseTree));
					tree5->termnode = (Node *)malloc(sizeof(Node));
					tree4->nontermnode = tree5;

					tree5->termnode->LType = LF_RPAREN;
					//tree5->termnode->lineno = nextToken->lineno;
					//tree5->termnode->linepos = nextToken->linepos;

					free(nextToken);

					nextToken = lexer();

					if(nextToken->kind == TK_COLON){
						// Futer defination here
						// I THINK THER IS NO NEED TO ADD :
						// IN PARSE TREE BUT IN-CASE WE NEED IT
						// WE GONNA MAKE IT PART OF OUR PARE TREE
						ParseTree *tree6 = (ParseTree *)malloc(sizeof(ParseTree));
						tree6->termnode = (Node *)malloc(sizeof(Node));
						tree5->nontermnode =  tree6;

						tree6->termnode->LType = LF_COLON;
						// NO NEED TO ADD LINE-NO AND LINE POS
						free(nextToken);

						ParseTree *tree7 = (ParseTree *)malloc(sizeof(ParseTree));
						tree6->nontermnode =  tree7;

						ParseTree *declsnt = (ParseTree *)malloc(sizeof(ParseTree));
						tree7->subnontermnode = declsnt;
						int isFirstDecl = -1;

						/*
						decl -->  vardecl
							     | ifdecl
							     | ifelsedecl
							     | ifelseifdecl
							     | whiledecl
							     | fordecl
							     | functioncall
							     | returnstatement
							     | globalvarinit
							     | strcutdecl    
						*/

						while(1){
							isFirstDecl++;
							nextToken = lexer();
							// Var-Decl
							if(nextToken->kind == TK_KW){
								//---------------------------
								if(nextToken->value == KW_INT){
									if(isFirstDecl != 0){
										declsnt = (ParseTree *)malloc(sizeof(ParseTree));
									}
									declsnt->subnontermnode = int_var_decl(nextToken);
									//declsnt->nontermnode = (ParseTree *)malloc(sizeof(ParseTree));
									declsnt = declsnt->nontermnode;
									continue;
								}

								else if(nextToken->value == KW_RET){
									// For now we just return a var
									// in future we will return
									// an expr, datastrcture, intlit, other datatypes
									
									if(isFirstDecl != 0){
										declsnt = (ParseTree *)malloc(sizeof(ParseTree));
									}

									declsnt->termnode = (Node *)malloc(sizeof(Node));
									declsnt->subtermnode = (Node *)malloc(sizeof(Node));

									declsnt->termnode->LType = LF_KW;
									declsnt->termnode->value = KW_RET;
									declsnt->termnode->lineno = nextToken->lineno;
									declsnt->termnode->linepos = nextToken->linepos;

									free(nextToken);
									nextToken = lexer();

									if(nextToken->kind == TK_ID){

										declsnt->subtermnode->LType = LF_ID;
										memcpy(declsnt->subtermnode->name, nextToken->name, strlen(nextToken->name)+1);
										declsnt->subtermnode->lineno = nextToken->lineno;
										declsnt->subtermnode->linepos = nextToken->linepos;
										free(nextToken);
										declsnt = declsnt->nontermnode;
										//continue;									
									}

									else if(nextToken->kind == TK_INTLIT){

										declsnt->subtermnode->LType = LF_INTLIT;
										declsnt->subtermnode->value = nextToken->value;
										declsnt->subtermnode->lineno = nextToken->lineno;
										declsnt->subtermnode->linepos = nextToken->linepos;									
										free(nextToken);
										declsnt = declsnt->nontermnode;
										//continue;
									}else{
										printf("Syntax Error at %d:%d, invalid token at return value in funtion defination\n", nextToken->linepos, nextToken->lineno);
										free(nextToken);
										exit(-1);
									}

									nextToken = lexer();
									if(nextToken->kind == TK_NEWLINE){
										free(nextToken);
										continue;
									}else{
										printf("Syntax ERROR at %d:%d, expecting a newline charcter at the end of return statement\n", nextToken->lineno, nextToken->linepos);
										free(nextToken);
										exit(-1);
									}


									
								}

								else if(nextToken->value == KW_ENDDEF){
									// END OF FUNTION DEFINATION
									if(isFirstDecl == 0){
										printf("ERROR at %d:%d, Funtion defination cannot be empty", nextToken->linepos, nextToken->lineno);
										free(nextToken);
										exit(-1);
									}

									free(nextToken);
									nextToken = lexer();
									if(nextToken->kind == TK_NEWLINE || nextToken->kind == TK_EOF){
										declsnt = NULL;
										tree7->nontermnode = NULL;
										return tree0;
									}else{
										printf("Syntax Error at %d:%d, expecting a newline char or EOF at end of funtion definaton\n", nextToken->linepos, nextToken->lineno);
										free(nextToken);
										exit(-1);
									}

								}

							}

							else if(nextToken->kind == TK_ID){
								// var initilisation
								// ? or struct initilisation
								// funtion calling
								if(isFirstDecl != 0){
									declsnt = (ParseTree *)malloc(sizeof(ParseTree));
								}

							}

							else if(nextToken->kind == TK_NEWLINE){
								isFirstDecl--;

								continue;
							}

							else {
								printf("Syntax Error at %d:%d, Invalid token inside funtion block\n", nextToken->lineno, nextToken->linepos);
								free(nextToken);
								exit(-1);
							}
						}

					}
					else if(nextToken->kind == TK_SEMICOLON){
						// it must ve a funtion prototype

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
