#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../symtable/symtable.h"
#include "../ircodegen/ircodegen.h"
#include "semantic.h"

extern SymbolTable *table;

void semantic_check(ParseTree *tree){
	if(tree->termnode->TType == TREE_VARDECL){
		// CHECK IF VAR IS ALREADY DECLARED OR NOT
		// Our Compiler will not allow the redeclaration of variables
		char idName[strlen(tree->nontermnode->termnode->name)+1]; 
		memcpy(idName, tree->nontermnode->termnode->name, sizeof(idName));
		
		SymbolEntry *symbol = findSymbol(table, idName);
		if (symbol != NULL){
			printf("Identifer %s already declared at %d:%d in file", symbol->name, symbol->linepos, symbol->lineno);
			//printf("ERROR: You cannot redeclare %s variable again\n", symbol->name);
			exit(-1);
		}else {
			/* DECLARE THAT VARIABLE */
			int linepos = tree->nontermnode->termnode->linepos;
			int lineno = tree->nontermnode->termnode->lineno;
			addSymbol(table, idName, VAR_ID, DT_INT, 0, lineno, linepos);
			IRGen(tree);
		}



		DataTypes datatype = tree->termnode->value;
		
		
		

	}

	else if(tree->termnode->TType == TREE_INITVAR){

	}
}



