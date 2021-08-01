#include "json_lexer.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

FILE* json;
lexeme* curr;

int lex ();

void fold( lexeme* ptr ) {

    if ( ptr -> set_size ) {

        ptr -> set = malloc( ptr -> set_size * sizeof(lexeme*));
        lexeme* prev = ptr, *tmp = ptr -> next;

        for ( int i = 0; i < ptr -> set_size; ++i ) {

            ptr -> set[i] = tmp;
            prev -> next = NULL;

            do { 
                prev = tmp;
                tmp = tmp -> next; 
            } while ( prev -> type != comma && tmp != curr );
        }

        prev -> next = NULL;
        ptr -> next = curr;
    }
}

int lex_collection() {

    lexeme* tmp = curr;
    tmp -> set_size = lex();
    fold(tmp);
    return 1 + lex();
}

int lex_string() {

    int size = 0, len = 0;
    char* str = NULL;
    char c;

    escape:
    while ( (c = (char) fgetc(json)) != '"' && c != EOF ) {
        if ( len + 1 >= size ) {
            size = (size * 2) + 1;
            str = realloc(str, sizeof(char) * size);
        }
        str[len++] = c;
    }
    if ( len ) {
        if ( str[len-1] == '\\' && c != EOF ) {
            str[len++] = c;
            goto escape;
        }
        str[len] = '\0';
    }

    curr -> type = string_t;
    curr -> value = str;
    curr -> set_size = len;

    return 1 + lex();
}

int lex_comma() {
    curr -> next = (lexeme*) malloc(sizeof(lexeme));
    curr = curr -> next;
    curr -> type = comma;
    return lex();
} 

int lex_number( char c ) {

    curr -> type = number;
    curr -> value = (double*) malloc(sizeof(double));
    double n = 0.0, p = 1.0;

    while ( isdigit(c) ) {

        n = (n * 10.0) + ( (int) (c - '0') );
        c = (char) fgetc(json);
    }
    
    if ( c == '.' ) { c = (char) fgetc(json); }

    while ( isdigit(c) ) {

        n = (n * 10.0) + ( (int) (c - '0') );
        p *= 10.0;
        c = (char) fgetc(json);
    }

    *((double*)curr -> value) = n / p;
    return ( c == ',' ) ? 1 + lex_comma() : 1 + lex();
}

int lex_phrase( char c ) {

    if ( c == 't' ) {
        curr -> type = true;

    } else if ( c == 'f' ) {
        curr -> type = false;

    } else {
        curr -> type = null;
    }

    while ( c >= 'a' && c <= 'z' ) { c = (char) fgetc(json); }

    return ( c == ',' ) ? 1 + lex_comma() : 1 + lex();

}

int lex () {

    curr -> next = (lexeme*) malloc(sizeof(lexeme));
    curr = curr -> next;

    char c = (char) fgetc(json);

    while ( isspace(c) ) { c = (char) fgetc(json); }

    switch ( c ) {
        case '{':
            curr -> type = object;
            return lex_collection();
        
        case '[':
            curr -> type = array;
            return lex_collection();

        case '}':
        case ']':
        case EOF:
            curr -> type = end;
            return 0;

        case '"':
            return lex_string();

        case ':':
            curr -> type = op;
            return lex() - 1;
        
        case ',':
            curr -> type = comma;
            return lex();

        default:
            if ( isdigit(c) ) {
                return lex_number( c );
            } else {
                return lex_phrase( c );
            }
        break;
    }
}

int lex_json( lexeme* head, FILE* fp ) {
    curr = head;
    json = fp;
    return lex();
}