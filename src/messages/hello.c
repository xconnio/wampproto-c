#include "wampproto/messages/hello.h"
#include "wampproto/dict.h"
#include "wampproto/messages/message.h"
#include "wampproto/value.h"
#include <stdio.h>
#include <stdlib.h>

static Dict *default_roles(void);

static List *hello_marshal(const Message *self)
{
    const Hello *hello = (const Hello *)self;
    Value *list = value_list(3);
    value_list_append(list, value_int(MESSAGE_TYPE_HELLO));
    value_list_append(list, value_str(hello->realm));
    Dict *details = hello->details;
    if (details->count == 0)
        details = default_roles();
    value_list_append(list, value_from_dict(details));

    return (List *)list;
}

static void hello_free(Message *self)
{
    free(self);
}

static Dict *default_roles(void)
{
    Dict *roles = create_dict();
    dict_insert(roles, "caller", value_from_dict(create_dict()));
    dict_insert(roles, "publisher", value_from_dict(create_dict()));
    dict_insert(roles, "subscriber", value_from_dict(create_dict()));
    dict_insert(roles, "callee", value_from_dict(create_dict()));

    return roles;
}

Dict *create_hello_details(Hello *self)
{
    Dict *details = create_dict();
    dict_insert(details, "authid", value_str(self->authid));
    if (self->auth_extra && self->auth_extra->count > 0)
        dict_insert(details, "authextra", value_from_dict(self->auth_extra));
    if (self->roles && self->roles->count > 0)
        dict_insert(details, "roles", value_from_dict(self->roles));
    if (self->authmethods && self->authmethods->len > 0)
        dict_insert(details, "authmethods", value_from_list(self->authmethods));

    return details;
}

Hello *hello_new(char *realm, char *auth_id, Dict *auth_extra, Dict *roles, List *auth_methods)
{
    Hello *hello = calloc(1, sizeof(*hello));
    hello->base.message_type = MESSAGE_TYPE_HELLO;
    hello->base.marshal = hello_marshal;
    hello->base.free = hello_free;
    hello->base.parse = hello_parse;

    hello->realm = realm;
    hello->authid = auth_id;
    hello->auth_extra = auth_extra;
    hello->roles = roles;
    hello->authmethods = auth_methods;
    hello->details = create_hello_details(hello);

    return hello;
}

Message *hello_parse(const List *val)
{

    if (!val || val->len != 3)
        return NULL;

    int64_t message_type = value_as_int(val->items[0]);
    if (message_type != MESSAGE_TYPE_HELLO)
        return NULL;

    char *realm = value_as_str(val->items[1]);
    Dict *details = value_as_dict(val->items[2]);

    char *auth_id = str_from_dict(details, "authid");
    Dict *auth_extra = dict_from_dict(details, "authextra");
    Dict *roles = dict_from_dict(details, "roles");
    List *auth_methods = list_from_dict(details, "authmethods");

    return (Message *)hello_new(realm, auth_id, auth_extra, roles, auth_methods);
}
