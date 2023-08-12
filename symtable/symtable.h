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
	char name[128];		// we can malloc this for memory-efficency
	DataTypes datatype;
	int decl_status;	// 0 = uninitialised, 1 = initialized
	int lineno;
	int linepos;
	// intlitdata, strlitdata
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
void addSymbol(SymbolTable *table, const char *name, SymbolKind kind, int datatype, int decl_status, int lineno, int linepos);
SymbolEntry *findSymbol(SymbolTable *table, const char *name);


#endif