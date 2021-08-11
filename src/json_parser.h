#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "json_validator.h"
#include <stdio.h>

typedef enum value_type {
    string,     number,     object,
    array,      true,       false,
    null,       comma
} value_type;

typedef struct value {

    value_type type;
    void* value;
    int v_len;

    int set_size;   
    struct value** set;

    struct value* next;

} value;

typedef struct parser {

    FILE* json;
    value* curr;
    stack_v* stk;
    short cont;

} parser;

typedef struct string_buffer {
    char* buffer;
    int size;
    int offset;
} string_buffer;

value* parse_json( FILE* fp );

void dispose( value* ptr );

value* object_get( value* obj, char* key );

value* array_get( value* arr, int index );

int number_as_int( value* ptr );

float number_as_float( value* ptr );

char* to_string( value* ptr );

#endif