#ifndef WAMPPROTO_DICT_H
#define WAMPPROTO_DICT_H

typedef struct Dict Dict;
typedef struct Value Value;

Dict *create_dict();
void dict_insert(Dict *dict, const char *key, Value *value);
Value *dict_get(Dict *dict, const char *key);
void dict_remove(Dict *dict, const char *key);
void dict_free(Dict *dict);

#endif // WAMPPROTO_DICT_H
