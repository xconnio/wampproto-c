#ifndef WAMPPROTO_SESSION_H
#define WAMPPROTO_SESSION_H
#include <stdint.h>
#include <uthash.h>

#include "wampproto/mutex.h"
#include "wampproto/serializers/serializer.h"

typedef struct {
    uint64_t key;
    UT_hash_handle hh;
} MapUint64ToVoid;

typedef struct {
    uint64_t key;
    uint64_t value;
    UT_hash_handle hh;
} MapUint64ToUint64;

typedef struct wampproto_Session wampproto_Session;

typedef struct wampproto_Session {
    Serializer* serializer;
    Bytes (*send_message)(wampproto_Session* session, const Message* message);
    Message* (*receive_message)(wampproto_Session* session, Message* message);
    void* (*free)(wampproto_Session* session);

    Mutex* mutex;

    // RPC-related maps
    MapUint64ToVoid* callRequests;
    MapUint64ToVoid* registerRequests;
    MapUint64ToVoid* registrations;
    MapUint64ToVoid* invocationRequests;
    MapUint64ToUint64* unregisterRequests;

    // PubSub-related maps
    MapUint64ToVoid* publishRequests;
    MapUint64ToVoid* subscribeRequests;
    MapUint64ToVoid* subscriptions;
    MapUint64ToUint64* unsubscribeRequests;
} wampproto_Session;

Bytes session_send_message(wampproto_Session* session, const Message* message);
Message* session_receive_message(wampproto_Session* session, Message* message);
wampproto_Session* session_new(Serializer* serializer);

static void add_void_entry(MapUint64ToVoid** requests, int64_t request_id);
static void add_value_entry(MapUint64ToUint64** requests, int64_t request_id, int64_t value);
static Bytes empty_bytes(void);
static int64_t remove_void_entry(MapUint64ToVoid** requests, int64_t request_id);
static int64_t remove_value_entry(MapUint64ToUint64** requests, int64_t request_id);
static int64_t void_entry_exists(MapUint64ToVoid** requests, int64_t request_id);

#endif
