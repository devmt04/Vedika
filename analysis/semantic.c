#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../symtable/symtable.h"
#include "../ircodegen/ircodegen.h"
#include "semantic.h"

extern SymbolTable *table;

void var_decl_sem_check(ParseTree *tree);
void var_init_sem_chk(ParseTree *tree);

//free memory of tree-heap
void free_Tree(ParseTree *tree){
	ParseTree *node = tree;
	Node *leaf = tree->termnode;
	while(1){
		ParseTree *nextNT = node->nontermnode;	//next non-terminal
		free(leaf);
		free(node);
		if(nextNT == NULL){
			break;
		}
		node = nextNT;
		leaf = node->termnode;
	}
}

void semantic_check(ParseTree *tree){
	if(tree->termnode->TType == TREE_VARDECL){
		var_decl_sem_check(tree);
	}
	else if(tree->termnode->TType == TREE_INITVAR){
		var_init_sem_chk(tree);
	}
}

void var_init_sem_chk(ParseTree *tree){
	//Chk if it is declared or NOT
	// it may declared globally or locally so Chk wisely
	char idName[strlen(tree->termnode->name)+1];
	memcpy(idName, tree->termnode->name, sizeof(idName));

	SymbolEntry *symbol = findSymbol(table, idName);
	
	if (symbol != NULL){
		DataTypes datatype = symbol->datatype;
		//TYPE CHECKING
		int data = tree->nontermnode->nontermnode->termnode->value;
		irgen_glob_int_var_init(idName, data);		

	}else {
		printf("Identifer %s is unknown, seeing it first time without any declarations at %d:%d", idName, tree->termnode->lineno, tree->termnode->linepos);
		exit(-1);
	}
}

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
	}else {
		/* DECLARE THAT VARIABLE */
		int linepos = tree->nontermnode->termnode->linepos;
		int lineno = tree->nontermnode->termnode->lineno;
		
		if(datatype == DT_INT){
			// CHECKING IF VAR IS INITILIZED OR UNINITILIZED
			if(tree->nontermnode->nontermnode->termnode->LType == LF_DECL_TERM){
				// UN-INITILIZED
				addSymbol(table, idName, VAR_ID, DT_INT, 0, lineno, linepos);
				irgen_glob_int_var_decl(0, idName, 0);
			}else{
				// INITILIZED
				addSymbol(table, idName, VAR_ID, DT_INT, 1, lineno, linepos);
				irgen_glob_int_var_decl(1, idName, tree->nontermnode->nontermnode->nontermnode->termnode->value);
			}
			free_Tree(tree);
		}

		else {
			// Other datatype
		}
	}
}



