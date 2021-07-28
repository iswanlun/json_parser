#ifndef JSON_LEXER_H
#define JSON_LEXER_H

typedef enum lexeme_type {
    string_t,   number,     object,
    array,      true,       false,
    null
} lexeme_type;

typedef struct lexeme {

    lexeme_type type;

    int set_size;

    void* value;

    struct lexeme* next;

} lexeme;

int lex_json( lexeme* head, char* string );

int disose( lexeme* head );

#endif