#ifndef WAMPPROTO_WAMPCRA_H
#define WAMPPROTO_WAMPCRA_H

#include "wampproto/authenticators/authenticator.h"

ClientAuthenticator *wampcra_authenticator_new(char *auth_id, char *secret, Dict *auth_extra);

#endif
