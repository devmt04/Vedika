#ifndef LEXER_H
#define LEXER_H

//#include <stdio.h> 

typedef enum {
	TK_KW = 1, 
	TK_ID = 2, 
	TK_INTLIT = 3, 
	TK_ASOP_EQ = 4, 
	TK_AROP_PLUS = 5,
	TK_NEWLINE = 6,
	TK_EOF = 0
} TokenKind;



enum Keywords { 
	KW_NOTFOUND = 0, //if given lexeme is not a Keyword
	KW_INT = 1,
};


typedef struct Token {
	TokenKind kind;
	//struct Token *next;	// require for parser
	union { // use union as these can never access simoultanously
		int value; // use long long instead of int
		char name[128]; //for ID's name
    }; //size of union will be size of largest member in it
     int lineno;
     int linepos;
} Token;


// funtion prototypes
Token *lexer();

#endif