#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "json_lexer.h"

typedef enum value_type {
    string, number, object, object_pair, array, boolean, null 
} value_type;

typedef struct value {

    value_type type;

    void* key; // string, number, boolean, null, object_pair
    int key_len;

    value** val; // object, array
    int val_len;

    int depth;

} value;

value* parse(lexeme* head);

#endif