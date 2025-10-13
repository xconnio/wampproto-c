#include "wampproto/messages/publish.h"
#include "wampproto/value.h"
#include <stdlib.h>

static List *publish_marshal(const Message *self)
{
    Publish *publish = (Publish *)self;

    int length = 4;
    if (publish->args != NULL)
        length = 5;
    if (publish->kwargs != NULL)
        length = 6;

    Value *list = value_list(length);

    value_list_append(list, value_int(MESSAGE_TYPE_PUBLISH));
    value_list_append(list, value_int(publish->request_id));
    value_list_append(list, value_from_dict(publish->options));
    value_list_append(list, value_str(publish->uri));

    if (publish->args != NULL)
        value_list_append(list, value_from_list(publish->args));

    if (publish->kwargs != NULL)
    {
        if (publish->args == NULL)
            value_list_append(list, value_list(0));

        value_list_append(list, value_from_dict(publish->kwargs));
    }

    return (List *)list;
}

static void publish_free(Message *self)
{
    Publish *publish = (Publish *)self;

    value_free(value_from_dict(publish->options));

    if (publish->args != NULL)
        value_free(value_from_list(publish->args));

    if (publish->kwargs != NULL)
        value_free(value_from_dict(publish->kwargs));

    free(publish);
}

Publish *publish_new(int64_t request_id, Dict *options, char *uri, List *args, Dict *kwargs)
{
    Publish *publish = calloc(1, sizeof(*publish));

    publish->base.message_type = MESSAGE_TYPE_PUBLISH;
    publish->base.marshal = publish_marshal;
    publish->base.free = publish_free;
    publish->base.parse = publish_parse;

    publish->request_id = request_id;
    publish->options = options;
    publish->uri = uri;
    publish->args = args;
    publish->kwargs = kwargs;

    return publish;
}

Message *publish_parse(const List *val)
{

    if (!val || val->len < 4)
        return NULL;

    int64_t request_id = value_as_int(val->items[1]);
    Dict *options = value_as_dict(val->items[2]);
    char *uri = value_as_str(val->items[3]);

    List *args = NULL;
    if (val->len >= 5)
        args = value_as_list(val->items[4]);

    Dict *kwargs = NULL;
    if (val->len == 6)
        kwargs = value_as_dict(val->items[5]);

    return (Message *)publish_new(request_id, options, uri, args, kwargs);
}
