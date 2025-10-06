#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h>

#include "wampproto/messages.h"
#include "wampproto/serializers.h"
#include "wampproto/value.h"

typedef struct
{
    Serializer base;
} JsonSerializer;

static cJSON *value_to_cjson(const Value *val);
static Value *cjson_to_value(const cJSON *json);

static cJSON *value_to_cjson(const Value *val)
{
    if (!val)
        return cJSON_CreateNull();

    switch (val->type)
    {
    case VALUE_NULL:
        return cJSON_CreateNull();
    case VALUE_BOOL:
        return cJSON_CreateBool(val->bool_val);
    case VALUE_INT:
        return cJSON_CreateNumber((double)val->int_val);
    case VALUE_FLOAT:
        return cJSON_CreateNumber(val->float_val);
    case VALUE_STR:
        return cJSON_CreateString(val->str_val);
    case VALUE_BYTES:
    {
        // Encode as base64 or hex (here: base64 via cJSON_AddRaw)
        // For now, we fallback to hex string for simplicity
        char *hex = malloc(val->bytes_val.len * 2 + 1);
        for (size_t i = 0; i < val->bytes_val.len; i++)
            sprintf(&hex[i * 2], "%02x", val->bytes_val.data[i]);
        cJSON *str = cJSON_CreateString(hex);
        free(hex);
        return str;
    }
    case VALUE_LIST:
    {
        cJSON *arr = cJSON_CreateArray();
        for (size_t i = 0; i < val->list_val.len; i++)
        {
            cJSON_AddItemToArray(arr, value_to_cjson(val->list_val.items[i]));
        }
        return arr;
    }
    case VALUE_DICT:
    {
        cJSON *obj = cJSON_CreateObject();
        Dict *dict = val->dict_val;
        for (size_t index = 0; index < dict->size; index++)
        {
            Entry *curr = dict->buckets[index];
            while (curr)
            {
                cJSON_AddItemToObject(obj, curr->key, value_to_cjson(curr->value));
                curr = curr->next;
            }
        }
        return obj;
    }
    default:
        return cJSON_CreateNull();
    }
}

static Value *cjson_to_value(const cJSON *json)
{
    if (!json)
        return value_null();

    if (cJSON_IsNull(json))
        return value_null();
    if (cJSON_IsBool(json))
        return value_bool(cJSON_IsTrue(json));
    if (cJSON_IsNumber(json))
    {
        // Decide int vs float based on presence of fractional part
        double d = json->valuedouble;
        if (d == (int64_t)d)
        {
            return value_int((int64_t)d);
        }
        else
        {
            return value_float(d);
        }
    }
    if (cJSON_IsString(json))
        return value_str(json->valuestring);
    if (cJSON_IsArray(json))
    {
        size_t size = cJSON_GetArraySize(json);
        Value *arr = value_list(0);
        cJSON *child;
        cJSON_ArrayForEach(child, json)
        {
            value_list_append(arr, cjson_to_value(child));
        }
        return arr;
    }
    if (cJSON_IsObject(json))
    {
        Value *dict = value_dict();
        cJSON *child;
        cJSON_ArrayForEach(child, json)
        {
            value_dict_set(dict, child->string, cjson_to_value(child));
        }
        return dict;
    }

    return value_null();
}

static Bytes json_serialize(const Serializer *self, const Message *msg)
{
    (void)self;
    if (!msg)
        return (Bytes){NULL, 0};

    List *val = msg->marshal(msg);
    if (!val)
        return (Bytes){NULL, 0};

    cJSON *json = value_to_cjson((Value *)val);
    char *str = cJSON_PrintUnformatted(json);

    const Bytes out = {.data = (uint8_t *)str, .len = strlen(str)};

    cJSON_Delete(json);
    value_free((Value *)val);
    return out;
}

static Message *json_deserialize(const Serializer *self, Bytes data)
{
    (void)self;
    if (data.len == 0)
        return NULL;

    char *copy = strndup((const char *)data.data, data.len);
    cJSON *json = cJSON_Parse(copy);
    free(copy);

    if (!json)
        return NULL;

    Value *val = cjson_to_value(json);
    cJSON_Delete(json);

    if (!val || val->type != VALUE_LIST || val->list_val.len < 1)
    {
        value_free(val);
        return NULL;
    }

    Message *msg = to_message(&val->list_val);

    free(val);
    return msg;
}

static void json_free(Serializer *self)
{
    free(self);
}

Serializer *json_serializer_new(void)
{
    JsonSerializer *ser = calloc(1, sizeof(JsonSerializer));
    ser->base.serialize = json_serialize;
    ser->base.deserialize = json_deserialize;
    ser->base.free = json_free;
    return (Serializer *)ser;
}
