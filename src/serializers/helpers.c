#include "wampproto/messages/call.h"
#include "wampproto/messages/message.h"
#include "wampproto/messages/registered.h"
#include "wampproto/value.h"

Message *to_message(const List *data)
{
    if (data == NULL)
        return NULL;

    switch (value_as_int(data->items[0]))
    {
    case MESSAGE_TYPE_REGISTERED:
        return registered_parse(data);
    case MESSAGE_TYPE_CALL:
        return call_parse(data);
    default:
        return NULL;
    }
}
