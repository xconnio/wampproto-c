#include "wampproto/value.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wampproto/dict.h"

static Value* value_alloc(ValueType type) {
    Value* v = calloc(1, sizeof(Value));
    v->type = type;
    return v;
}

Value* value_null(void) { return value_alloc(VALUE_NULL); }

Value* value_int(int64_t n) {
    Value* v = value_alloc(VALUE_INT);
    v->int_val = n;
    return v;
}

double value_as_double(Value* value) {
    if (value->type != VALUE_FLOAT) return 0.0;

    return value->float_val;
}

Value* value_float(double n) {
    Value* v = value_alloc(VALUE_FLOAT);
    v->float_val = n;
    return v;
}

int value_as_bool(Value* value) { return value->bool_val; }

Value* value_bool(int b) {
    Value* v = value_alloc(VALUE_BOOL);
    v->bool_val = b ? 1 : 0;
    return v;
}

char* value_as_str(const Value* value) {
    if (value == NULL || value->type != VALUE_STR) return "";

    return value->str_val;
}

Value* value_str(const char* s) {
    Value* v = value_alloc(VALUE_STR);
    v->str_val = strdup(s);
    return v;
}

List* value_as_list(Value* value) { return value->list_val; }

Value* value_list(const size_t len) {
    Value* v = value_alloc(VALUE_LIST);
    v->list_val = malloc(sizeof(List));
    v->list_val->items = calloc(len, sizeof(Value*));
    v->list_val->len = 0;
    return v;
}

Value* create_value_from_list(List* list) {
    Value* v = value_alloc(VALUE_LIST);
    v->list_val = list;
    memset(list, 0, sizeof(List));
    return v;
}

Value* value_dict(void) {
    Value* v = value_alloc(VALUE_DICT);
    v->dict_val = create_dict();
    return v;
}

Dict* value_as_dict(const Value* value) {
    if (!value || value->type != VALUE_DICT) return NULL;

    return value->dict_val;
}

Value* value_bytes(const uint8_t* data, size_t len) {
    Value* v = value_alloc(VALUE_BYTES);
    if (len > 0 && data != NULL) {
        v->bytes_val.data = malloc(len);
        memcpy(v->bytes_val.data, data, len);
        v->bytes_val.len = len;

    } else {
        v->bytes_val.data = NULL;
        v->bytes_val.len = 0;
    }
    return v;
}

int value_list_set(Value* list, size_t idx, Value* val) {
    if (!list || list->type != VALUE_LIST) return -1;
    if (idx >= list->list_val->len) return -1;
    list->list_val->items[idx] = val;
    return 0;
}

int value_list_append(Value* list, Value* val) {
    if (!list || list->type != VALUE_LIST) return -1;
    size_t n = list->list_val->len + 1;
    Value** new_items = realloc(list->list_val->items, n * sizeof(Value*));
    if (!new_items) return -1;
    new_items[list->list_val->len] = val;
    list->list_val->items = new_items;
    list->list_val->len = n;
    return 0;
}

int value_dict_set(Value* dict, const char* key, Value* val) {
    if (!dict || dict->type != VALUE_DICT) return -1;

    Dict* map = dict->dict_val;
    dict_insert(map, key, val);
    return 0;
}

int value_dict_append(Value* dict, const char* key, Value* val) {
    if (!dict || dict->type != VALUE_DICT || !key) return -1;

    Dict* map = dict->dict_val;
    dict_insert(map, key, val);
    return 0;
}

int64_t value_as_int(const Value* v) {
    if (!v) return 0;
    switch (v->type) {
        case VALUE_INT:
            return v->int_val;
        case VALUE_BOOL:
            return v->bool_val ? 1 : 0;
        case VALUE_FLOAT:
            return (int64_t)v->float_val;
        case VALUE_STR:
            return atoll(v->str_val);
        default:
            return 0;
    }
}

Value* value_from_list(const List* list) {
    Value* v = value_alloc(VALUE_LIST);
    v->list_val = (List*)list;
    return v;
}

Value* value_from_dict(Dict* dict) {
    Value* v = value_alloc(VALUE_DICT);
    v->dict_val = dict;
    return v;
}

void value_free(Value* v) {
    if (!v) return;
    switch (v->type) {
        case VALUE_STR:
            free(v->str_val);
            break;
        case VALUE_LIST:
            for (size_t i = 0; i < v->list_val->len; i++) {
                value_free(v->list_val->items[i]);
            }
            free(v->list_val->items);
            break;
        case VALUE_DICT: {
            Dict* cur = v->dict_val;
            dict_free(cur);
            break;
        }
        case VALUE_BYTES:
            free(v->bytes_val.data);
            break;
        default:
            break;
    }
    free(v);
}

int64_t int_from_dict(Dict* dict, const char* key) {
    Value* value = dict_get(dict, key);
    if (value == NULL) return 0;

    return value_as_int(value);
}

char* str_from_dict(Dict* dict, const char* key) {
    Value* value = dict_get(dict, key);
    if (value == NULL) return "";

    return value_as_str(value);
}

double float_from_dict(Dict* dict, const char* key) {
    Value* value = dict_get(dict, key);
    if (value == NULL) return 0.0;

    return value_as_double(value);
}

List* list_from_dict(Dict* dict, const char* key) {
    Value* value = dict_get(dict, key);
    if (value == NULL) value = value_list(0);

    return value_as_list(value);
}

Dict* dict_from_dict(Dict* dict, const char* key) {
    Value* value = dict_get(dict, key);
    if (value == NULL) return NULL;

    return value_as_dict(value);
}
