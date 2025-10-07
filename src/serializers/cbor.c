#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tinycbor/cbor.h>

#include "wampproto/dict.h"
#include "wampproto/messages.h"
#include "wampproto/serializers.h"
#include "wampproto/value.h"

typedef struct
{
    Serializer base;
} CborSerializer;

// Forward declaration
static Value *cbor_value_to_value(CborValue *it);

static Value *cbor_value_to_value(CborValue *it)
{
    if (cbor_value_is_null(it))
    {
        return value_null();
    }
    if (cbor_value_is_boolean(it))
    {
        bool val;
        cbor_value_get_boolean(it, &val);
        return value_bool(val);
    }
    if (cbor_value_is_integer(it))
    {
        int64_t val;
        cbor_value_get_int64(it, &val);
        return value_int(val);
    }
    if (cbor_value_is_double(it))
    {
        double val;
        cbor_value_get_double(it, &val);
        return value_float(val);
    }
    if (cbor_value_is_text_string(it))
    {
        size_t len;
        cbor_value_get_string_length(it, &len);
        char *str = malloc(len + 1);
        cbor_value_copy_text_string(it, str, &len, NULL);
        str[len] = '\0';
        Value *result = value_str(str);
        free(str);
        return result;
    }
    if (cbor_value_is_byte_string(it))
    {
        size_t len;
        cbor_value_get_string_length(it, &len);
        uint8_t *bytes = malloc(len);
        cbor_value_copy_byte_string(it, bytes, &len, NULL);
        Value *result = value_bytes(bytes, len);
        free(bytes);
        return result;
    }
    if (cbor_value_is_array(it))
    {
        size_t len;
        cbor_value_get_array_length(it, &len);

        Value *arr = value_list(len);
        CborValue arrayIt;
        cbor_value_enter_container(it, &arrayIt);

        while (!cbor_value_at_end(&arrayIt))
        {
            Value *item = cbor_value_to_value(&arrayIt);
            if (item)
            {
                value_list_append(arr, item);
            }
            if (item->type != VALUE_DICT)
                cbor_value_advance(&arrayIt);
        }

        cbor_value_leave_container(it, &arrayIt);
        return arr;
    }
    if (cbor_value_is_map(it))
    {
        Value *dict = value_dict();
        CborValue mapIt;
        cbor_value_enter_container(it, &mapIt);

        while (!cbor_value_at_end(&mapIt))
        {
            // Get key (assuming string key)
            if (!cbor_value_is_text_string(&mapIt))
            {
                cbor_value_advance(&mapIt);
                continue;
            }

            size_t key_len;
            cbor_value_get_string_length(&mapIt, &key_len);
            char *key = malloc(key_len + 1);
            cbor_value_copy_text_string(&mapIt, key, &key_len, NULL);
            key[key_len] = '\0';

            cbor_value_advance(&mapIt);

            // Get value
            if (!cbor_value_at_end(&mapIt))
            {
                Value *val = cbor_value_to_value(&mapIt);
                if (val)
                {
                    dict_insert(dict->dict_val, key, val);
                }
                cbor_value_advance(&mapIt);
            }

            free(key);
        }

        cbor_value_leave_container(it, &mapIt);
        return dict;
    }

    return NULL;
}

CborError cbor_encode_value(CborEncoder *encoder, const Value *val);

static Bytes encode_value_to_cbor(const List *val)
{
    if (!val)
    {
        const Bytes out = {.data = NULL, .len = 0};
        return out;
    }

    // Estimate buffer size
    const size_t buffer_size = 1024;
    uint8_t *buffer = malloc(buffer_size);
    if (!buffer)
    {
        const Bytes out = {.data = NULL, .len = 0};
        return out;
    }

    CborEncoder encoder;
    cbor_encoder_init(&encoder, buffer, buffer_size, 0);

    const CborError err = cbor_encode_value(&encoder, (Value *)val);
    if (err != CborNoError)
    {
        free(buffer);
        Bytes out = {.data = NULL, .len = 0};
        return out;
    }

    size_t actual_len = cbor_encoder_get_buffer_size(&encoder, buffer);

    buffer = realloc(buffer, actual_len);

    Bytes out = {.data = buffer, .len = actual_len};
    return out;
}

static Value *decode_cbor_to_value(const uint8_t *data, size_t len)
{
    if (!data || len == 0)
    {
        return NULL;
    }

    CborParser parser;
    CborValue it;

    const CborError err = cbor_parser_init(data, len, 0, &parser, &it);
    if (err != CborNoError)
    {
        return NULL;
    }

    return cbor_value_to_value(&it);
}

static Bytes cbor_serialize(const Serializer *self, const Message *msg)
{
    (void)self;

    if (!msg)
    {
        const Bytes out = {.data = NULL, .len = 0};
        return out;
    }

    List *val = msg->marshal(msg);
    if (!val)
    {
        const Bytes out = {.data = NULL, .len = 0};
        return out;
    }

    const Bytes out = encode_value_to_cbor(val);
    value_free((Value *)val);
    return out;
}

static Message *cbor_deserialize(const Serializer *self, Bytes data)
{
    (void)self;

    if (data.len == 0)
    {
        return NULL;
    }

    Value *val = decode_cbor_to_value(data.data, data.len);
    if (!val)
    {
        return NULL;
    }

    if (val->type != VALUE_LIST || val->list_val.len < 1)
    {
        value_free(val);
        return NULL;
    }

    Message *msg = to_message(&val->list_val);

    free(val);
    return msg;
}

static void cbor_free(Serializer *self)
{
    free(self);
}

Serializer *cbor_serializer_new(void)
{
    CborSerializer *ser = calloc(1, sizeof(CborSerializer));
    ser->base.serialize = cbor_serialize;
    ser->base.deserialize = cbor_deserialize;
    ser->base.free = cbor_free;
    return (Serializer *)ser;
}

CborError cbor_encode_value(CborEncoder *encoder, const Value *value)
{
    if (!value)
    {
        return cbor_encode_null(encoder);
    }

    switch (value->type)
    {
    case VALUE_NULL:
        return cbor_encode_null(encoder);

    case VALUE_INT:
        return cbor_encode_int(encoder, value->int_val);

    case VALUE_FLOAT:
        return cbor_encode_double(encoder, value->float_val);

    case VALUE_BOOL:
        return cbor_encode_boolean(encoder, value->bool_val);

    case VALUE_STR:
        return cbor_encode_text_string(encoder, value->str_val, strlen(value->str_val));

    case VALUE_BYTES:
        return cbor_encode_byte_string(encoder, value->bytes_val.data, value->bytes_val.len);

    case VALUE_LIST:
    {
        CborEncoder list_encoder;
        CborError err = cbor_encoder_create_array(encoder, &list_encoder, value->list_val.len);
        if (err != CborNoError)
            return err;

        for (size_t i = 0; i < value->list_val.len; ++i)
        {
            err = cbor_encode_value(&list_encoder, value->list_val.items[i]);
            if (err != CborNoError)
                return err;
        }

        return cbor_encoder_close_container(encoder, &list_encoder);
    }

    case VALUE_DICT:
    {
        Dict *dict = value->dict_val;
        size_t count = dict->count;

        CborEncoder mapEncoder;
        CborError err = cbor_encoder_create_map(encoder, &mapEncoder, count);
        if (err != CborNoError)
            return err;

        for (size_t index = 0; index < dict->size; index++)
        {
            Entry *curr = dict->buckets[index];
            while (curr)
            {
                err = cbor_encode_text_string(&mapEncoder, curr->key, strlen(curr->key));
                if (err != CborNoError)
                    return err;
                err = cbor_encode_value(&mapEncoder, curr->value);
                if (err != CborNoError)
                    return err;

                curr = curr->next;
            }
        }
        return cbor_encoder_close_container(encoder, &mapEncoder);
    }

    default:
        return CborErrorUnknownType;
    }
}
