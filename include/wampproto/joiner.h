#ifndef WAMPPROTO_JOINER_H
#define WAMPPROTO_JOINER_H

#include "wampproto/authenticators/authenticator.h"
#include "wampproto/serializers/serializer.h"
#include "wampproto/session_details.h"
#include "wampproto/value.h"

typedef enum { STATE_NONE, STATE_HELLO_SENT, STATE_AUTHENTICATE_SENT, STATE_JOINED } State;

typedef struct Joiner Joiner;

typedef struct Joiner {
    const char *realm;
    Serializer *serializer;
    ClientAuthenticator *authenticator;
    SessionDetails *session_details;
    Bytes (*send_hello)(Joiner *self);
    Bytes (*receive)(Joiner *self, Bytes bytes);
    State state;
} Joiner;

Joiner *joiner_new(const char *realm, Serializer *serializer, ClientAuthenticator *authenticator);

#endif
