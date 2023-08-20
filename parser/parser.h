#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"

typedef enum {
	TREE_PROGRAM = 0, //TOP-NODE
	TREE_VARDECL = 1,
	TREE_INITVAR = 2,
	TREE_FUNDECL = 3,
} TreeType;

typedef enum {
	DT_NA = 0, // uninitialized
	DT_INT = 100,
} DataTypes;

typedef enum {
	LF_KW = 1, 
	LF_ID = 2, 
	LF_INTLIT = DT_INT, 
	LF_ASOP_EQ = 4, 
	LF_AROP_PLUS = 5,
	
	LF_LPAREN = 6, 
	LF_RPAREN = 7,  
	LF_COLON = 8,  
	LF_SEMICOLON = 9, 
	LF_COMMA = 10,  // WILL NOT USED IN FUNTION PARSER TREE

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
	
	union{
		Node *termnode;
		struct ParseTree *subnontermnode;
	};
	union{
		Node *subtermnode;
		struct ParseTree *nontermnode;
	};
	
} ParseTree;

ParseTree *Parser(Token *token);

#endif