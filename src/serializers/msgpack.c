#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <msgpack.h>

#include "wampproto/messages.h"
#include "wampproto/serializers.h"
#include "wampproto/value.h"

typedef struct
{
    Serializer base;
} MsgpackSerializer;

static Value *msgpack_object_to_value(const msgpack_object *obj);

static Value *msgpack_object_to_value(const msgpack_object *obj)
{
    switch (obj->type)
    {
    case MSGPACK_OBJECT_NIL:
        return value_null();

    case MSGPACK_OBJECT_BOOLEAN:
        return value_bool(obj->via.boolean);

    case MSGPACK_OBJECT_POSITIVE_INTEGER:
    {
        int64_t v = (int64_t)obj->via.u64;
        return value_int(v);
    }

    case MSGPACK_OBJECT_NEGATIVE_INTEGER:
    {
        int64_t v = obj->via.i64;
        return value_int(v);
    }

    case MSGPACK_OBJECT_FLOAT32:
    case MSGPACK_OBJECT_FLOAT64:
        return value_float(obj->via.f64);

    case MSGPACK_OBJECT_STR:
    {
        // Not null-terminated from msgpack; copy and NUL-terminate.
        size_t len = obj->via.str.size;
        const char *src = obj->via.str.ptr;
        char *s = (char *)malloc(len + 1);
        if (!s)
            return NULL;
        memcpy(s, src, len);
        s[len] = '\0';
        Value *v = value_str(s);
        free(s);
        return v;
    }

    case MSGPACK_OBJECT_BIN:
    {
        size_t len = obj->via.bin.size;
        const char *src = obj->via.bin.ptr;
        uint8_t *buf = (uint8_t *)malloc(len);
        if (!buf)
            return NULL;
        memcpy(buf, src, len);
        Value *v = value_bytes(buf, len);
        free(buf);
        return v;
    }

    case MSGPACK_OBJECT_ARRAY:
    {
        size_t n = obj->via.array.size;
        Value *arr = value_list(0); // capacity-style constructor is safer
        for (size_t i = 0; i < n; ++i)
        {
            Value *elem = msgpack_object_to_value(&obj->via.array.ptr[i]);
            if (!elem)
                elem = value_null();
            value_list_append(arr, elem);
        }
        return arr;
    }

    case MSGPACK_OBJECT_MAP:
    {
        Value *dict = value_dict();
        size_t n = obj->via.map.size;
        for (size_t i = 0; i < n; ++i)
        {
            const msgpack_object_kv *kv = &obj->via.map.ptr[i];
            // We’ll follow your CBOR implementation: only accept string keys
            if (kv->key.type != MSGPACK_OBJECT_STR)
            {
                continue;
            }
            size_t klen = kv->key.via.str.size;
            const char *kptr = kv->key.via.str.ptr;
            char *key = (char *)malloc(klen + 1);
            if (!key)
                continue;
            memcpy(key, kptr, klen);
            key[klen] = '\0';

            Value *v = msgpack_object_to_value(&kv->val);
            if (!v)
                v = value_null();
            value_dict_set(dict, key, v);
            free(key);
        }
        return dict;
    }

    // You can add EXT handling here if you use it.
    case MSGPACK_OBJECT_EXT:
    default:
        return NULL;
    }
}

static int value_to_msgpack(msgpack_packer *pk, const Value *value);

static int value_to_msgpack(msgpack_packer *pk, const Value *value)
{
    if (!value)
    {
        return msgpack_pack_nil(pk);
    }

    switch (value->type)
    {
    case VALUE_NULL:
        return msgpack_pack_nil(pk);

    case VALUE_BOOL:
        return value->bool_val ? msgpack_pack_true(pk) : msgpack_pack_false(pk);

    case VALUE_INT:
        // msgpack_c chooses signed/unsigned automatically from function used.
        // We send as int64 to preserve negatives.
        return msgpack_pack_int64(pk, value->int_val);

    case VALUE_FLOAT:
        // msgpack-c will encode as float64 via msgpack_pack_double
        return msgpack_pack_double(pk, value->float_val);

    case VALUE_STR:
    {
        size_t len = strlen(value->str_val);
        int rc = msgpack_pack_str(pk, (uint32_t)len);
        if (rc != 0)
            return rc;
        return msgpack_pack_str_body(pk, value->str_val, len);
    }

    case VALUE_BYTES:
    {
        size_t len = value->bytes_val.len;
        int rc = msgpack_pack_bin(pk, (uint32_t)len);
        if (rc != 0)
            return rc;
        return msgpack_pack_bin_body(pk, (const char *)value->bytes_val.data, len);
    }

    case VALUE_LIST:
    {
        size_t n = value->list_val.len;
        int rc = msgpack_pack_array(pk, (uint32_t)n);
        if (rc != 0)
            return rc;
        for (size_t i = 0; i < n; ++i)
        {
            rc = value_to_msgpack(pk, value->list_val.items[i]);
            if (rc != 0)
                return rc;
        }
        return 0;
    }

    case VALUE_DICT:
    {
        Dict *dict = value->dict_val;
        int rc = msgpack_pack_map(pk, (uint32_t)dict->count);
        if (rc != 0)
            return rc;

        Entry *curr, *tmp;
        HASH_ITER(hh, dict->table, curr, tmp)
        {
            size_t klen = strlen(curr->key);
            rc = msgpack_pack_str(pk, (uint32_t)klen);
            if (rc != 0)
                return rc;

            rc = msgpack_pack_str_body(pk, curr->key, klen);
            if (rc != 0)
                return rc;

            rc = value_to_msgpack(pk, curr->value);
            if (rc != 0)
                return rc;
        }

        return 0;
    }

    default:
        return -1;
    }
}

static Bytes encode_value_to_msgpack(const List *val)
{
    if (!val)
    {
        const Bytes out = {.data = NULL, .len = 0};
        return out;
    }

    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    if (value_to_msgpack(&pk, (Value *)val) != 0)
    {
        msgpack_sbuffer_destroy(&sbuf);
        const Bytes out = {.data = NULL, .len = 0};
        return out;
    }

    // Copy out to a heap buffer we own
    uint8_t *buf = malloc(sbuf.size);
    if (!buf)
    {
        msgpack_sbuffer_destroy(&sbuf);
        const Bytes out = {.data = NULL, .len = 0};
        return out;
    }

    memcpy(buf, sbuf.data, sbuf.size);
    const Bytes out = {.data = buf, .len = sbuf.size};

    msgpack_sbuffer_destroy(&sbuf);
    return out;
}

static Value *decode_msgpack_to_value(const uint8_t *data, size_t len)
{
    if (!data || len == 0)
        return NULL;

    size_t off = 0;
    msgpack_unpacked result;
    msgpack_unpacked_init(&result);

    bool ok = msgpack_unpack_next(&result, (const char *)data, len, &off);
    if (!ok)
    {
        msgpack_unpacked_destroy(&result);
        return NULL;
    }

    Value *v = msgpack_object_to_value(&result.data);
    msgpack_unpacked_destroy(&result);
    return v;
}

static Bytes msgpack_serialize(const Serializer *self, const Message *msg)
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

    const Bytes out = encode_value_to_msgpack(val);
    value_free((Value *)val);
    return out;
}

static Message *msgpack_deserialize(const Serializer *self, Bytes data)
{
    (void)self;
    if (data.len == 0)
        return NULL;

    Value *val = decode_msgpack_to_value(data.data, data.len);
    if (!val)
        return NULL;

    if (val->type != VALUE_LIST || val->list_val.len < 1)
    {
        value_free(val);
        return NULL;
    }

    Message *msg = to_message(&val->list_val);

    free(val);
    return msg;
}

static void msgpack_free(Serializer *self)
{
    free(self);
}

Serializer *msgpack_serializer_new(void)
{
    MsgpackSerializer *ser = calloc(1, sizeof(MsgpackSerializer));
    ser->base.serialize = msgpack_serialize;
    ser->base.deserialize = msgpack_deserialize;
    ser->base.free = msgpack_free;
    return (Serializer *)ser;
}
