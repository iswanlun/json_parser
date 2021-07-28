#ifndef JSON_ENGINE_H
#define JSON_ENGINE_H

#include "json_parser.h"

char* as_string(value* val, int* length);

value* find(value* val, value_type type);

int dispose(value* val);

#endif