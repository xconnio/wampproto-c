#include "wampproto/messages/published.h"
#include "wampproto/messages/message.h"
#include "wampproto/value.h"
#include <stdlib.h>

static List *published_marshal(const Message *self)
{
    Published *published = (Published *)self;

    Value *list = value_list(3);
    value_list_append(list, value_int(MESSAGE_TYPE_PUBLISHED));
    value_list_append(list, value_int(published->request_id));
    value_list_append(list, value_int(published->publication_id));

    return (List *)list;
}

static void published_free(Message *self)
{
    free(self);
}

Published *published_new(int64_t request_id, int64_t publication_id)
{
    Published *published = calloc(1, sizeof(*published));

    published->base.message_type = MESSAGE_TYPE_PUBLISHED;
    published->base.marshal = published_marshal;
    published->base.free = published_free;
    published->base.parse = published_parse;

    published->request_id = request_id;
    published->publication_id = publication_id;

    return published;
}

Message *published_parse(const List *val)
{
    if (!val || val->len != 3)
        return NULL;

    int64_t request_id = value_as_int(val->items[1]);
    int64_t publication_id = value_as_int(val->items[2]);

    return (Message *)published_new(request_id, publication_id);
}
