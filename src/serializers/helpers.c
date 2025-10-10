#include "wampproto/messages/call.h"

#include "wampproto/messages/authenticate.h"
#include "wampproto/messages/challenge.h"
#include "wampproto/messages/hello.h"
#include "wampproto/messages/message.h"
#include "wampproto/messages/registered.h"
#include "wampproto/messages/welcome.h"
#include "wampproto/value.h"
#include <stdio.h>

Message *to_message(const List *data)
{

    if (data == NULL)
        return NULL;

    switch (value_as_int(data->items[0]))
    {
    case MESSAGE_TYPE_HELLO:
        return hello_parse(data);

    case MESSAGE_TYPE_WELCOME:
        return welcome_parse(data);

    case MESSAGE_TYPE_REGISTERED:
        return registered_parse(data);

    case MESSAGE_TYPE_CALL:
        return call_parse(data);

    case MESSAGE_TYPE_CHALLENGE:
        return challenge_parse(data);

    case MESSAGE_TYPE_AUTHENTICATE:
        return authenticate_parse(data);

    default:
        return NULL;
    }
}
