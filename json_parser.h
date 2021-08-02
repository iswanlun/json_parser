#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <stdio.h>

typedef enum value_type {
    string_t,   number,     object,
    array,      true,       false,
    null,       comma
} value_type;

typedef struct value {

    value_type type;
    void* value;

    int set_size;   
    struct value** set;

    struct value* next;

} value;

value* parse_json( FILE* fp );

void dispose( value* ptr );

value* object_get( value* obj, char* key );

value* array_get( value* arr, int index );

#endif