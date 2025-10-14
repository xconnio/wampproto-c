#ifndef WAMPPROTO_SESSION_DETAILS_H
#define WAMPPROTO_SESSION_DETAILS_H

#include <stdint.h>

typedef struct {
    int64_t session_id;
    char *realm;
    char *auth_id;
    char *auth_role;

} SessionDetails;

SessionDetails *session_details_new(int64_t session_id, char *realm, char *auth_id, char *auth_role);

#endif
