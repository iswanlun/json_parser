#include "json_parser.h"
#include <stdlib.h>
#include <stdio.h>

char* enum_names[] = {
    "string_t", "number", "object", 
    "array", "true", "false", "null", 
    "end", "comma"
};

void print_value( value* head, int depth ) {

    for ( int i = 0; i < depth; ++i ) {
        printf("  ");
    }

    printf("%s : ", enum_names[head -> type]);

    switch (head -> type) {
        case string_t :
            printf("%s", ((char*) head -> value));
            break;
        case number :
            printf("%f", *((double*) head -> value));
            break;
        case object :
            printf("{ (%d)", head -> set_size );
            break;
        case array :
            printf("[ (%d)", head -> set_size );
            break;
        case true :
            printf("true");
            break;
        case false :
            printf("false");
            break;
        case null :
            printf("null");
            break;
        case end :
            printf(" ]/}");
            break;
        case comma :
            printf(",");
            break;
        default:
            printf("fail");
    }
}

void print_tree( value* ptr, int depth ) {

    int d = depth;

    print_value( ptr, d );

    if ( ptr -> set_size ) {

        printf("\n");

        for ( int i = 0; i < ptr -> set_size; ++i ) {
            print_tree( ptr -> set[i], d+1 );
            printf("\n");        
        }

    } else if ( ptr -> next ) {
        printf(" -> ");
        print_tree( ptr -> next, d+1 );
    }
}

void dispose( value* ptr ) {

    if ( ptr -> set_size ) {

        for ( int i = 0; i < ptr -> set_size; ++i ) {

            if ( ptr -> set[i] -> next ) {
                dispose( ptr -> set[i] -> next );
            }

            free ( ptr -> set[i] );
        }

        free( ptr -> set );
    }
    
    free( ptr );
}

int main( int argc, char** argv ) {

    if ( argc < 2 ) return 1;

    FILE* fp = fopen( argv[1], "r");

    if (fp) {
        value* head = parse_json(fp);

        print_tree(head, 0);

        dispose(head);
        fclose(fp);

    } else {
        puts("file error.");
    }
    return 0;
}