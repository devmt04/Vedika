/*
Current Progress :

Detect only INTLIT, KEYWORDS, IDENTIFIERS, NEWLINE 
 WHITESPACES, =


Detected Keywords :
int

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

char last_read_char = -1;

char scan(){
	if (last_read_char != -1) {
        char c = last_read_char;
        last_read_char = -1;  // Reset the value
        return c;
    }

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


void unget_buffered(char c) {
    if (last_read_char == -1) {
        last_read_char = c;
    } else {
        // Handle the case where you're attempting to unget more than one character
    }
}


// classify given string as Keyword or Not a keyword
//int identify_keyword(char buf[128]){
int identify_keyword(char *buf){
	// INT
	switch (*buf){
		case 'i':
			if(memcmp(buf, "int", 3) == 0){
				return KW_INT;
			}
		case 'd':
			if(memcmp(buf, "def", 3)==0){
				return KW_DEF;
			}
		case 'r':
			if(memcmp(buf, "return", 6)==0){
				return KW_RET;
			}
		case 'e':
			if(memcmp(buf, "enddef", 6)==0){
				return KW_ENDDEF;
			}
		
		default:
			return KW_NOTFOUND;
	}
}


Token *new_token(){
	int offset = 0; // offset for linepos, as linepos represent only
					// pos of 1st charcter of lexeme

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
	
	else if(lexeme == '('){
		linepos++;

		token->kind = TK_LPAREN;
		token->linepos = linepos;
		token->lineno = lineno;

		linepos++;
		offset=0;

		return token;
	}

	else if(lexeme == ','){
		linepos++;

		token->kind = TK_COMMA;
		token->linepos = linepos;
		token->lineno = lineno;

		linepos++;
		offset=0;

		return token;
	}

	else if(lexeme == ')'){
		linepos++;

		token->kind = TK_RPAREN;
		token->linepos = linepos;
		token->lineno = lineno;

		linepos++;
		offset=0;

		return token;
	}

	else if(lexeme == ':'){
		linepos++;

		token->kind = TK_COLON;
		token->linepos = linepos;
		token->lineno = lineno;

		linepos++;
		offset=0;

		return token;
	}

	else if(lexeme == ';'){
		linepos++;

		token->kind = TK_SEMICOLON;
		token->linepos = linepos;
		token->lineno = lineno;

		linepos++;
		offset=0;

		return token;
	}
	
	else if(lexeme == '='){
		linepos++;

		token->kind = TK_ASOP_EQ;
		token->linepos = linepos;
		token->lineno = lineno;

		linepos++;
		offset=0;

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
				unget_buffered(next);
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
		int buffsize = 0;

		char buf[128] = {0};//max length of id/kw assumed to be 128 char
		
		for(int i = 0;; i++){
			if(isalnum(lexeme) || '_' == lexeme){
				buf[i] = lexeme;
				lexeme = scan();
				offset++;
				buffsize++;
			}else{
				buf[i] = '\0'; //end of lexeme
				buffsize++;
				//bufflen++;
				// putback charcter
				unget_buffered(lexeme);
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
			
			//strcpy(token->name, buf);
			memcpy(token->name, buf, buffsize);
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