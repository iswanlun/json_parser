#include "json_parser.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

FILE* json;
value* curr;

int parse();

void fold( value* ptr ) {
    if ( ptr -> set_size ) {
        ptr -> set = malloc( ptr -> set_size * sizeof(value*));
        value* prev = ptr, *tmp = ptr -> next;

        for ( int i = 0; i < ptr -> set_size; ++i ) {

            ptr -> set[i] = tmp;
            prev -> next = NULL;

            do { 
                prev = tmp;
                tmp = tmp -> next;
                if ( tmp -> type == comma ) {
                    tmp = tmp -> next;
                    free(prev -> next);
                    break;
                }
            } while ( tmp != curr );
        }
        prev -> next = NULL;
        ptr -> next = curr;
    }
}

int parse_collection() {
    value* tmp = curr;
    tmp -> set_size = parse();
    fold(tmp);
    return 1 + parse();
}

int parse_string() {
    int size = 0, len = 0;
    char* str = NULL, c;

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
    return 1 + parse();
}

int parse_comma() {
    curr -> next = (value*) malloc(sizeof(value));
    curr = curr -> next;
    curr -> type = comma;
    return parse();
} 

int parse_number( char c ) {
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
    return ( c == ',' ) ? 1 + parse_comma() : 1 + parse();
}

int parse_phrase( char c ) {
    if ( c == 't' ) {
        curr -> type = true;

    } else if ( c == 'f' ) {
        curr -> type = false;

    } else {
        curr -> type = null;
    }

    while ( c >= 'a' && c <= 'z' ) { c = (char) fgetc(json); }
    return ( c == ',' ) ? 1 + parse_comma() : 1 + parse();
}

int parse () {

    char c = (char) fgetc(json);
    while ( isspace(c) ) { c = (char) fgetc(json); }

    if ( c == ':' ) { return parse() - 1; }

    curr -> next = (value*) malloc(sizeof(value));
    curr = curr -> next;   

    switch ( c ) {
        case '{':   curr -> type = object;
                    return parse_collection();
        
        case '[':   curr -> type = array;
                    return parse_collection();

        case '"':   return parse_string();

        case ',':   curr -> type = comma;
                    return parse();

        case '}':
        case ']':
        case EOF:   curr -> type = end;
                    return 0;

        default:    if ( isdigit(c) ) {
                        return parse_number( c );
                    } else {
                        return parse_phrase( c );
                    }
    }
}

value* parse_json( FILE* fp ) {
    value head;
    curr = &head;
    json = fp;
    parse();
    return head.next;
}