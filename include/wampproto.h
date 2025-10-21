#ifndef WAMMPROTO_H
#define WAMMPROTO_H

#ifdef __cplusplus
extern "C" {
#endif

// Core modules
#include "wampproto/dict.h"
#include "wampproto/id_generator.h"
#include "wampproto/joiner.h"
#include "wampproto/mutex.h"
#include "wampproto/session.h"
#include "wampproto/session_details.h"
#include "wampproto/value.h"

// Submodules
#include "wampproto/authenticators.h"
#include "wampproto/messages.h"
#include "wampproto/serializers.h"
#include "wampproto/transports.h"

#ifdef __cplusplus
}
#endif

#endif  // WAMMPROTO_H
