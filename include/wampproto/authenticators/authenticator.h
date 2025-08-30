#ifndef WAMPPROTO_AUTHENTICATOR_H
#define WAMPPROTO_AUTHENTICATOR_H

#include "wampproto/value.h"
#include "wampproto/messages/challenge.h"
#include "wampproto/messages/authenticate.h"

typedef struct ClientAuthenticator ClientAuthenticator;

struct ClientAuthenticator {
    const char *(*auth_method)(const ClientAuthenticator *self);
    const char *(*auth_id)(const ClientAuthenticator *self);
    Value *(*auth_extra)(const ClientAuthenticator *self);

    Authenticate *(*authenticate)(const ClientAuthenticator *self, const Challenge *challenge);

    void (*free)(ClientAuthenticator *self);
};

#endif
