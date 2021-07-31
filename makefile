
CC = gcc

json_parser.o : json_parser.c json_parser.h
	$(CC) -c $^

test_json_parser : test_json_parser.c json_parser.o
	$(CC) $^ -o $@.out

json_lexer.o : json_lexer.c json_lexer.h
	$(CC) -c -g $^

test_json_lexer : test_json_lexer.c json_lexer.o
	$(CC) $^ -o $@.out

PROXY clean : 
	rm *.o \
	rm *.out \