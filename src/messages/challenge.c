#include "wampproto/messages/challenge.h"
#include "wampproto/dict.h"
#include "wampproto/messages/message.h"
#include "wampproto/value.h"
#include <stdio.h>
#include <stdlib.h>

static List *challenge_marshal(const Message *self)
{
    Challenge *challenge = (Challenge *)self;
    Value *list = value_list(3);
    value_list_append(list, value_int(MESSAGE_TYPE_CHALLENGE));
    value_list_append(list, value_str(challenge->auth_method));

    Value *auth_extra = value_dict();
    auth_extra->dict_val = challenge->auth_extra;

    value_list_append(list, auth_extra);

    return (List *)list;
}

void challenge_free(Message *self)
{
    Challenge *challenge = (Challenge *)self;

    free(challenge->auth_method);

    if (challenge->auth_extra)
        dict_free(challenge->auth_extra);

    free(challenge);
}

Challenge *challenge_new(char *auth_method, Dict *auth_extra)
{

    Challenge *message = calloc(1, sizeof(*message));
    if (!message)
        return NULL;

    message->base.message_type = MESSAGE_TYPE_CHALLENGE;
    message->base.marshal = challenge_marshal;
    message->base.free = challenge_free;
    message->base.parse = challenge_parse;

    message->auth_method = auth_method;
    message->auth_extra = auth_extra;

    return message;
}

Message *challenge_parse(const List *list)
{

    if (!list || list->len < 3)
        return NULL;

    char *auth_method = value_as_str(list->items[1]);
    Dict *extra = value_as_dict(list->items[2]);

    return (Message *)challenge_new(auth_method, extra);
}
