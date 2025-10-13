#include <stdio.h>

#include "wampproto/messages/abort.h"
#include "wampproto/messages/authenticate.h"
#include "wampproto/messages/call.h"
#include "wampproto/messages/cancel.h"
#include "wampproto/messages/challenge.h"
#include "wampproto/messages/error.h"
#include "wampproto/messages/event.h"
#include "wampproto/messages/goodbye.h"
#include "wampproto/messages/hello.h"
#include "wampproto/messages/interrupt.h"
#include "wampproto/messages/invocation.h"
#include "wampproto/messages/message.h"
#include "wampproto/messages/publish.h"
#include "wampproto/messages/published.h"
#include "wampproto/messages/register.h"
#include "wampproto/messages/registered.h"
#include "wampproto/messages/result.h"
#include "wampproto/messages/subscribe.h"
#include "wampproto/messages/unregister.h"
#include "wampproto/messages/unregistered.h"
#include "wampproto/messages/welcome.h"
#include "wampproto/messages/yield.h"
#include "wampproto/value.h"

Message* to_message(const List* data) {
    if (data == NULL) return NULL;

    switch (value_as_int(data->items[0])) {
        case MESSAGE_TYPE_HELLO:
            return hello_parse(data);

        case MESSAGE_TYPE_WELCOME:
            return welcome_parse(data);

        case MESSAGE_TYPE_ABORT:
            return abort_parse(data);

        case MESSAGE_TYPE_CANCEL:
            return cancel_parse(data);

        case MESSAGE_TYPE_INTERRUPT:
            return interrupt_parse(data);

        case MESSAGE_TYPE_GOODBYE:
            return goodbye_parse(data);

        case MESSAGE_TYPE_ERROR:
            return error_parse(data);

        case MESSAGE_TYPE_REGISTER:
            return register_parse(data);

        case MESSAGE_TYPE_UNREGISTER:
            return unregister_parse(data);

        case MESSAGE_TYPE_REGISTERED:
            return registered_parse(data);

        case MESSAGE_TYPE_UNREGISTERED:
            return unregistered_parse(data);

        case MESSAGE_TYPE_CALL:
            return call_parse(data);

        case MESSAGE_TYPE_INVOCATION:
            return invocation_parse(data);

        case MESSAGE_TYPE_YIELD:
            return yield_parse(data);

        case MESSAGE_TYPE_RESULT:
            return result_parse(data);

        case MESSAGE_TYPE_PUBLISH:
            return publish_parse(data);

        case MESSAGE_TYPE_PUBLISHED:
            return published_parse(data);

        case MESSAGE_TYPE_EVENT:
            return event_parse(data);

        case MESSAGE_TYPE_SUBSCRIBE:
            return subscribe_parse(data);

        case MESSAGE_TYPE_CHALLENGE:
            return challenge_parse(data);

        case MESSAGE_TYPE_AUTHENTICATE:
            return authenticate_parse(data);

        default:
            return NULL;
    }
}
