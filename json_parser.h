#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <stdio.h>

typedef enum value_type {
    string_t,   number,     object,
    array,      true,       false,
    null,       op,         end,
    comma
} value_type;

typedef struct value {

    value_type type;
    void* value;

    int set_size;   
    struct value** set;

    struct value* next;

} value;

value* parse_json( FILE* fp );

int dispose( value* head );

#endif