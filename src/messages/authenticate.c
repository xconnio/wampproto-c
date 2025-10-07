#include "wampproto/messages/authenticate.h"
#include "wampproto/dict.h"
#include "wampproto/messages/message.h"
#include "wampproto/value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static List *authenticate_marshal(const Message *self)
{

    const Authenticate *authenticate = (const Authenticate *)self;

    Value *list = value_list(3);
    value_list_append(list, value_int(MESSAGE_TYPE_AUTHENTICATE));
    value_list_append(list, value_str(authenticate->signature));

    Value *auth_extra = value_dict();
    auth_extra->dict_val = authenticate->auth_extra;

    value_list_append(list, auth_extra);

    return (List *)list;
}

void authenticate_free(Message *self)
{
    Authenticate *authenticate = (Authenticate *)self;

    dict_free(authenticate->auth_extra);

    free(authenticate);
}

Authenticate *authenticate_new(const char *signature, Dict *auth_extra)
{

    Authenticate *authenticate = calloc(1, sizeof(Authenticate));
    if (!authenticate)
        return NULL;

    authenticate->base.message_type = MESSAGE_TYPE_AUTHENTICATE;
    authenticate->base.marshal = authenticate_marshal;
    authenticate->base.free = authenticate_free;
    authenticate->base.parse = authenticate_parse;

    authenticate->signature = signature;
    authenticate->auth_extra = auth_extra;

    return authenticate;
}

Message *authenticate_parse(const List *list)
{
    if (!list || list->len < 3)
        return NULL;

    char *signature = value_as_str(list->items[1]);
    Dict *auth_extra = value_as_dict(list->items[2]);

    return (Message *)authenticate_new(signature, auth_extra);
}
