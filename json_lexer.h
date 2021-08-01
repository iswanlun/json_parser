#ifndef JSON_LEXER_H
#define JSON_LEXER_H

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
    value** set;

    struct value* next;

} value;

int lex_json( value* head, FILE* fp );

int disose( value* head );

#endif