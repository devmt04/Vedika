.DEAFULT_GOAL := vedika

CC = gcc
SRCS = vedika.c lexer/lexer.c

vedika : $(SRCS)
	$(CC) $^ -o $@