#include <stdlib.h>

#include "wampproto/authenticators/authenticator.h"
#include "wampproto/value.h"

Authenticate* authenticate(const ClientAuthenticator* self, const Challenge* challenge) {
    (void)self;
    (void)challenge;

    return NULL;
}

ClientAuthenticator* anonymous_authenticator_new(const char* auth_id, Dict* auth_extra) {
    ClientAuthenticator* auth = calloc(1, sizeof(ClientAuthenticator));
    auth->auth_id = auth_id;
    auth->auth_extra = auth_extra;
    auth->auth_method = "anonymous";
    auth->authenticate = authenticate;
    return auth;
}
