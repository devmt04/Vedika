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

		DataTypes datatype = tree->termnode->value;
		char idName[strlen(tree->nontermnode->termnode->name)+1]; 
		memcpy(idName, tree->nontermnode->termnode->name, sizeof(idName));
		
		SymbolEntry *symbol = findSymbol(table, idName);

		if (symbol != NULL){
			printf("ERROR: You cannot redeclare %s variable again\n", symbol->name);
			exit(-1);
		}else {
			/* DECLARE THAT VARIABLE */
			addSymbol(table, idName, VAR_ID, DT_INT, 0);
			IRGen(tree);
		}

	}

	else if(tree->termnode->TType == TREE_INITVAR){

	}
}



