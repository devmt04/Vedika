/*
Current Progress :

Detect only INTLIT, KEYWORDS, IDENTIFIERS, NEWLINE 
and WHITESPACES


Detected Keywords :
INT

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ctype.h>
#include "lexer.h"

#define BUFFER_SIZE 4096

extern FILE *fp;

extern int lineno;
extern int linepos;

char buffer[BUFFER_SIZE];
size_t bufferPos = 0; // Current position in the buffer
size_t bytesRead = 0; // Number of bytes read into the buffer


char scan(){
	// if the buffer is empty, read more data from the file
	if(bufferPos >= bytesRead){
		bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
		bufferPos = 0;
	}

	//if no more data is avilable, return EOF
	if(bytesRead == 0){
		return EOF;
	}

	return buffer[bufferPos++];
}


// classify given string as Keyword or Not a keyword
int identify_keyword(char buf[128]){
	// INT
	switch (*buf){
		case 'i':
			if(strcmp(buf, "int") == 0){
				return KW_INT;
			}
		default:
			return KW_NOTFOUND;
	}
}


Token *new_token(){
	int offset = 0; //offset for linepos, as linepos contain only
					// start pos of lexeme

	char lexeme = scan();
	Token *token = (Token *)malloc(sizeof(Token));

	// Start - Handling whitespaces
	while(1){
		switch (lexeme){
			case ' ':
				linepos++;
				lexeme = scan();
				continue;
			case '\t':
				linepos += sizeof('\t');
				lexeme = scan();
				continue;
		}
		break;
	}// End - Handling whitespaces
	if(lexeme == '\n'){
		/*
		In our case, we treat a newline charcter
		as charcter for line-termination, so it is 
		also considered as a token/
		*/
		lineno++;
		linepos = 0;

		token->kind = TK_NEWLINE;
		return token;
	}

	// Start - Handlinf Integers Literal
	else if (isdigit(lexeme)){
		int value = lexeme - '0';

		linepos++;

		for(;;){
			char next = scan();
			if(isdigit(next)){
				value = (value*10) + (next - '0');
				offset++;
			}else if(next == ' ' || next == '\t' || next == '\n' || next == EOF){
				token->kind = TK_INTLIT;
				token->value = value;
				token->linepos = linepos;
				token->lineno = lineno;

				linepos+=offset-1;
				offset=0;

				// Putback charcter that we fetched in advanced
				// to check if next carcter is also a intlit
				// or something eles
				ungetc(next, fp);
				return token;
			}else{
				// there is no token which start with digits
				// and has something else in between
				printf("ERROR-invalid token type!");
				exit(-1);
			}
		}
	} // End - Handlinf Integers Literal


	/* Start - KW and ID */
	else if(isalpha(lexeme) || '_' == lexeme){

		linepos++;
		
		char buf[128] = {0};//max length assumed to be 128 char
		
		for(int i = 0;; i++){
			if(isalnum(lexeme) || '_' == lexeme){
				buf[i] = lexeme;
				lexeme = scan();
				offset++;
			}else{
				buf[i] = '\0'; //end of lexeme
				//bufflen++;
				// putback charcter
				ungetc(lexeme, fp);
				break;
			}
		}

		// if kw_status == 0, then it will not be a keyword
		int kw_status = identify_keyword(buf);
		if( kw_status > 0){
			// It is a keyword
			token->kind = TK_KW;
			token->value = kw_status;
			token->linepos = linepos;
			token->lineno = lineno;

			linepos += offset-1;
			offset = 0;
			return token;
		}else{
			// kw_status == 0 : Not a keyword
			// it have to be a identifer
			// we will add it to symbol table in parsing phase
			token->kind = TK_ID;
			strcpy(token->name, buf);
			token->linepos = linepos;
			token->lineno = lineno;

			linepos += offset - 1;
			offset = 0;
			
			return token;
		}
	}

	else if(lexeme==EOF){
		token->kind = TK_EOF;
		return token;
	}
}

Token *lexer(){
	return new_token();
}
