#include <stdlib.h>
#include <stdio.h>
#include "json_validator.h"

short mv_p( int mv, valid_state state, stack_v* stk ) {
    stk -> s_ptr += mv;
    stk -> stack[stk -> s_ptr] = state;
    return 1;
}

short mv( int mv, valid_state state, stack_v* stk ) {
    stk -> s_ptr += mv;
    return 1;
}

short err( int mv, valid_state state, stack_v* stk ) {
    stk -> s_ptr = 0;
    return 0;
}

short obj_p( int mv, valid_state state, stack_v* stk ) {
    stk -> s_ptr += mv;
    stk -> stack[stk -> s_ptr] = state;
    stk -> stack[++(stk -> s_ptr)] = ob;
    return 1;
}

short arr_p( int mv, valid_state state, stack_v* stk ) {
    stk -> s_ptr += mv;
    stk -> stack[stk -> s_ptr] = state;
    stk -> stack[++(stk -> s_ptr)] = ar;
    return 1;
}

short st( int mv, valid_state state, stack_v* stk ) {
    stk -> stack[stk -> s_ptr] = en;
    stk -> s_ptr += mv;
    stk -> stack[stk -> s_ptr] = state;
    return 1;
}

short ef( int mv, valid_state state, stack_v* stk ) {
    return 1;
}

fsm_mode fsm[10][9] = {
    { { -1, cur, &mv }, { 1, ar_v, &mv_p }, { 0, er, &err },    { 0, er, &err },    { 1, ar_v, &mv_p },  { 0, er, &err },  { 1, ar_v, &arr_p },  { 1, ar_v, &obj_p },  { 0, er, &err } },
    { { -2, cur, &mv }, { 0, er, &err },    { 1, ar_c, &mv_p }, { 0, er, &err },    { 0, er, &err },     { 0, er, &err },  { 0, er, &err },      { 0, er, &err },      { 0, er, &err } },
    { { 0, er, &err },  { -1, cur, &mv },   { 0, er, &err },    { 0, er, &err },    { -1, cur, &mv },    { 0, er, &err },  { -1, ar_v, &arr_p }, { -1, ar_v, &obj_p }, { 0, er, &err } },
    { { 0, er, &err },  { 0, er, &err },    { 0, er, &err },    { -1, cur, &mv },   { 1, ob_s, &mv_p },  { 0, er, &err },  { 0, er, &err },      { 0, er, &err },      { 0, er, &err } },
    { { 0, er, &err },  { 0, er, &err },    { 0, er, &err },    { 0, er, &err },    { 0, er, &err },     { 1, c, &mv_p },  { 0, er, &err },      { 0, er, &err },      { 0, er, &err } },
    { { 0, er, &err },  { 1, ob_v, &mv_p }, { 0, er, &err },    { 0, er, &err },    { 1, ob_v, &mv_p },  { 0, er, &err },  { 1, ob_v, &arr_p },  { 1, ob_v, &obj_p },  { 0, er, &err } },
    { { 0, er, &err },  { 0, er, &err },    { 1, ob_c, &mv_p }, { -4, cur, &mv },   { 0, er, &err },     { 0, er, &err },  { 0, er, &err },      { 0, er, &err },      { 0, er, &err } },
    { { 0, er, &err },  { 0, er, &err },    { 0, er, &err },    { 0, er, &err },    { -3, ob_s, &mv_p }, { 0, er, &err },  { 0, er, &err },      { 0, er, &err },      { 0, er, &err } },
    { { 0, er, &err },  { 0, er, &err },    { 0, er, &err },    { 0, er, &err },    { 0, er, &err },     { 0, er, &err },  { 1, ar, &st },       { 1, ob, &st },       { 0, er, &err } },
    { { 0, er, &err },  { 0, er, &err },    { 0, er, &err },    { 0, er, &err },    { 0, er, &err },     { 0, er, &err },  { 0, er, &err },      { 0, er, &err },      { 0, cur, &ef } }
};

short is_valid( stack_v* stk, input i ) {

    if ( stk -> s_ptr + 1 >= stk -> s_size ) {
        stk -> s_size *= 2;
        stk -> stack = realloc( stk -> stack, stk -> s_size );
    }
    
    int m = fsm[stk -> stack[stk -> s_ptr]][(short)i].movement;
    valid_state s = fsm[(short)stk -> stack[stk -> s_ptr]][(short)i].next_state;
    short r = stk -> stack[stk -> s_ptr];

    return fsm[r][i].action(m, s, stk);
}

stack_v* create_stack( int size ) {

    stack_v* stk = (stack_v*) malloc( sizeof(stack_v) );
    stk -> stack = (valid_state*) malloc( size * sizeof(valid_state) );
    stk -> s_size = size;
    stk -> s_ptr = 0;
    stk -> stack[0] = b;

    return stk;
}

void dispose_stack( stack_v* stk ) {

    free( stk -> stack );
    free( stk );
}