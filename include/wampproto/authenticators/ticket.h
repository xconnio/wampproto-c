#ifndef WAMPPROTO_TICKET_H
#define WAMPPROTO_TICKET_H

#include "wampproto/authenticators/authenticator.h"

ClientAuthenticator *ticket_authenticator_new(char *auth_id, char *ticket, Dict *auth_extra);

#endif
