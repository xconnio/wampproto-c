#ifndef WAMPPROTO_MESSAGE_H
#define WAMPPROTO_MESSAGE_H

#include "wampproto/value.h"

typedef struct Message Message;

struct Message {
    int   (*message_type)(const Message *self);
    Value *(*marshal)(const Message *self);
    void  (*free)(Message *self);

    Message *(*parse)(const Value *data);
};

#endif

#ifndef WAMPPROTO_REGISTERED_MESSAGE_H
#define WAMPPROTO_REGISTERED_MESSAGE_H

typedef struct {
    Message base;
    int64_t request_id;
    int64_t registration_id;
} Registered;

Registered * registered_init(void);
Registered *registered_new(int64_t request_id, int64_t registration_id);
Message *registered_parse(const Value *val);

#endif
