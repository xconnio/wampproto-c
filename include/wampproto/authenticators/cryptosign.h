#ifndef WAMPPROTO_CRYPTOSIGN_H
#define WAMPPROTO_CRYPTOSIGN_H

#include "wampproto/authenticators/authenticator.h"

ClientAuthenticator* cryptosign_authenticator_new(const char* auth_id, const char* private_key_hex, Dict* auth_extra);

#endif
