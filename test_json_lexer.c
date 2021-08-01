#include "json_lexer.h"
#include <stdlib.h>
#include <stdio.h>

void print_values( value* head, int depth ) {

    char* enum_names[] = {
        "string_t", "number", "object", 
        "array", "true", "false", "null", 
        "op", "end", "comma"
    };

    value* tmp;

    while ( head != NULL ) {

        for ( int i = 0; i < depth; ++i ) {
            printf("-");
        }

        printf(" %s \t\t", enum_names[head -> type]);

        switch (head -> type) {
            case string_t :
                printf("%s", ((char*) head -> value));
                free(head -> value);
                break;
            case number :
                printf("%f", *((double*) head -> value));
                free(head -> value);
                break;
            case object :
            case array :
                printf("{/[ size : %d\n", head -> set_size );

                for ( int i = 0; i < head -> set_size; ++i ) {
                    print_values( head -> set[i], depth+1 );
                }
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
            case op :
                printf(":");
                break;
            case end :
                printf(" ]/}");
                break;
            case comma :
                printf(" ,");
                break;
            default:
                printf("fail");
        }

        tmp = head -> next;
        free(head);
        head = tmp;
        printf("\n");
    }
}

int main( int argc, char** argv ) {

    if ( argc < 2 ) return 1;

    FILE* fp = fopen( argv[1], "r");

    if (fp) {
        value* head = (value*) malloc(sizeof(value));
        head -> type = null;
        int i;
        i = lex_json(head, fp);
        printf("RETURN: %d\n", i);
        
        print_values(head, 0);
        fclose(fp);

    } else {
        puts("file error.");
    }
    return 0;
}