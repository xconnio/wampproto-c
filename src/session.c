#include <uthash.h>
#include <wampproto/messages/call.h>
#include <wampproto/serializers/cbor.h>
#include <wampproto/serializers/serializer.h>

typedef struct {
    uint64_t key;
    UT_hash_handle hh;
} MapUint64ToVoid;

typedef struct {
    uint64_t key;
    uint64_t value;
    UT_hash_handle hh;
} MapUint64ToUint64;

typedef struct {
    Serializer* serializer;

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
} Session;

Session* session_new(Serializer* serializer) {
    Session* session = malloc(sizeof(Session));
    if (!session) {
        return NULL;
    }

    memset(session, 0, sizeof(Session));

    if (serializer == NULL) serializer = cbor_serializer_new();

    session->serializer = serializer;
    return session;
}

Bytes session_send_message(Session* session, const Message* message) {
    const Serializer* serializer = session->serializer;
    Bytes data = serializer->serialize(serializer, message);

    switch (message->message_type) {
        case MESSAGE_TYPE_CALL: {
            Call* call = (Call*)message;

            MapUint64ToVoid* found = NULL;
            HASH_FIND(hh, session->callRequests, &call->request_id, sizeof(uint64_t), found);
            if (!found) {
                MapUint64ToVoid* entry = malloc(sizeof(MapUint64ToVoid));
                if (entry) {
                    entry->key = call->request_id;
                    HASH_ADD(hh, session->callRequests, key, sizeof(uint64_t), entry);
                }
            }

            return data;
        }
        default:
            return data;
    }
}

void* session_receive_message(const Session* session, const Message* message) { return 0; }
