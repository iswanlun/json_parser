#include "json_parser.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

FILE* json;
value* curr;

int parse();
int parse_char( char c );

void fold( value* ptr ) {

    if ( ptr -> set_size ) {
        ptr -> set = malloc( ptr -> set_size * sizeof(value*) );
        value* prev = ptr, *tmp = ptr -> next;

        for ( int i = 0; i < ptr -> set_size; ++i ) {
            ptr -> set[i] = tmp;
            
            while ( tmp != curr ) {
                prev = tmp;
                tmp = tmp -> next;

                if ( tmp -> type == comma ) {
                    tmp = tmp -> next;
                    free( prev -> next );
                    prev -> next = NULL;
                    break;
                }
            }
        }
        tmp -> next = NULL;
    }
    curr = ptr;
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

int parse_number( char c ) {
    curr -> type = number;
    curr -> value = (double*) malloc(sizeof(double));
    double n = 0.0, p = 1.0;
    if ( c == '-' ) { c = (char) fgetc(json); p = -1.0; }

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
    return 1 + parse_char( c );
}

int parse_phrase( char c ) {

    if ( c == 't' ) {
        curr -> type = true;

    } else if ( c == 'f' ) {
        curr -> type = false;

    } else {
        curr -> type = null;
    }

    c = (char) fgetc(json);
    while ( c >= 'a' && c <= 'z' ) { c = (char) fgetc(json); }
    return 1 + parse_char( c );
}

int parse () {
    char c = (char) fgetc(json);
    return parse_char(c);
}

int parse_char( char c ) {

    while ( isspace(c) ) { c = (char) fgetc(json); }

    switch ( c ) {
        case ':' : return parse() - 1;

        case '}' :
        case ']' :
        case EOF :  return 0; 
    }

    curr -> next = (value*) malloc(sizeof(value));
    curr = curr -> next;   

    switch ( c ) {
        case '{':   curr -> type = object;
                    return parse_collection(); // return ( isvalid() ) ? parse_collection() : clean() ;
        
        case '[':   curr -> type = array;
                    return parse_collection();

        case '"':   return parse_string();

        case ',':   curr -> type = comma;
                    return parse();

        default:    if ( isdigit(c) || c == '-' ) {
                        return parse_number( c );
                    }
                    return parse_phrase( c );
    }
}

value* parse_json( FILE* fp ) {
    value head;
    curr = &head;
    json = fp;
    parse();
    return head.next;
}

void dispose( value* ptr ) {

    if ( ptr -> value ) {
        free ( ptr -> value ); 
    }

    if ( ptr -> set_size ) {

        for ( int i = 0; i < ptr -> set_size; ++i ) {

            dispose( ptr -> set[i] );
        }
        free( ptr -> set );

    } else if ( ptr -> next ) {
        dispose( ptr -> next );
    }

    free( ptr );
}

value* object_get( value* obj, char* key ) {

    if ( obj -> set_size ) {
        for ( int i = 0; i < obj -> set_size; ++i ) {
            if ( ! strcmp( obj -> set[i] -> value, key ) ) {
                return obj -> set[i] -> next;
            }
        }
    }
    return NULL;
}

value* array_get( value* arr, int index ) {

    if ( index >= 0 && index < arr -> set_size ) {
        return arr -> set[index];
    }
    return NULL;
}