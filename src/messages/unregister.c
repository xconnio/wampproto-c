#include "wampproto/messages/unregister.h"
#include "wampproto/messages/message.h"
#include "wampproto/value.h"
#include <stdlib.h>

static List *unregister_marshal(const Message *self)
{
    Unregister *unregister = (Unregister *)self;

    Value *list = value_list(3);
    value_list_append(list, value_int(MESSAGE_TYPE_UNREGISTER));
    value_list_append(list, value_int(unregister->request_id));
    value_list_append(list, value_int(unregister->registration_id));

    return (List *)list;
}

static void unregister_free(Message *self)
{
    free(self);
}

Unregister *unregister_new(int64_t request_id, int64_t registration_id)
{
    Unregister *unregister = calloc(1, sizeof(*unregister));

    unregister->base.message_type = MESSAGE_TYPE_UNREGISTER;
    unregister->base.marshal = unregister_marshal;
    unregister->base.free = unregister_free;
    unregister->base.parse = unregister_parse;

    unregister->request_id = request_id;
    unregister->registration_id = registration_id;

    return unregister;
}

Message *unregister_parse(const List *val)
{

    if (!val || val->len != 3)
        return NULL;

    int64_t request_id = value_as_int(val->items[1]);
    int64_t registration_id = value_as_int(val->items[2]);

    return (Message *)unregister_new(request_id, registration_id);
}
