/*
The syntax of IR-Code we gonna generate is as follow:

for syntax : int a = 45	
	where a is a global var here, of type int
	the IR-Code will be:


	|| alloc_glob int a
	|| mov 45, a

	alloc_glob statement is used to alloc global variables with following syntax:

	|| alloc_glob type varname

	mov statement is used to assign data to variables with following syntax:

	|| mov data, varname

*/

#include <stdio.h>
#include <string.h>	
#include <stdlib.h>
#include <time.h>

#include "ircodegen.h"
#include "../parser/parser.h"
#include "../analysis/semantic.h"
extern char filename[128];

void irgen_func_decl(ParseTree *tree, int status);
void irgen_glob_int_var_decl(ParseTree *tree, int decl_status);
void irgen_glob_int_var_init(ParseTree *tree, int status);

void codeGen(ParseTree *tree, int status){
	if(tree->termnode->TType == TREE_FUNDECL){
		irgen_func_decl(tree, status);
	}

	else if(tree->termnode->TType == TREE_VARDECL){
		irgen_glob_int_var_decl(tree, status);
	}

	else if(tree->termnode->TType == TREE_INITVAR){
		irgen_glob_int_var_init(tree, status);
	}
}

void write_ircode(char *ircode);

void irgen_func_decl(ParseTree *tree, int status){
	/*
		@function type int
		func:
			push-arg int a
			push-arg int b
			push-arg int c
			...
			alloc int a
			mov 5 a
			ret a
		enddef
	*/

// char *idName, DataTypes returnType, ParseTree *argsTree, ParseTree *declsTree

	int code_size = 0;
	//char ircode_buf[4096] = {0};
	char ircode[1024] = {0};

	sprintf(ircode, "\n@function type %d\n%s:\n", tree->nontermnode->termnode->value, tree->nontermnode->nontermnode->termnode->name);
	//memcpy(ircode_buf, ircode, strlen(ircode));
	write_ircode(ircode);


	// Push arguments in stack
	ParseTree *argNode = tree->nontermnode->nontermnode->nontermnode->nontermnode->subnontermnode;
	while (1){
		memset(ircode, 0, strlen(ircode)+1);
		if(tree->nontermnode->nontermnode->nontermnode->nontermnode->subnontermnode == NULL){
			// If No Args are passsed
			memcpy(ircode, "\tpush-arg NONE\n", strlen("\tpush-arg NONE\n"));
			//memcpy(ircode_buf+code_size, ircode, strlen(ircode));
			code_size = strlen(ircode);
			ircode[code_size+1] = '\0';
			write_ircode(ircode);
			break;
		}

		if(argNode == NULL){
			// If No Args are passsed
			memcpy(ircode, "\tpush-arg STOP\n", strlen("\tpush-arg STOP\n"));
			//memcpy(ircode_buf+code_size, ircode, strlen(ircode));
			code_size = strlen(ircode);
			ircode[code_size+1] = '\0';
			write_ircode(ircode);
			break;
		}

		else {
			sprintf(ircode, "\tpush-arg %d %s\n", argNode->subnontermnode->termnode->value, argNode->subnontermnode->subtermnode->name);
			//memcpy(ircode_buf+code_size, ircode, strlen(ircode));
			write_ircode(ircode);
			ParseTree *prevStmtTree = argNode;
			argNode = argNode->nontermnode;
			free(prevStmtTree->subnontermnode->termnode);
			free(prevStmtTree->subnontermnode->subtermnode);
			free(prevStmtTree->subnontermnode);
			free(prevStmtTree);
			continue;			
		}
	}

	ParseTree *declNode = tree->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->nontermnode->subnontermnode;
	while(1){
		if(declNode == NULL){
			memcpy(ircode, "\nenddef", 7);
			ircode[7] = '\0';
			write_ircode(ircode);
			break;
		}

		if(declNode->subnontermnode->termnode->TType == TREE_FUNDECL){
			printf("ERROR at %d:%d, You cannot declare a funtion inside a funtion\n", declNode->subnontermnode->termnode->lineno, declNode->subnontermnode->termnode->linepos);
			exit(-1);
		}

		ParseTree *prevDeclNode = declNode; 
		int status = semantic_check(declNode->subnontermnode);
		codeGen(declNode->subnontermnode, status);
		declNode = declNode->nontermnode;
		// MEMORY-MANAGEMENT
	}

}



void irgen_glob_int_var_init(ParseTree *tree, int decl_status){
	//	|| mov data, idName char *idName, int data
	char ircode[1024] = {0};
	sprintf(ircode, "\nmov %d, %s\n", tree->nontermnode->nontermnode->termnode->value, tree->termnode->name);
    write_ircode(ircode);

    free(tree->nontermnode->nontermnode->termnode);
    free(tree->nontermnode->nontermnode);
    free(tree->nontermnode->termnode);
    free(tree->nontermnode);
    free(tree->termnode);
    free(tree);
}

void irgen_glob_int_var_decl(ParseTree *tree, int decl_status){
	/*
	|| alloc_glob int a
	|| mov int-data, a
	*/

	/* Later we will change mov with something like
	movq or movw according to size of datatype used */

	/* THIS MAY BE POSSIBLE THAT IN FUTURE WE 
	   CUSTOMIZE OUR COMPILER IN SUCH A WAY THAT
	   IT SUPPOSED TO FETCH GLOBAL V  AR ONLY FROM
	   A SPECIFIC BLOCK OF CODE LIKE IN ASSEMBLY
	   WE HAVE .section .data PORTION */

	char idName[strlen(tree->nontermnode->termnode->name)+1];
	memcpy(idName, tree->nontermnode->termnode->name, sizeof(idName));

	if (decl_status == 0){
		// UN-INITILIZED
		char ircode[1024];
		sprintf(ircode, "\nalloc_glob int %s\n", idName);
		write_ircode(ircode);
		free(tree->nontermnode->termnode);
		free(tree->nontermnode);
		free(tree->termnode);
		free(tree);
	}else{
		// MAKE IT MEMORY-EFFICENT?
		                                                                                                                                          
		char ircode[1024];
		int data = tree->nontermnode->nontermnode->nontermnode->termnode->value;

		sprintf(ircode, "\nalloc_glob int %s\n", idName);
		write_ircode(ircode);

		memset(ircode, 0, strlen(ircode));
		sprintf(ircode, "mov %d, %s\n", data, idName);
		write_ircode(ircode);

		free(tree->nontermnode->nontermnode->nontermnode->termnode);
		free(tree->nontermnode->nontermnode->nontermnode);
		free(tree->nontermnode->nontermnode->termnode);
		free(tree->nontermnode->nontermnode);
		free(tree->nontermnode->termnode);
		free(tree->nontermnode);
		free(tree->termnode);
		free(tree);
	}


}

void write_ircode(char *ircode){
	FILE *file = fopen("ir.code", "a+");

    if (file == NULL) {
        printf("Error opening file");
        exit(-1);
    }
    fprintf(file, "%s", ircode);
    fclose(file);
}


/*
void free_tree(ParseTree *tree){
	ParseTree *prevNode = tree;
	while(1){
		if(prevNode->nontermnode == NULL){
			if(prevNode->subtermnode == NULL){
				break;
			}
			else {
				free(prevNode->subtermnode);
				free(prevNode);
				//continue;
			}
		}
		else if(prevNode->termnode == NULL){
			// then there will be subtermnode
			
		}else{

		}
	}
}

WE WILL USE MULTITHREADING FOR THIS FUNTION
*/