.DEAFULT_GOAL := vedika

CC = gcc
SRCS = vedika.c lexer/lexer.c parser/parser.c symtable/symtable.c ircodegen/ircodegen.c

vedika : $(SRCS)
	$(CC) $^ -o $@


debug:
	$(CC) -std=c99 -g -o vedika $(SRCS)
	gdb ./vedika