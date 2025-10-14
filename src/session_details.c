#include "wampproto/session_details.h"

#include <stdlib.h>

SessionDetails *session_details_new(int64_t session_id, char *realm, char *auth_id, char *auth_role) {
    SessionDetails *session_details = calloc(1, sizeof(*session_details));

    session_details->session_id = session_id;
    session_details->realm = realm;
    session_details->auth_id = auth_id;
    session_details->auth_role = auth_role;

    return session_details;
}
