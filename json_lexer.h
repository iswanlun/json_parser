#ifndef JSON_LEXER_H
#define JSON_LEXER_H

#include <stdio.h>

typedef enum lexeme_type {
    string_t,   number,     object,
    array,      true,       false,
    null,       op,         end,
    comma
} lexeme_type;

typedef struct lexeme {

    lexeme_type type;

    int set_size;

    void* value;
    void** set;

    struct lexeme* next;

} lexeme;

int lex_json( lexeme* head, FILE* fp );

int disose( lexeme* head );

#endif