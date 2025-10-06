#ifndef WAMPPROTO_DICT_H
#define WAMPPROTO_DICT_H

#include <stddef.h>

typedef struct Dict Dict;
typedef struct Value Value;

Dict *create_dict(size_t size);
void dict_insert(Dict *dict, const char *key, Value *value);
Value *dict_get(Dict *dict, const char *key);
void dict_free(Dict *dict);

#endif // WAMPPROTO_DICT_H
