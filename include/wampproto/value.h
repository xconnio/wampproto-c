#ifndef WAMPPROTO_VALUE_H
#define WAMPPROTO_VALUE_H

#include <stddef.h>
#include <stdint.h>

typedef struct Value Value;
typedef struct Entry Entry;

typedef enum
{
    VALUE_NULL,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_BOOL,
    VALUE_STR,
    VALUE_LIST,
    VALUE_DICT,
    VALUE_BYTES,
} ValueType;

typedef struct Entry
{
    char *key;
    Value *value;
    Entry *next;

} Entry;

typedef struct Dict
{
    Entry **buckets;
    size_t size;
    size_t count;
} Dict;

typedef struct
{
    struct Value **items;
    size_t len;
} List;

typedef struct
{
    uint8_t *data;
    size_t len;
} Bytes;

typedef struct Value
{
    ValueType type;
    union
    {
        int64_t int_val;
        double float_val;
        int bool_val;
        char *str_val;
        List list_val;
        Dict *dict_val;
        Bytes bytes_val;
    };
} Value;

Value *value_null(void);
Value *value_int(int64_t n);
Value *value_float(double n);
Value *value_bool(int b);
Value *value_str(const char *s);
Value *value_list(size_t len);
Value *value_dict(void);
Value *value_bytes(const uint8_t *data, size_t len);

int64_t value_as_int(const Value *v);
char *value_as_str(const Value *v);
Dict *value_as_dict(const Value *v);

int value_list_set(Value *list, size_t idx, Value *val);
int value_dict_set(Value *dict, const char *key, Value *val);

int value_list_append(Value *list, Value *val);
int value_dict_append(Value *dict, const char *key, Value *val);

Value *value_from_dict(Dict *dict);
Value *value_from_list(const List *list);

#define VALUE_FROM_INT(n) value_int((n))
#define VALUE_FROM_FLOAT(x) value_float((x))
#define VALUE_FROM_BOOL(b) value_bool((b))
#define VALUE_FROM_STR(s) value_str((s))
#define VALUE_FROM_BYTES(d, l) value_bytes((d), (l))

void value_free(Value *v);

#endif
