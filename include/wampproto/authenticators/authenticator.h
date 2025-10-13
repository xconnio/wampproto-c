#ifndef WAMPPROTO_AUTHENTICATOR_H
#define WAMPPROTO_AUTHENTICATOR_H

#include "wampproto/messages/authenticate.h"
#include "wampproto/messages/challenge.h"
#include "wampproto/value.h"

typedef struct ClientAuthenticator ClientAuthenticator;

struct ClientAuthenticator {
    const char* auth_id;
    const char* auth_method;
    Dict* auth_extra;

    const char* auth_data;

    Authenticate* (*authenticate)(const ClientAuthenticator* self, const Challenge* challenge);

    void (*free)(ClientAuthenticator* self);
};

#endif
