
CC = gcc

json_parser.o : json_parser.c json_parser.h
	$(CC) -c $^

test_json_parser : test_json_parser.c json_parser.o
	$(CC) $^ -o $@.out

PROXY clean : 
	rm *.o \
	rm *.out \
	rm *.h.gch