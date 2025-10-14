#include <stdlib.h>

#include "wampproto/authenticators/authenticator.h"
#include "wampproto/messages/authenticate.h"
#include "wampproto/value.h"

Authenticate* authenticate(const ClientAuthenticator* self, const Challenge* challenge) {
    return authenticate_new("", self->auth_extra);
}

ClientAuthenticator* anonymous_authenticator_new(const char* auth_id, Dict* auth_extra) {
    ClientAuthenticator* auth = calloc(1, sizeof(ClientAuthenticator));
    auth->auth_id = auth_id;
    auth->auth_extra = auth_extra;
    auth->auth_method = "anonymous";
    auth->authenticate = authenticate;
    return auth;
}
