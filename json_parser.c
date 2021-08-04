#include "json_parser.h"
#include "json_validator.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int parse( parser* psr );
int parse_char( parser* psr, char c );

void fold( parser* psr, value* ptr ) {

    if ( ptr -> set_size ) {
        ptr -> set = malloc( ptr -> set_size * sizeof(value*) );
        value* prev = ptr, *tmp = ptr -> next;

        for ( int i = 0; i < ptr -> set_size; ++i ) {
            ptr -> set[i] = tmp;
            
            while ( tmp != psr -> curr ) {
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
    psr -> curr = ptr;
}

int parse_collection( parser* psr ) {
    value* tmp = psr -> curr;
    tmp -> set_size = parse( psr );
    fold(psr, tmp);
    return 1 + parse( psr );
}

int parse_string( parser* psr ) {
    int size = 0, len = 0;
    char* str = NULL, c;

    escape:
    while ( (c = (char) fgetc( psr -> json )) != '"' && c != EOF ) {
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

    psr -> curr -> type = string_t;
    psr -> curr -> value = str;
    return 1 + parse( psr );
}

int parse_number( parser* psr, char c ) {
    psr -> curr -> type = number;
    psr -> curr -> value = (double*) malloc(sizeof(double));
    double n = 0.0, p = 1.0;
    if ( c == '-' ) { c = (char) fgetc( psr -> json ); p = -1.0; }

    while ( isdigit(c) ) {

        n = (n * 10.0) + ( (int) (c - '0') );
        c = (char) fgetc( psr -> json );
    }
    
    if ( c == '.' ) { c = (char) fgetc( psr -> json ); }

    while ( isdigit(c) ) {

        n = (n * 10.0) + ( (int) (c - '0') );
        p *= 10.0;
        c = (char) fgetc( psr -> json );
    }

    *((double*)psr -> curr -> value) = n / p;
    return 1 + parse_char( psr, c );
}

int parse_phrase( parser* psr, char c ) {

    if ( c == 't' ) {
        psr -> curr -> type = true;

    } else if ( c == 'f' ) {
        psr -> curr -> type = false;

    } else {
        psr -> curr -> type = null;
    }

    c = (char) fgetc( psr -> json );
    while ( c >= 'a' && c <= 'z' ) { c = (char) fgetc( psr -> json ); }
    return 1 + parse_char( psr, c );
}

int clean() {
    printf("Clean triggered event.\n");
    return 0;
}

int parse ( parser* psr ) {
    char c = (char) fgetc( psr -> json );
    return parse_char(psr, c);
}

int parse_char( parser* psr, char c ) {

    while ( isspace(c) ) { c = (char) fgetc( psr -> json ); }

    switch ( c ) {
        case ':' :  return ( is_valid( psr -> stk, col ) ) ? parse( psr ) - 1 : clean();

        case '}' :  return ( is_valid( psr -> stk, ob_e ) ) ? 0 : clean();
        case ']' :  return ( is_valid( psr -> stk, ar_e ) ) ? 0 : clean();
        case EOF :  return ( is_valid( psr -> stk, end ) ) ? 0 : clean(); 
    }

    psr -> curr -> next = (value*) malloc(sizeof(value));
    psr -> curr = psr -> curr -> next;   

    switch ( c ) {
        case '{':   psr -> curr -> type = object;
                    return ( is_valid( psr -> stk, ob_i ) ) ? parse_collection( psr ) : clean();
        
        case '[':   psr -> curr -> type = array;
                    return ( is_valid( psr -> stk, ar_i ) ) ? parse_collection( psr ) : clean();

        case '"':   return ( is_valid( psr -> stk, str ) ) ? parse_string( psr ) : clean();

        case ',':   psr -> curr -> type = comma;
                    return ( is_valid( psr -> stk, com_i ) ) ? parse( psr ) : clean();

        default:    if ( isdigit(c) || c == '-' ) {
                        return ( is_valid( psr -> stk, v ) ) ? parse_number( psr, c ) : clean();
                    }
                    return ( is_valid( psr -> stk, v ) ) ? parse_phrase( psr, c ) : clean();
    }
}

value* parse_json( FILE* fp ) {

    value head;
    parser psr;

    psr.curr = &head;
    psr.json = fp;
    psr.stk = create_stack( 256 );

    parse( &psr );
    dispose_stack( psr.stk );

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