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

#include "../parser/parser.h"

extern char filename[128];

void irgen_glob_var_int(ParseTree *parsetree);
void write_ircode(char ircode[4096]);
void IRGen(ParseTree *tree);

void IRGen(ParseTree *tree){
	// Identify type of declaration or statement or codeblock
	
	/* a var decl can be identified, if it starts with
	 	a datatye kind keyword like int. */  

	if(tree->termnode->TType == TREE_VARDECL){
		switch (tree->termnode->value){
			case DT_INT:
				/* If the TType == TREE_FUNC_DECL
				then we declare local variables,
				till then we are in global scope so we have
				to declare global var */
				irgen_glob_var_int(tree);
				break;
			default:
				printf("IRGen error");

		}
	}

}



void irgen_glob_var_int(ParseTree *parsetree){
	/*
	|| alloc_glob int a
	|| mov intlit, a
	*/

	/* Later we will change mov with something like
	movq or movw according to size of datatype used */

	/* THIS MAY BE POSSIBLE THAT IN FUTURE WE 
	   CUSTOMIZE OUR COMPILER IN SUCH A WAY THAT
	   IT SUPPOSED TO FETCH GLOBAL VAR ONLY FROM
	   A SPECIFIC BLOCK OF CODE LIKE IN ASSEMBLY
	   WE HAVE .section .data PORTION */

	ParseTree *tree = parsetree;

	/* AT PRESENT WE ASSUMED THE GLOBAL VAR ARE
	   ALREADY INITIALIZED BY PROGRAMMER */

	char ircode_buf[4096] = {0};
	
	char idName[strlen(tree->nontermnode->termnode->name)];
	memcpy(idName, tree->nontermnode->termnode->name, sizeof(idName));
	char ircode[1024] = {0};

    sprintf(ircode, "alloc_glob int %s\n", idName);
    int size_ircode = sizeof(ircode);
    memcpy(ircode_buf, ircode, size_ircode);

    int data = tree->nontermnode->nontermnode->nontermnode->termnode->value;
    sprintf(ircode, "mov %d, %s\n", data, idName);
    memcpy(ircode_buf + size_ircode, ircode, sizeof(ircode));


    write_ircode(ircode_buf);


}


void write_ircode(char ircode[4096]){

	FILE *file = fopen("ir.code", "a+");

    if (file == NULL) {
        printf("Error opening file");
        exit(-1);
    }
    fprintf(file, "%s", ircode);
    fclose(file);
}