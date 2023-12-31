#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"

typedef enum {
	TREE_PROGRAM = 0, //TOP-NODE
	TREE_VARDECL = 1,
	TREE_INITVAR = 2,
} TreeType;

typedef enum {
	DT_NA = 0, // uninitialized
	DT_INT = 1,
} DataTypes;


typedef enum {
	LF_KW = 1, 
	LF_ID = 2, 
	LF_INTLIT = 3, 
	LF_ASOP_EQ = 4, 
	LF_AROP_PLUS = 5,
	//LF_NEWLINE = 6,
	//LF_EOF = 0
	LF_DECL_TERM = 0 //declaration termination
} LeafType;

typedef struct Node {
	union {
		TreeType TType; // when Node is of 1st number, i.e, node that represent whole subtree
		LeafType LType; // Underlying Nodes
	};
	
	union { 
		int value; // use long long instead of int
		char name[128]; //for ID's name
    }; 

    int lineno;
    int linepos;
    
}Node;

typedef struct ParseTree {
	Node *termnode;
	struct ParseTree *nontermnode;
} ParseTree;

ParseTree *Parser(Token *token);

#endif