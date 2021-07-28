#include <stdio.h>
#include <string.h>
#include "json_lexer.h"

char* src;
lexeme* curr;

void construct() {
    curr -> next = (lexeme*) malloc(sizeof(lexeme));
    curr = curr -> next;
}

int lex_object() {
    
    curr -> type = object;
    lexeme* tmp = curr;
    construct();
    src++;

    tmp -> set_size = lex();
    return 1 + lex();
}

int lex_array() {

    curr -> type = object;
    lexeme* tmp = curr;
    construct();
    src++;

    tmp -> set_size = lex();
    return 1 + lex();
}

int lex_string() {

    char* i = ++src; 

    escape:
    while ( *src != '"' ) {
        src++;
    }
    if ( *(src-1) == '\\' ) {
        src++;
        goto escape;
    }

    int size = src - i;
    curr -> type = string_t;
    curr -> value = (char*) malloc( (size + 1) * sizeof(char) );
    strncpy( curr -> value, i, size );
    ((char*) curr -> value)[size] = '\0';
    construct();
    src++;

    return 1 + lex();
}

int lex_colen() {

    ++src;
    return lex() - 1;
} 

int lex_number() {

    curr -> type = number;
    curr -> value = (double*) malloc(sizeof(double));
    double n = 0.0, p = 1.0;

    while ( isdigit(*src) ) {

        n = (n * 10.0) + ( (int) (*src - '0') );
        src++;
    }
    
    if ( *src == '.' ) { src++; }

    while ( isdigit(*src) ) {

        n = (n * 10.0) + ( (int) (*src - '0') );
        p *= 10.0;
        src++;
    }

    *((double*) curr -> value) = n / p;
    construct();
    return 1 + lex();
}

int lex_phrase() {

    if ( !strncmp(src, "true", 4) ) {
        curr -> type = true;
        src += 4;

    } else if ( !strncmp(src, "false", 5) ) {
        curr -> type = false;
        src += 5;

    } else {
        curr -> type = null;
        while ( *src >= 'a' && *src <= 'z' ) { src++; }
    }

    construct();
    return 1 + lex();
}

int lex () {

    while ( isspace(*src) || *src == ',' ) { ++src; }

    switch ( *src ) {
        case '{':
            return lex_object();
        
        case '[':
            return lex_array();

        case '}':
        case ']':
        case EOF:
            return 0;

        case '"':
            return lex_string();

        case ':':
            return lex_colen();

        default:
            if ( isdigit(*src) ) {
                return lex_number();
            } else {
                return lex_phrase();
            }
        break;
    }
}

int lex_json( lexeme* head, char* str ) {
    curr = head;
    src = str;
    lex();
    return 0;
}

int dispose( lexeme* head ) {
    
    lexeme* tmp;
    while ( head != NULL) {
        if ( head -> value != NULL ) {
            free( head -> value );
        }
        tmp = head -> next;
        free(head);
        head = tmp;
    }
    return 0;
}