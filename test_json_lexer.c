#include "json_lexer.h"
#include <stdlib.h>
#include <stdio.h>

void print_lexemes( lexeme* head ) {

    char* enum_names[] = {
        "string_t", "number", "object", 
        "array", "true", "false", "null", 
        "op"
    };

    lexeme* tmp = head;

    while ( head != NULL ) {
        printf(" %s ", enum_names[head -> type]);

        switch (head -> type) {
            case string_t :
                printf("%s", ((char*) head -> value));
                free(head -> value);
                break;
            case number :
                printf("%d", ((double*) head -> value));
                free(head -> value);
                break;
            case object :
                printf("{ ");
                break;
            case array :
                printf("[ ");
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
            default:
                printf("fail");
        }
        printf("\n");
        tmp = head -> next;
        free(head);
        head = tmp;
    }
}

int main( int argc, char** argv ) {

    if ( argc < 2 ) return 1;

    FILE* fp = fopen( argv[1], "r");

    puts("1");

    if (fp) {
        lexeme* head = (lexeme*) malloc(sizeof(lexeme));
        head -> type = null;

        puts("2");

        lex_json(head, fp);

        puts("3");

        print_lexemes(head);

        puts("4");

        fclose(fp);

        puts("5");
    }
    return 0;
}