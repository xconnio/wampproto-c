#ifndef WAMPPROTO_C_ANONYMOUS_H
#define WAMPPROTO_C_ANONYMOUS_H

#include "wampproto/authenticators/authenticator.h"

ClientAuthenticator* anonymous_authenticator_new(const char* auth_id, Dict* auth_extra);

#endif
