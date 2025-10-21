#include "wampproto/session.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthash.h>

#include "wampproto/messages/abort.h"
#include "wampproto/messages/call.h"
#include "wampproto/messages/error.h"
#include "wampproto/messages/event.h"
#include "wampproto/messages/goodbye.h"
#include "wampproto/messages/invocation.h"
#include "wampproto/messages/message.h"
#include "wampproto/messages/publish.h"
#include "wampproto/messages/published.h"
#include "wampproto/messages/register.h"
#include "wampproto/messages/registered.h"
#include "wampproto/messages/result.h"
#include "wampproto/messages/subscribe.h"
#include "wampproto/messages/subscribed.h"
#include "wampproto/messages/unregister.h"
#include "wampproto/messages/unregistered.h"
#include "wampproto/messages/unsubscribe.h"
#include "wampproto/messages/unsubscribed.h"
#include "wampproto/messages/yield.h"
#include "wampproto/mutex.h"
#include "wampproto/serializers/cbor.h"
#include "wampproto/serializers/serializer.h"
#include "wampproto/value.h"

static void session_free(wampproto_Session* session);

static void add_void_entry(MapUint64ToVoid** requests, int64_t request_id);
static void add_value_entry(MapUint64ToUint64** requests, int64_t request_id, int64_t value);
static Bytes empty_bytes(void);
static int64_t remove_void_entry(MapUint64ToVoid** requests, int64_t request_id);
static int64_t remove_value_entry(MapUint64ToUint64** requests, int64_t request_id);
static int64_t void_entry_exists(MapUint64ToVoid** requests, int64_t request_id);

wampproto_Session* session_new(Serializer* serializer) {
    wampproto_Session* session = malloc(sizeof(*session));
    if (!session) {
        return NULL;
    }

    memset(session, 0, sizeof(wampproto_Session));

    if (serializer == NULL) serializer = cbor_serializer_new();

    session->serializer = serializer;
    session->mutex = mutex_create();
    session->send_message = session_send_message;
    session->receive_message = session_receive_message;
    session->free = session_free;
    return session;
}

Bytes session_send_message(wampproto_Session* session, const Message* message) {
    mutex_lock(session->mutex);
    const Serializer* serializer = session->serializer;

    Bytes bytes = empty_bytes();

    switch (message->message_type) {
        case MESSAGE_TYPE_CALL: {
            Call* call = (Call*)message;

            add_void_entry(&session->callRequests, call->request_id);

            bytes = serializer->serialize(serializer, message);
            break;
        }
        case MESSAGE_TYPE_REGISTER: {
            Register* r = (Register*)message;

            add_void_entry(&session->registerRequests, r->request_id);

            bytes = serializer->serialize(serializer, message);
            break;
        }
        case MESSAGE_TYPE_UNREGISTER: {
            Unregister* unregister = (Unregister*)message;

            add_value_entry(&session->unregisterRequests, unregister->request_id, unregister->registration_id);

            bytes = serializer->serialize(serializer, message);
            break;
        }
        case MESSAGE_TYPE_YIELD: {
            Yield* yield = (Yield*)message;

            if (!remove_void_entry(&session->invocationRequests, yield->request_id)) {
                break;
            }

            bytes = serializer->serialize(serializer, message);
            break;
        }
        case MESSAGE_TYPE_PUBLISH: {
            Publish* publish = (Publish*)message;
            int64_t acknowledge = int_from_dict(publish->options, "acknowledge");
            if (acknowledge == 1) {
                add_void_entry(&session->publishRequests, publish->request_id);
            }

            bytes = serializer->serialize(serializer, message);
            break;
        }
        case MESSAGE_TYPE_SUBSCRIBE: {
            Subscribe* subscribe = (Subscribe*)message;
            add_void_entry(&session->subscribeRequests, subscribe->request_id);

            bytes = serializer->serialize(serializer, message);
            break;
        }
        case MESSAGE_TYPE_UNSUBSCRIBE: {
            Unsubscribe* unsubscribe = (Unsubscribe*)message;
            add_value_entry(&session->unsubscribeRequests, unsubscribe->request_id, unsubscribe->subscription_id);

            bytes = serializer->serialize(serializer, message);
            break;
        }
        case MESSAGE_TYPE_ERROR: {
            Error* error = (Error*)message;
            if (error->message_type != MESSAGE_TYPE_INVOCATION) break;

            if (!remove_void_entry(&session->invocationRequests, error->request_id)) {
                break;
            }

            bytes = serializer->serialize(serializer, message);
            break;
        }
        case MESSAGE_TYPE_GOODBYE: {
            bytes = serializer->serialize(serializer, message);
            break;
        }

        default:
            bytes = serializer->serialize(serializer, message);
            break;
    }

    mutex_unlock(session->mutex);

    return bytes;
}

Message* session_receive_message(wampproto_Session* session, Message* message) {
    mutex_lock(session->mutex);

    Message* output = NULL;

    switch (message->message_type) {
        case MESSAGE_TYPE_RESULT: {
            Result* result = (Result*)message;
            if (remove_void_entry(&session->callRequests, result->request_id)) {
                output = message;
                break;
            }
            break;
        }
        case MESSAGE_TYPE_REGISTERED: {
            Registered* registered = (Registered*)message;
            if (remove_void_entry(&session->registerRequests, registered->request_id)) {
                add_void_entry(&session->registrations, registered->registration_id);
                output = message;
                break;
            }
            break;
        }
        case MESSAGE_TYPE_UNREGISTERED: {
            Unregistered* unregistered = (Unregistered*)message;
            int64_t registration_id = remove_value_entry(&session->unregisterRequests, unregistered->request_id);
            if (!registration_id) break;

            if (remove_void_entry(&session->registrations, registration_id)) {
                output = message;
                break;
            }

            break;
        }
        case MESSAGE_TYPE_INVOCATION: {
            Invocation* invocation = (Invocation*)message;
            if (!void_entry_exists(&session->registrations, invocation->registration_id)) {
                break;
            }

            add_void_entry(&session->invocationRequests, invocation->request_id);
            output = message;
            break;
        }
        case MESSAGE_TYPE_PUBLISHED: {
            Published* published = (Published*)message;
            if (remove_void_entry(&session->publishRequests, published->request_id)) {
                output = message;
                break;
            }
            break;
        }
        case MESSAGE_TYPE_SUBSCRIBED: {
            Subscribed* subscribed = (Subscribed*)message;
            if (remove_void_entry(&session->subscribeRequests, subscribed->request_id)) {
                add_void_entry(&session->subscriptions, subscribed->subscription_id);
                output = message;
                break;
            }
            break;
        }
        case MESSAGE_TYPE_UNSUBSCRIBED: {
            Unsubscribed* unsubscribed = (Unsubscribed*)message;
            uint64_t subscription_id = remove_value_entry(&session->unsubscribeRequests, unsubscribed->request_id);
            if (!subscription_id) break;

            if (!remove_void_entry(&session->subscriptions, subscription_id)) {
                break;
            }
            output = message;
            break;
        }
        case MESSAGE_TYPE_EVENT: {
            Event* event = (Event*)message;
            if (void_entry_exists(&session->subscriptions, event->subscription_id)) {
                output = message;
                break;
            }
            break;
        }
        case MESSAGE_TYPE_ERROR: {
            Error* error = (Error*)message;
            switch (error->message_type) {
                case MESSAGE_TYPE_CALL: {
                    if (!remove_void_entry(&session->callRequests, error->request_id)) {
                        break;
                    }
                    output = message;
                    break;
                }
                case MESSAGE_TYPE_REGISTER: {
                    if (!remove_void_entry(&session->registerRequests, error->request_id)) {
                        break;
                    }
                    output = message;
                    break;
                }
                case MESSAGE_TYPE_UNREGISTER: {
                    if (!remove_value_entry(&session->unregisterRequests, error->request_id)) {
                        break;
                    }
                    output = message;
                    break;
                }
                case MESSAGE_TYPE_SUBSCRIBE: {
                    if (!remove_void_entry(&session->subscribeRequests, error->request_id)) {
                        break;
                    }
                    output = message;
                    break;
                }
                case MESSAGE_TYPE_UNSUBSCRIBE: {
                    if (!remove_value_entry(&session->unsubscribeRequests, error->request_id)) {
                        break;
                    }
                    output = message;
                    break;
                }
                case MESSAGE_TYPE_PUBLISH: {
                    if (!remove_void_entry(&session->publishRequests, error->request_id)) {
                        break;
                    }
                    output = message;
                    break;
                }
                default:
                    break;
            }
            output = message;
            break;
        }
        case MESSAGE_TYPE_ABORT: {
            output = message;
            break;
        }
        case MESSAGE_TYPE_GOODBYE: {
            output = message;
            break;
        }
        default:
            break;
    }

    mutex_unlock(session->mutex);
    return output;
}

static void add_void_entry(MapUint64ToVoid** requests, int64_t request_id) {
    MapUint64ToVoid* found = NULL;
    HASH_FIND(hh, *requests, &request_id, sizeof(uint64_t), found);
    if (!found) {
        MapUint64ToVoid* entry = malloc(sizeof(*entry));
        if (entry) {
            entry->key = request_id;
            HASH_ADD(hh, *requests, key, sizeof(uint64_t), entry);
        }
    }
}

static void add_value_entry(MapUint64ToUint64** requests, int64_t request_id, int64_t value) {
    MapUint64ToUint64* found = NULL;
    HASH_FIND(hh, *requests, &request_id, sizeof(uint64_t), found);
    if (!found) {
        MapUint64ToUint64* entry = malloc(sizeof(*entry));
        if (entry) {
            entry->key = request_id;
            entry->value = value;
            HASH_ADD(hh, *requests, key, sizeof(uint64_t), entry);
        }
    }
}

static Bytes empty_bytes(void) {
    Value* bytes = value_bytes(NULL, 0);
    return bytes->bytes_val;
}

static int64_t void_entry_exists(MapUint64ToVoid** requests, int64_t request_id) {
    MapUint64ToVoid* found = NULL;
    HASH_FIND(hh, *requests, &request_id, sizeof(uint64_t), found);
    if (!found) return 0;

    return 1;
}

static int64_t remove_void_entry(MapUint64ToVoid** requests, int64_t request_id) {
    MapUint64ToVoid* found = NULL;
    HASH_FIND(hh, *requests, &request_id, sizeof(uint64_t), found);
    if (!found) return 0;

    HASH_DEL(*requests, found);
    return 1;
}

static int64_t remove_value_entry(MapUint64ToUint64** requests, int64_t request_id) {
    MapUint64ToUint64* found = NULL;
    HASH_FIND(hh, *requests, &request_id, sizeof(uint64_t), found);
    if (!found) return 0;

    int64_t value = found->value;

    HASH_DEL(*requests, found);
    return value;
}

void free_MapUint64ToVoid(MapUint64ToVoid** map) {
    MapUint64ToVoid *current, *tmp;
    HASH_ITER(hh, *map, current, tmp) {
        HASH_DEL(*map, current);
        free(current);
    }
    *map = NULL;
}

void free_MapUint64ToUint64(MapUint64ToUint64** map) {
    MapUint64ToUint64 *current, *tmp;
    HASH_ITER(hh, *map, current, tmp) {
        HASH_DEL(*map, current);
        free(current);
    }
    *map = NULL;
}

void session_free(wampproto_Session* session) {
    if (!session) return;

    free_MapUint64ToVoid(&session->callRequests);
    free_MapUint64ToVoid(&session->registerRequests);
    free_MapUint64ToVoid(&session->registrations);
    free_MapUint64ToVoid(&session->invocationRequests);
    free_MapUint64ToUint64(&session->unregisterRequests);

    free_MapUint64ToVoid(&session->publishRequests);
    free_MapUint64ToVoid(&session->subscribeRequests);
    free_MapUint64ToVoid(&session->subscriptions);
    free_MapUint64ToUint64(&session->unsubscribeRequests);

    if (session->mutex) mutex_destroy(session->mutex);
    if (session->serializer) session->serializer->free(session->serializer);

    free(session);
}
