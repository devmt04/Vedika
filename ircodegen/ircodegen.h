#ifndef IRCODEGEN_H
#define IRCODEGEN_H

#include "../parser/parser.h"

void irgen_glob_int_var_decl(int decl_status, char *idName, int data);
void irgen_glob_int_var_init(char *idName, int data);
void irgen_func_decl(char *idName, DataTypes returnType, ParseTree *argsTree, ParseTree *declsTree);
#endif