#ifndef WAMPPROTO_WAMPCRA_H
#define WAMPPROTO_WAMPCRA_H

#include "wampproto/authenticators/authenticator.h"

ClientAuthenticator* wampcra_authenticator_new(const char* auth_id, const char* secret, Dict* auth_extra);

#endif
