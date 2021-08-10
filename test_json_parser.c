#include "json_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

char* enum_names[] = {
    "string_t", "number", "object", 
    "array", "true", "false", "null", 
    "comma"
};

void print_value( value* head, int depth ) {

    for ( int i = 0; i < depth; ++i ) {
        printf("  ");
    }

    printf("%s : ", enum_names[head -> type]);

    switch (head -> type) {
        case string :
        case number :
            printf("%s", ((char*) head -> value));
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
        case comma :
            printf(",");
            break;
        default:
            printf("Unrecognized.");
    }
}

void print_tree( value* ptr, int standoff, int depth ) {

    int d = depth;

    print_value( ptr, standoff );

    if ( ptr -> type == string && ptr -> set_size == -1 ) {
        printf("  ->  ");
        print_tree( ptr ->next, 0, d+1 );

    } else if ( ptr -> set_size ) {

        printf("\n");

        for ( int i = 0; i < ptr -> set_size; ++i ) {
            print_tree( ptr -> set[i], d+1, d+1 );
            printf("\n");        
        }
    }
}

void test_parser( FILE* fp ) {

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);

    value* head = parse_json(fp);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    printf ("Parse time = %f seconds\n\n",
        (end.tv_nsec - begin.tv_nsec) / 1000000000.0 + 
        (end.tv_sec  - begin.tv_sec)
    );

    if (head) {

        print_tree(head, 0, 0);
        dispose(head);

    } else {
        printf("Json syntax Error\n");
    }
}

void test_to_string( FILE* fp ) {

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);

    value* head = parse_json(fp);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    printf ("Parse time = %f seconds\n\n",
        (end.tv_nsec - begin.tv_nsec) / 1000000000.0 + 
        (end.tv_sec  - begin.tv_sec)
    );

    if (head) {

        char* str = (char*) calloc(1, sizeof(char));
        string_buffer b = { str, 1, 0 };

        if ( !to_string(head, &b) ) {

            printf("%s\n", b.buffer);

        } else {
            printf("Could not create string.\n");
        }

        free(b.buffer);
        dispose(head);

    } else {
        printf("Json syntax Error\n");
    }
}

int main( int argc, char** argv ) {

    if ( argc < 2 ) return 1;

    FILE* fp = fopen( argv[1], "r" );

    if (fp) {

        test_parser( fp );
        //test_to_string( fp );
        fclose(fp);

    } else {
        puts("file error.");
    }
}