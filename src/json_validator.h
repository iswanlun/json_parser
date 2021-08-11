#ifndef JSON_VALIDATOR_H
#define JSON_VALIDATOR_H

typedef enum valid_state {
    ar, ar_v, ar_c, ob, ob_s, c, ob_v, ob_c, b, en, cur, er
} valid_state;

typedef enum input {
    ar_e, v, com_i, ob_e, str, col, ar_i, ob_i, end
} input;

typedef struct stack_v {
    valid_state* stack;
    int s_ptr;
    int s_size;
} stack_v;

typedef struct {
    int movement;
    valid_state next_state;
    short (*action)(int, valid_state, stack_v*);
} fsm_mode;

short is_valid( stack_v* stk, input i );

stack_v* create_stack( int size );

void dispose_stack( stack_v* stk );

#endif