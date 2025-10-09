#include "wampproto/dict.h"
#include "wampproto/messages/hello.h"
#include "wampproto/messages/message.h"
#include "wampproto/messages/welcome.h"
#include "wampproto/serializers/json.h"
#include "wampproto/serializers/msgpack.h"
#include "wampproto/serializers/serializer.h"
#include "wampproto/value.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_hello_message(void);
void test_welcome_message(void);

int main(void)
{
    test_hello_message();
    test_welcome_message();
    return 0;
}

static char *authid = "john";
static char *realm = "realm1";
static char *authmethod = "ticket";
static int64_t session_id = 987432;

static Message *create_hello_message(void)
{

    Dict *auth_extra = create_dict(1);
    dict_insert(auth_extra, "name", value_str(authid));

    Dict *roles = create_dict(2);
    dict_insert(roles, "callee", value_bool(1));

    Value *auth_methods = value_list(1);
    value_list_append(auth_methods, value_str("ticket"));

    Hello *hello = hello_new(realm, authid, auth_extra, roles, &auth_methods->list_val);

    return (Message *)hello;
}

void test_hello_message(void)
{
    Message *msg = create_hello_message();
    Serializer *serializer = json_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    Message *message = serializer->deserialize(serializer, bytes);

    assert(message != NULL);

    Hello *hello = (Hello *)message;
    assert(strcmp(hello->realm, realm) == 0);

    List *list = hello->authmethods;
    printf("\nLENGTH: %ld\n", list->len);
    assert(list->len == 1);
    assert(strcmp(value_as_str(list->items[0]), authmethod) == 0);
}

static Message *create_welcome_message(void)
{
    Dict *details = create_dict(2);
    dict_insert(details, "broker", value_bool(1));
    dict_insert(details, "authmethod", value_str(authmethod));

    return (Message *)welcome_new(session_id, details);
}

void test_welcome_message(void)
{
    Message *msg = create_welcome_message();
    Serializer *serializer = msgpack_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    Message *message = serializer->deserialize(serializer, bytes);

    assert(message != NULL);

    Welcome *welcome = (Welcome *)message;
    assert(welcome->session_id == session_id);

    assert(strcmp(welcome->authmethod, authmethod) == 0);
}
