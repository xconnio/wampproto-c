
#include "wampproto/messages/message.h"
#include "wampproto/messages/registered.h"
#include "wampproto/value.h"

Message *to_message(const List *data)
{
    if (data == NULL)
        return NULL;

    switch (value_as_int(data->items[0]))
    {
    case 65:
        return registered_parse(data);
    default:
        return NULL;
    }
}
