#include "json_parser.h"
#include "json_validator.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int parse( parser* psr );
int parse_char( parser* psr, char c );

void fold( parser* psr, value* ptr ) {

    if ( psr -> cont && ptr -> set_size ) {
        ptr -> set = calloc( ptr -> set_size, sizeof(value*) );
        value* prev = ptr, *tmp = ptr -> next;

        for ( int i = 0; i < ptr -> set_size; ++i ) {

            prev -> next = NULL;
            ptr -> set[i] = tmp;
            
            while ( tmp != psr -> curr ) {
                prev = tmp;
                tmp = tmp -> next;

                if ( tmp -> type == comma ) {
                    tmp = tmp -> next;
                    free( prev -> next );
                    break;
                }
            }
        }
        tmp -> next = NULL;

    } else if ( !psr -> cont ) { 
        ptr -> set_size = 0;
        if ( ptr -> next ) {
            dispose( ptr -> next );
        }
        ptr -> next = NULL;
    }
    psr -> curr = ptr;
}

int parse_collection( parser* psr ) {
    value* tmp = psr -> curr;
    tmp -> set_size = parse( psr );
    fold( psr, tmp );
    return 1 + parse( psr );
}

int parse_string( parser* psr ) {

    psr -> curr -> type = string;
    psr -> curr -> v_len = 0;
    int size = 0;
    char c;

    escape:
    while ( (c = (char) fgetc( psr -> json )) != '"' && c != EOF ) {
        if ( psr -> curr -> v_len + 1 >= size ) {
            size = (size * 2) + 2;
            psr -> curr -> value = realloc( psr -> curr -> value, size * sizeof(char) );
        }
        ((char*)psr -> curr -> value)[psr -> curr -> v_len++] = c;
    }
    if ( psr -> curr -> v_len ) {
        if ( ((char*)psr -> curr -> value)[psr -> curr -> v_len-1] == '\\' && c != EOF ) {
            ((char*)psr -> curr -> value)[psr -> curr -> v_len++] = c;
            goto escape;
        }
        ((char*)psr -> curr -> value)[psr -> curr -> v_len] = '\0';
    }
    return 1 + parse( psr );
}

int clean( parser* psr ) {
    psr -> cont = 0;
    return 0;
}

char build_number( parser* psr, char c, int* size ) {

    while ( isdigit(c) ) {
        if ( psr -> curr -> v_len + 2 >= *size ) {
            *size = (*size + 1) * 2;
            psr -> curr -> value = realloc( psr -> curr -> value, (*size) * sizeof(char) );
        }
        ((char*)psr -> curr -> value)[psr -> curr -> v_len++] = c;
        c = (char) fgetc( psr -> json );
    }
    return c;
}

int parse_number( parser* psr, char c ) {

    psr -> curr -> type = number;
    psr -> curr -> v_len = 0;
    int size = 3;

    psr -> curr -> value = (char*) calloc( size, sizeof(char) );
    ((char*)psr -> curr -> value)[psr -> curr -> v_len++] = c;

    c = (char) fgetc( psr -> json );
    c = build_number( psr, c, &size );

    if ( c == '.' ) { 
        ((char*)psr -> curr -> value)[psr -> curr -> v_len++] = c; 
        c = (char) fgetc( psr -> json ); 
    }

    c = build_number( psr, c, &size );
    ((char*)psr -> curr -> value)[psr -> curr -> v_len] = '\0';
    if ( psr -> curr -> v_len == 1 && ((char*)psr -> curr -> value)[0] == '-' ) { clean( psr ); }
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

int parse_colon( parser* psr ) {
    psr -> curr -> set_size = -1;
    return parse( psr ) - 1;
}

int parse( parser* psr ) {
    char c = (char) fgetc( psr -> json );
    return parse_char(psr, c);
}

int parse_char( parser* psr, char c ) {

    while ( isspace(c) ) { c = (char) fgetc( psr -> json ); }

    switch ( c ) {
        case ':' :  return ( is_valid( psr -> stk, col ) ) ? parse_colon( psr ) : clean( psr );
        case '}' :  return ( is_valid( psr -> stk, ob_e ) ) ? 0 : clean( psr );
        case ']' :  return ( is_valid( psr -> stk, ar_e ) ) ? 0 : clean( psr );
        case EOF :  return ( is_valid( psr -> stk, end ) ) ? 0 : clean( psr );
    }

    psr -> curr -> next = (value*) calloc(1, sizeof(value));
    psr -> curr = psr -> curr -> next;   

    switch ( c ) {
        case '{':   psr -> curr -> type = object;
                    return ( is_valid( psr -> stk, ob_i ) ) ? parse_collection( psr ) : clean( psr );
        
        case '[':   psr -> curr -> type = array;
                    return ( is_valid( psr -> stk, ar_i ) ) ? parse_collection( psr ) : clean( psr );

        case '"':   return ( is_valid( psr -> stk, str ) ) ? parse_string( psr ) : clean( psr );

        case ',':   psr -> curr -> type = comma;
                    return ( is_valid( psr -> stk, com_i ) ) ? parse( psr ) : clean( psr );

        default:    if ( isdigit(c) || c == '-' ) {
                        return ( is_valid( psr -> stk, v ) ) ? parse_number( psr, c ) : clean( psr );
                    }
                    return ( is_valid( psr -> stk, v ) ) ? parse_phrase( psr, c ) : clean( psr );
    }
}

value* parse_json( FILE* fp ) {

    value head;
    parser psr = { fp, &head, create_stack( 256 ), 1 };

    parse( &psr );
    dispose_stack( psr.stk );

    if ( psr.cont ) {
        return head.next;
    }
    dispose( head.next );
    return NULL;
}

void dispose( value* ptr ) {
    
    if ( ptr -> type < object ) {
        free ( ptr -> value ); 
        ptr -> value = NULL;
    }
    if ( ptr -> set_size > 0 ) {

        for ( int i = 0; i < ptr -> set_size; ++i ) {
            dispose( ptr -> set[i] );
            ptr -> set[i] = NULL;
        }
        free( ptr -> set );
        ptr -> set = NULL;
    }
    if ( ptr -> next ) {

        dispose ( ptr -> next );
        ptr -> next = NULL;
    }
    free( ptr );
}

value* object_get( value* obj, const char* key ) {

    if ( obj -> set_size && obj -> type == object ) {
        for ( int i = 0; i < obj -> set_size; ++i ) {
            if ( ! strcmp( obj -> set[i] -> value, key ) ) {
                return obj -> set[i] -> next;
            }
        }
    }
    return NULL;
}

value* array_get( value* arr, int index ) {

    if ( index >= 0 && index < arr -> set_size && arr -> type == array ) {
        return arr -> set[index];
    }
    return NULL;
}

int resize_and_append( string_buffer* s_buffer, int len_append, const char* format, void* to_append ) {

    short alloc = 0;

    while ( s_buffer -> size <= s_buffer ->offset + len_append ) {
        s_buffer -> size = (s_buffer -> size + 1) * 2;
        alloc = 1;
    }
    if ( alloc ) {
        s_buffer -> buffer = realloc( s_buffer -> buffer, s_buffer ->size );
    }
    if ( s_buffer -> buffer ) {

        s_buffer -> offset += snprintf( &(s_buffer -> buffer[s_buffer -> offset]), (s_buffer -> size - s_buffer ->offset), format, to_append );
        return 0;
    }
    return 1;
}

int add_value( value* ptr, string_buffer* s_buffer ) {

    switch ( ptr -> type ) {
        case string : resize_and_append( s_buffer, 1, "%s", "\"" );
        case number : return resize_and_append( s_buffer, ptr -> v_len, "%s", ptr -> value );
    
        case object : return resize_and_append( s_buffer, 1, "%s", "{" );
    
        case array : return resize_and_append( s_buffer, 1, "%s", "[" );
    
        case true : return resize_and_append( s_buffer, 4, "%s", "true" );
    
        case false : return resize_and_append( s_buffer, 5, "%s", "false" );
    
        case null : return resize_and_append( s_buffer, 4, "%s", "null" );
        
        default: return 1;
    }
}

int build_string( value* ptr, string_buffer* s_buffer ) {

    int v = add_value( ptr, s_buffer );

    if ( ptr -> type == string ) {

        v &= resize_and_append( s_buffer, 1, "%s", "\"" );

        if ( ptr -> set_size == -1 ) {
            v &= resize_and_append( s_buffer, 1, "%s", ":" );
            v &= build_string( ptr -> next, s_buffer );
        }

    } else if ( ptr -> set_size ) {

        int i = 0;
        for ( ; i < ptr -> set_size-1; ++i ) {
            v &= build_string( ptr -> set[i], s_buffer );
            v &= resize_and_append( s_buffer, 1, "%s", "," );
        }
        v &= build_string( ptr -> set[i], s_buffer );
    }

    if ( ptr -> type == object ) {
        v &= resize_and_append( s_buffer, 1, "%s", "}" );
    }
    if ( ptr -> type == array ) {
        v &= resize_and_append( s_buffer, 1, "%s", "]" );
    }

    return v;
}

char* to_string( value* ptr ) {

    string_buffer b = { NULL, 0, 0 };
    if ( !build_string( ptr, &b ) ) {
        return b.buffer;
    }
    free(b.buffer);
    return NULL;
}

int number_as_int( value* ptr ) {

    if ( ptr -> type != number ) { return 0; }

    int i = 0, n = 0, neg = 1;
    char c = ((char*)ptr -> value)[i++];
    if ( c == '-' ) { neg = -1; c = ((char*)ptr -> value)[i++]; }

    while ( isdigit(c) ) {

        n = (n * 10.0) + ( (int) (c - '0') );
        c = ((char*)ptr -> value)[i++];
    }
    return n * neg;
}

float number_as_float( value* ptr ) {

    if ( ptr -> type != number ) { return 0.0; }

    int i = 0;
    float n = 0.0, p = 1.0;
    char c = ((char*)ptr -> value)[i++];
    if ( c == '-' ) { c = ((char*)ptr -> value)[i++]; p = -1.0; }

    while ( isdigit(c) ) {

        n = (n * 10.0) + ( (int) (c - '0') );
        c = ((char*)ptr -> value)[i++];
    }
    
    if ( c == '.' ) { c = ((char*)ptr -> value)[i++]; }

    while ( isdigit(c) ) {

        n = (n * 10.0) + ( (int) (c - '0') );
        p *= 10.0;
        c = ((char*)ptr -> value)[i++];
    }
    return (float) n / p;
}