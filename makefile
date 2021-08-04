
CC = gcc

json_validator.o : json_validator.c json_validator.h
	$(CC) -c -g $^

json_parser.o : json_parser.c json_parser.h json_validator.o
	$(CC) -c -g $^

test_json_parser : test_json_parser.c json_parser.o json_validator.o
	$(CC) $^ -o $@.out

PROXY clean : 
	rm *.o *.out *.h.gch