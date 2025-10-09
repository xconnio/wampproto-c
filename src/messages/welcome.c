#include "wampproto/messages/welcome.h"
#include "wampproto/dict.h"
#include "wampproto/messages/message.h"
#include "wampproto/value.h"
#include <stdlib.h>

static List *welcome_marshal(const Message *self)
{
    const Welcome *welcome = (Welcome *)self;
    Value *list = value_list(3);
    value_list_append(list, value_int(MESSAGE_TYPE_WELCOME));
    value_list_append(list, value_int(welcome->session_id));
    value_list_append(list, value_from_dict(welcome->details));

    return (List *)list;
}

static void welcome_free(Message *self)
{
    free(self);
}

Welcome *welcome_new(int64_t session_id, Dict *details)
{
    Welcome *welcome = calloc(session_id, sizeof(*welcome));
    welcome->base.message_type = MESSAGE_TYPE_WELCOME;
    welcome->base.marshal = welcome_marshal;
    welcome->base.free = welcome_free;
    welcome->base.parse = welcome_parse;

    welcome->session_id = session_id;
    welcome->details = details;

    welcome->authid = str_from_dict(details, "authid");
    welcome->authmethod = str_from_dict(details, "authmethod");
    welcome->authrole = str_from_dict(details, "authrole");
    welcome->authextra = dict_from_dict(details, "authextra");
    if (welcome->authextra == NULL)
        welcome->authextra = create_dict(0);

    welcome->roles = list_from_dict(details, "roles");

    return welcome;
}

Message *welcome_parse(const List *val)
{

    if (!val || val->len != 3)
        return NULL;

    int64_t message_type = value_as_int(val->items[0]);
    if (message_type != MESSAGE_TYPE_WELCOME)
        return NULL;

    int64_t session_id = value_as_int(val->items[1]);
    Dict *details = value_as_dict(val->items[2]);

    return (Message *)welcome_new(session_id, details);
}
