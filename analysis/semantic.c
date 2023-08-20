#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../symtable/symtable.h"
#include "../ircodegen/ircodegen.h"
#include "semantic.h"

extern SymbolTable *table;

int var_decl_sem_check(ParseTree *tree);
int var_init_sem_chk(ParseTree *tree);
int semantic_check(ParseTree *tree);
int func_decl_sem_chk(ParseTree *tree);

int semantic_check(ParseTree *tree){
	if(tree->termnode->TType == TREE_VARDECL){
		return var_decl_sem_check(tree);
	}
	else if(tree->termnode->TType == TREE_INITVAR){
		return var_init_sem_chk(tree);
	}
	else if(tree->termnode->TType == TREE_FUNDECL){
		return func_decl_sem_chk(tree);
	}
}


int func_decl_sem_chk(ParseTree *tree){
	char *idName = tree->nontermnode->nontermnode->termnode->name;
	DataTypes returnType = tree->nontermnode->termnode->value;

	// chk if funtion id is already used or not
	SymbolEntry *symbol = findSymbol(table, idName);
	if(symbol != NULL){
		printf("SEMATIC ERROR: Funtion name %s is already used at %d:%d\n", idName, symbol->linepos, symbol->lineno);
		// free the parse-tree
		exit(-1);
	}else{
		addSymbol(table, idName, FUNC_ID, returnType, 1, tree->termnode->lineno, tree->termnode->linepos);
	}

	ParseTree *argsTree = tree->nontermnode->nontermnode->nontermnode->nontermnode->subnontermnode;
	ParseTree *declsTree = tree->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->subnontermnode;
						   tree->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->subnontermnode;
	// ADD ALL ARGS IN LOCAL SYMBOL TABLE
	ParseTree *tempArgsNode = argsTree;
	while(1){
		if(tempArgsNode == NULL){
			break;
		}
		SymbolEntry *symbol = findSymbol(table, tempArgsNode->subnontermnode->subtermnode->name);
		if(symbol == NULL){
			addSymbol(table, tempArgsNode->subnontermnode->subtermnode->name, VAR_ID, tempArgsNode->subnontermnode->termnode->value, -1, -1, -1);
			tempArgsNode = tempArgsNode->nontermnode;
		}else{
			printf("ERROR: Same variable decclared more than once as funtion args at %d:%d\n", tree->termnode->lineno, tree->termnode->linepos);
			exit(-1);
		}
	}
	/*CHK FOR ILLEGAL BREAK STATEMENTS,
	ILLEGAL RETURN STATEMENTS*/
/*
	ParseTree *tempdeclNode = declsTree;
	
	while(1){

		if(tempdeclNode == NULL){
			break;
		}

		// return-type checking
		else if(tempdeclNode->subnontermnode->termnode->LType == LF_KW && tempdeclNode->subnontermnode->termnode->value == KW_RET){
			DataTypes datatype = tempdeclNode->subtermnode->LType;
			printf("%d %d\n", returnType, datatype);
			if(returnType != datatype){
				printf("ERROR at %d:%d, Datatype pass in return statement of function '%s' is not matched with return-type for which funton defined!\n", tempdeclNode->termnode->lineno, tempdeclNode->termnode->linepos, idName);
				exit(-1);
			}else{
				tempdeclNode = tempdeclNode->nontermnode;
				continue;
			}
		}
		

	}
*/
	return 0;
}

int var_init_sem_chk(ParseTree *tree){
	//Chk if it is declared or NOT
	// it may declared globally or locally so Chk wisely
	char idName[strlen(tree->termnode->name)+1];
	memcpy(idName, tree->termnode->name, sizeof(idName));

	SymbolEntry *symbol = findSymbol(table, idName);

	if(symbol == NULL){
		printf("Identifer %s is unknown, seeing it first time without any declarations at %d:%d", idName, tree->termnode->lineno, tree->termnode->linepos);
		exit(-1);
	}

	return 0;
}

int var_decl_sem_check(ParseTree *tree){
	// CHECK IF VAR IS ALREADY DECLARED OR NOT
	// Our Compiler will not allow the redeclaration of variables
	char idName[strlen(tree->nontermnode->termnode->name)+1];
	memcpy(idName, tree->nontermnode->termnode->name, sizeof(idName));
	DataTypes datatype = tree->termnode->value;

	SymbolEntry *symbol = findSymbol(table, idName);
	if (symbol != NULL){
		printf("Identifer %s already declared at %d:%d in file.c", symbol->name, symbol->linepos, symbol->lineno);
		exit(-1);
	}

	else {
		// ADD VARIABLE TO SYMTABLE
		int linepos = tree->nontermnode->termnode->linepos;
		int lineno = tree->nontermnode->termnode->lineno;

		// CHECKING IF VAR IS INITILIZED OR UNINITILIZED
		if(tree->nontermnode->nontermnode->termnode->LType == LF_DECL_TERM){
			// UN-INITILIZED
			addSymbol(table, idName, VAR_ID, datatype, 0, lineno, linepos);
			return 0;
		}else{
			// INITILIZED
			addSymbol(table, idName, VAR_ID, datatype, 1, lineno, linepos);
			return 1;
		}
	}
}
	








/*
void func_decl_sem_chk(ParseTree *tree){
	// idName, returnType, exprTree, declTree
	//char idName[strlen(tree->nontermnode->nontermnode->termnode->name)+1];
	//memcpy(idName, tree->nontermnode->nontermnode->termnode->name, strlen(tree->nontermnode->nontermnode->termnode->name)+1);
	char *idName = tree->nontermnode->nontermnode->termnode->name;
	DataTypes returnType = tree->nontermnode->termnode->value;

	// chk if funtion id is already used or not
	SymbolEntry *symbol = findSymbol(table, idName);
	if(symbol != NULL){
		printf("SEMATIC ERROR: Funtion name %s is already used at %d:%d\n", idName, symbol->linepos, symbol->lineno);
		// free the parse-tree
		exit(-1);
	}else{
		addSymbol(table, idName, FUNC_ID, returnType, 1, tree->termnode->lineno, tree->termnode->linepos);
		// THE TYPE CHING OF RETURN STATEMENT CAN BE
		// DONE IN PARSING PHASE ???

		// CURRENTLY W ARE NOT WORKING ON PROTOTYPES
		// MAT BE WE NEED TO MENTION SCOPE OF DECLARION
		// IN BELOW FUNTION
		// irgen_func_decl(idName, returnType, argsTree, declsTree);
		// we have to check sematics for return statements
		
//---------------------
		//irgen_func_decl(idName, returnType, tree->nontermnode->nontermnode->nontermnode->nontermnode->subnontermnode, tree->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->subnontermnode);
		

		//	free_Tree(tree); //!

		// create a new local symtable
		// add args in symbol table
	}

}

void var_init_sem_chk(ParseTree *tree){
	//Chk if it is declared or NOT
	// it may declared globally or locally so Chk wisely
	char idName[strlen(tree->termnode->name)+1];
	memcpy(idName, tree->termnode->name, sizeof(idName));

	SymbolEntry *symbol = findSymbol(table, idName);
/*	
	if (symbol != NULL){
		//DataTypes datatype = symbol->datatype;
		//TYPE CHECKING
		//int data = tree->nontermnode->nontermnode->termnode->value;
		//irgen_glob_int_var_init(idName, data);		

	} */
/*
	if(symbol == NULL){
		printf("Identifer %s is unknown, seeing it first time without any declarations at %d:%d", idName, tree->termnode->lineno, tree->termnode->linepos);
		exit(-1);
	}
}
/*
void var_decl_sem_check(ParseTree *tree){
	// CHECK IF VAR IS ALREADY DECLARED OR NOT
	// Our Compiler will not allow the redeclaration of variables
	char idName[strlen(tree->nontermnode->termnode->name)+1]; 
	memcpy(idName, tree->nontermnode->termnode->name, sizeof(idName));
	DataTypes datatype = tree->termnode->value;

	SymbolEntry *symbol = findSymbol(table, idName);
	
	if (symbol != NULL){
		printf("Identifer %s already declared at %d:%d in file.c", symbol->name, symbol->linepos, symbol->lineno);
		exit(-1);
	}


	else {
		// DECLARE THAT VARIABLE 
		int linepos = tree->nontermnode->termnode->linepos;
		int lineno = tree->nontermnode->termnode->lineno;
		
		if(datatype == DT_INT){
			// CHECKING IF VAR IS INITILIZED OR UNINITILIZED
			if(tree->nontermnode->nontermnode->termnode->LType == LF_DECL_TERM){
				// UN-INITILIZED
				addSymbol(table, idName, VAR_ID, DT_INT, 0, lineno, linepos);
				//irgen_glob_int_var_decl(0, idName, 0);
			}else{
				// INITILIZED
				addSymbol(table, idName, VAR_ID, DT_INT, 1, lineno, linepos);
				//irgen_glob_int_var_decl(1, idName, tree->nontermnode->nontermnode->nontermnode->termnode->value);
			}
			free_Tree(tree);
		}
		else {
			// Other datatype
		}
	}
	
}

*/