#include <stdlib.h>

#include "wampproto/authenticators/authenticator.h"
#include "wampproto/value.h"

Authenticate *authenticate(const ClientAuthenticator *self, const Challenge *challenge)
{
    (void)self;
    (void)challenge;

    return authenticate_new(self->auth_data, NULL);
}

ClientAuthenticator *ticket_authenticator_new(const char *auth_id, const char *secret,
                                              Dict *auth_extra)
{
    ClientAuthenticator *auth = calloc(1, sizeof(ClientAuthenticator));
    auth->auth_id = auth_id;
    auth->auth_extra = auth_extra;
    auth->auth_method = "wampcra";
    auth->auth_data = secret;
    return auth;
}
