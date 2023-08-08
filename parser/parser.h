#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"

typedef enum {
	TREE_PROGRAM = 0, //TOP-NODE
	TREE_VARDECL = 1,
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
	LF_NEWLINE = 6,
	LF_EOF = 0
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
    // other attr for error traking
}Node;

typedef struct ParseTree {
	Node *termnode;
	struct ParseTree *nontermnode;
} ParseTree;

// Recusive-decent routine for vardecl

typedef struct TreeReturnNode {
	ParseTree *firstnode;
//	ParseTree *tree2nd;
	ParseTree *lastnode;
} TreeReturnNode;

TreeReturnNode *var_decl(Token *token, DataTypes vartype);
TreeReturnNode *Parser(Token *token);

#endif