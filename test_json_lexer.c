#include "json_lexer.h"
#include <stdlib.h>
#include <stdio.h>

void print_lexemes( lexeme* head ) {

    char* enum_names[] = {
        "string_t", "number", "object", 
        "array", "true", "false", "null", 
        "op", "end"
    };

    lexeme* tmp = head;

    while ( head != NULL ) {
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
                printf("{ size : %d", head -> set_size );
                break;
            case array :
                printf("[ size : %d", head -> set_size );
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

    if (fp) {
        lexeme* head = (lexeme*) malloc(sizeof(lexeme));
        head -> type = null;
        int i;
        i = lex_json(head, fp);
        printf("RETURN: %d\n", i);
        
        print_lexemes(head);
        fclose(fp);

    } else {
        puts("file error.");
    }
    return 0;
}