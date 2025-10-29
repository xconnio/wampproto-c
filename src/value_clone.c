#include <stdlib.h>
#include <string.h>

#include "wampproto/dict.h"
#include "wampproto/value.h"

static List* list_clone(const List* src);
static Dict* dict_clone_internal(const Dict* src);

Value* value_clone(const Value* src) {
    if (!src) return NULL;

    Value* copy = calloc(1, sizeof(Value));
    if (!copy) return NULL;
    copy->type = src->type;

    switch (src->type) {
        case VALUE_NULL:
            break;

        case VALUE_INT:
            copy->int_val = src->int_val;
            break;

        case VALUE_FLOAT:
            copy->float_val = src->float_val;
            break;

        case VALUE_BOOL:
            copy->bool_val = src->bool_val;
            break;

        case VALUE_STR:
            copy->str_val = src->str_val ? strdup(src->str_val) : NULL;
            break;

        case VALUE_LIST:
            copy->list_val = list_clone(src->list_val);
            break;

        case VALUE_DICT:
            copy->dict_val = dict_clone_internal(src->dict_val);
            break;

        case VALUE_BYTES:
            if (src->bytes_val.len > 0 && src->bytes_val.data) {
                copy->bytes_val.data = malloc(src->bytes_val.len);
                memcpy(copy->bytes_val.data, src->bytes_val.data, src->bytes_val.len);
                copy->bytes_val.len = src->bytes_val.len;
            } else {
                copy->bytes_val.data = NULL;
                copy->bytes_val.len = 0;
            }
            break;
    }

    return copy;
}

static List* list_clone(const List* src) {
    if (!src) return NULL;

    List* clone = malloc(sizeof(List));
    if (!clone) return NULL;

    clone->len = src->len;
    clone->items = calloc(clone->len, sizeof(Value*));
    if (!clone->items) {
        free(clone);
        return NULL;
    }

    for (size_t i = 0; i < src->len; i++) {
        clone->items[i] = value_clone(src->items[i]);
    }

    return clone;
}

static Dict* dict_clone_internal(const Dict* src) {
    if (!src) return NULL;

    Dict* clone = create_dict();
    if (!clone) return NULL;

    Entry* curr;
    for (curr = src->table; curr != NULL; curr = curr->hh.next) {
        Value* cloned_value = value_clone(curr->value);
        dict_insert(clone, curr->key, cloned_value);
    }

    return clone;
}

Dict* dict_clone(const Dict* src) { return dict_clone_internal(src); }
