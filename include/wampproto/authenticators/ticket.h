#ifndef WAMPPROTO_TICKET_H
#define WAMPPROTO_TICKET_H

#include "wampproto/authenticators/authenticator.h"

ClientAuthenticator* ticket_authenticator_new(const char* auth_id, const char* ticket, Dict* auth_extra);

#endif
