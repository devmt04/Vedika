#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "../parser/parser.h"

typedef enum {
	VAR_ID = 0,
	FUNC_ID = 1,
	CLASS_ID = 2,
} SymbolKind;


/* Define strcuures */
typedef struct SymbolEntry {
	SymbolKind kind;	// 0 for Variable_ID 
	char name[128];
	DataTypes datatype;
	// intlitdata, strlitdata

	// other attr like lineno, pos, etc for error handling
	//int uniqeID;
	struct SymbolEntry *next;
} SymbolEntry;

typedef struct Scope {
	SymbolEntry *symbolList;
	int scopeID;
	struct Scope *parent;
	struct Scope *child;
} Scope;

typedef struct SymbolTable {
	Scope *currentScope;
} SymbolTable;


SymbolTable *createSymbolTable();
void enterScope(SymbolTable *table);
void exitScope(SymbolTable *table);
void addSymbol(SymbolTable *table, const char *name, SymbolKind kind, int datatype);
SymbolEntry *findSymbol(SymbolTable *table, const char *name);


#endif