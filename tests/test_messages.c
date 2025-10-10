#include "wampproto/dict.h"
#include "wampproto/messages/abort.h"
#include "wampproto/messages/call.h"
#include "wampproto/messages/error.h"
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
void test_abort_messsage(void);
void test_error_message(void);

int main(void)
{
    test_hello_message();
    test_welcome_message();
    test_abort_messsage();
    test_error_message();
    return 0;
}

static char *authid = "john";
static char *realm = "realm1";
static char *authmethod = "ticket";
static int64_t session_id = 987432;

static Message *create_hello_message(void)
{

    Dict *auth_extra = create_dict();
    dict_insert(auth_extra, "name", value_str(authid));

    Dict *roles = create_dict();
    dict_insert(roles, "callee", value_bool(1));

    Value *auth_methods = value_list(1);
    value_list_append(auth_methods, value_str("ticket"));

    Hello *hello = hello_new(realm, authid, auth_extra, roles, auth_methods->list_val);

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
    assert(list->len == 1);
    assert(strcmp(value_as_str(list->items[0]), authmethod) == 0);
}

static Message *create_welcome_message(void)
{
    Dict *details = create_dict();
    Dict *roles = create_dict();
    dict_insert(roles, "broker", value_bool(1));
    dict_insert(details, "roles", value_from_dict(roles));
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
    assert(int_from_dict(welcome->roles, "broker") == 1);
}

// ABORT Message Test
static char *abort_message = "The realm does not exist.";
static char *abort_reason = "wamp.error.no_such_realm";
static int64_t abort_kwarg_user_id = 63;
static Message *create_abort_message(void)
{
    Dict *details = create_dict();
    dict_insert(details, "message", value_str(abort_message));
    Dict *kwargs = create_dict();
    dict_insert(kwargs, "user_id", value_int(abort_kwarg_user_id));

    return (Message *)abort_new(details, abort_reason, NULL, kwargs);
}

void test_abort_messsage(void)
{

    Message *msg = create_abort_message();
    Serializer *serializer = json_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Abort *abort = (Abort *)msg;

    assert(strcmp(abort->reason, abort_reason) == 0);
    assert(strcmp(str_from_dict(abort->details, "message"), abort_message) == 0);
    assert(int_from_dict(abort->kwargs, "user_id") == abort_kwarg_user_id);
    assert(abort->args->len == 0);
}

// ERROR Message Test

static int64_t request_id = 12;
static char *error_uri = "com.myapp.error.object_write_protected";
static char *error_args_message = "Object is write protected.";
static int64_t error_severity_code = 3;
static Message *create_error_message(void)
{
    int64_t message_type = MESSAGE_TYPE_CALL;
    Dict *details = create_dict();
    Value *args = value_list(1);
    value_list_append(args, value_str(error_args_message));
    Dict *kwargs = create_dict();
    dict_insert(kwargs, "severity", value_int(error_severity_code));

    return (Message *)error_new(message_type, request_id, details, error_uri, value_as_list(args),
                                kwargs);
}

void test_error_message(void)
{
    Message *msg = create_error_message();
    Serializer *serializer = json_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Error *error = (Error *)msg;

    assert(error->message_type == MESSAGE_TYPE_CALL);
    assert(error->request_id == request_id);
    assert(strcmp(error->uri, error_uri) == 0);

    assert(error->args->len == 1);

    Value *v = error->args->items[0];
    assert(strcmp(value_as_str(v), error_args_message) == 0);
    assert(int_from_dict(error->kwargs, "severity") == error_severity_code);
}
