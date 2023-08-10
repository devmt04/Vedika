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


extern char filename[128];


void write_ircode(char ircode[4096]);

void irgen_glob_int_var_decl(int decl_status, char *idName, int data){
	/*
	|| alloc_glob int a
	|| mov int-data, a
	*/

	/* Later we will change mov with something like
	movq or movw according to size of datatype used */

	/* THIS MAY BE POSSIBLE THAT IN FUTURE WE 
	   CUSTOMIZE OUR COMPILER IN SUCH A WAY THAT
	   IT SUPPOSED TO FETCH GLOBAL VAR ONLY FROM
	   A SPECIFIC BLOCK OF CODE LIKE IN ASSEMBLY
	   WE HAVE .section .data PORTION */

	char ircode_buf[4096] = {0};
	char ircode[1024] = {0};

	if (decl_status == 0){
		// UN-INITILIZED
		sprintf(ircode, "alloc_glob int %s\n", idName);
		int size_ircode = strlen(ircode); 
		memcpy(ircode_buf, ircode, size_ircode);
		ircode_buf[size_ircode+1] = '\0';
	}else{
		sprintf(ircode, "alloc_glob int %s\n", idName);
		int size_ircode = strlen(ircode); 
		memcpy(ircode_buf, ircode, size_ircode);

		sprintf(ircode, "mov %d, %s\n", data, idName);
		memcpy(ircode_buf + size_ircode, ircode, strlen(ircode));
		ircode_buf[size_ircode+strlen(ircode)+1] = '\0';
	}
    write_ircode(ircode_buf);
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