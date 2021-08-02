
CC = gcc

json_parser.o : json_parser.c json_parser.h
	$(CC) -c -g $^

test_json_parser : test_json_parser.c json_parser.o
	$(CC) $^ -o $@.out

PROXY clean : 
	rm *.o *.out *.h.gch