#include <stdlib.h>
#include <string.h>

#include "symtable.h"

SymbolTable *createSymbolTable(){
	SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
	table->currentScope = NULL;
	return table;
}

void enterScope(SymbolTable *table){
	Scope *newScope = (Scope *)malloc(sizeof(Scope));
	newScope->symbolList = NULL;
	newScope->parent = table->currentScope;
	newScope->child = NULL;

	if(table->currentScope != NULL){
		table->currentScope->child = newScope;
	} //?
	table->currentScope = newScope;
}

void exitScope(SymbolTable *table){
	if(table->currentScope != NULL){
		table->currentScope = table->currentScope->parent;
	}
}

void addSymbol(SymbolTable *table, const char name[128], SymbolKind kind, int datatype){
	SymbolEntry *newSymbol = (SymbolEntry *)malloc(sizeof(SymbolEntry));
	strcpy(newSymbol->name, name);
	newSymbol->kind = kind;
	newSymbol->datatype = datatype;
	newSymbol->next = table->currentScope->symbolList;
	table->currentScope->symbolList = newSymbol;
}


SymbolEntry *findSymbol(SymbolTable *table, const char name[128]){
	Scope *current = table->currentScope;
	while (current != NULL){
		SymbolEntry *symbol = current->symbolList;
		while(symbol != NULL){
			if(strcmp(symbol->name, name) == 0){
				return symbol;
			}
			symbol = symbol->next;
		}
		current = current->parent;
	}
	return NULL;
}