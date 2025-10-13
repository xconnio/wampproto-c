#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "wampproto/dict.h"
#include "wampproto/messages/abort.h"
#include "wampproto/messages/call.h"
#include "wampproto/messages/cancel.h"
#include "wampproto/messages/error.h"
#include "wampproto/messages/event.h"
#include "wampproto/messages/goodbye.h"
#include "wampproto/messages/hello.h"
#include "wampproto/messages/interrupt.h"
#include "wampproto/messages/invocation.h"
#include "wampproto/messages/message.h"
#include "wampproto/messages/publish.h"
#include "wampproto/messages/published.h"
#include "wampproto/messages/register.h"
#include "wampproto/messages/result.h"
#include "wampproto/messages/subscribe.h"
#include "wampproto/messages/unregister.h"
#include "wampproto/messages/unregistered.h"
#include "wampproto/messages/welcome.h"
#include "wampproto/messages/yield.h"
#include "wampproto/serializers/cbor.h"
#include "wampproto/serializers/json.h"
#include "wampproto/serializers/msgpack.h"
#include "wampproto/serializers/serializer.h"
#include "wampproto/value.h"

void test_hello_message(void);
void test_welcome_message(void);
void test_abort_messsage(void);
void test_error_message(void);
void test_cancel_message(void);
void test_interrupt_message(void);
void test_goodbye_message(void);
void test_register_message(void);
void test_unregister_message(void);
void test_unregistered_message(void);
void test_invocation_message(void);
void test_yield_message(void);
void test_result_message(void);

void test_publish_message(void);
void test_published_message(void);
void test_event_message(void);

void test_subscribe_message(void);

int main(void) {
    test_hello_message();
    test_welcome_message();

    test_abort_messsage();
    test_error_message();
    test_cancel_message();
    test_interrupt_message();
    test_goodbye_message();

    test_register_message();
    test_unregister_message();
    test_unregistered_message();

    test_invocation_message();
    test_yield_message();
    test_result_message();

    test_subscribe_message();

    test_publish_message();
    test_published_message();
    test_event_message();

    return 0;
}

static char* authid = "john";
static char* realm = "realm1";
static char* authmethod = "ticket";
static int64_t session_id = 987432;

static Message* create_hello_message(void) {
    Dict* auth_extra = create_dict();
    dict_insert(auth_extra, "name", value_str(authid));

    Dict* roles = create_dict();
    dict_insert(roles, "callee", value_bool(1));

    Value* auth_methods = value_list(1);
    value_list_append(auth_methods, value_str("ticket"));

    Hello* hello = hello_new(realm, authid, auth_extra, roles, auth_methods->list_val);

    return (Message*)hello;
}

void test_hello_message(void) {
    Message* msg = create_hello_message();
    Serializer* serializer = json_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    Message* message = serializer->deserialize(serializer, bytes);

    assert(message != NULL);

    Hello* hello = (Hello*)message;
    assert(strcmp(hello->realm, realm) == 0);

    List* list = hello->authmethods;
    assert(list->len == 1);
    assert(strcmp(value_as_str(list->items[0]), authmethod) == 0);
}

static Message* create_welcome_message(void) {
    Dict* details = create_dict();
    Dict* roles = create_dict();
    dict_insert(roles, "broker", value_bool(1));
    dict_insert(details, "roles", value_from_dict(roles));
    dict_insert(details, "authmethod", value_str(authmethod));

    return (Message*)welcome_new(session_id, details);
}

void test_welcome_message(void) {
    Message* msg = create_welcome_message();
    Serializer* serializer = msgpack_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    Message* message = serializer->deserialize(serializer, bytes);

    assert(message != NULL);

    Welcome* welcome = (Welcome*)message;
    assert(welcome->session_id == session_id);

    assert(strcmp(welcome->authmethod, authmethod) == 0);
    assert(int_from_dict(welcome->roles, "broker") == 1);
}

// ABORT Message Test
static char* abort_message = "The realm does not exist.";
static char* abort_reason = "wamp.error.no_such_realm";
static int64_t abort_kwarg_user_id = 63;
static Message* create_abort_message(void) {
    Dict* details = create_dict();
    dict_insert(details, "message", value_str(abort_message));
    Dict* kwargs = create_dict();
    dict_insert(kwargs, "user_id", value_int(abort_kwarg_user_id));

    return (Message*)abort_new(details, abort_reason, NULL, kwargs);
}

void test_abort_messsage(void) {
    Message* msg = create_abort_message();
    Serializer* serializer = json_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Abort* abort = (Abort*)msg;

    assert(strcmp(abort->reason, abort_reason) == 0);
    assert(strcmp(str_from_dict(abort->details, "message"), abort_message) == 0);
    assert(int_from_dict(abort->kwargs, "user_id") == abort_kwarg_user_id);
    assert(abort->args->len == 0);
}

// ERROR Message Test

static int64_t request_id = 12;
static char* error_uri = "com.myapp.error.object_write_protected";
static char* error_args_message = "Object is write protected.";
static int64_t error_severity_code = 3;
static Message* create_error_message(void) {
    int64_t message_type = MESSAGE_TYPE_CALL;
    Dict* details = create_dict();
    Value* args = value_list(1);
    value_list_append(args, value_str(error_args_message));
    Dict* kwargs = create_dict();
    dict_insert(kwargs, "severity", value_int(error_severity_code));

    return (Message*)error_new(message_type, request_id, details, error_uri, value_as_list(args), kwargs);
}

void test_error_message(void) {
    Message* msg = create_error_message();
    Serializer* serializer = json_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Error* error = (Error*)msg;

    assert(error->message_type == MESSAGE_TYPE_CALL);
    assert(error->request_id == request_id);
    assert(strcmp(error->uri, error_uri) == 0);

    assert(error->args->len == 1);

    Value* v = error->args->items[0];
    assert(strcmp(value_as_str(v), error_args_message) == 0);
    assert(int_from_dict(error->kwargs, "severity") == error_severity_code);
}

// CANCEL Message Test

static Message* create_cancel_message(void) {
    Dict* options = create_dict();
    dict_insert(options, "severity", value_int(error_severity_code));

    return (Message*)cancel_new(request_id, options);
}

void test_cancel_message(void) {
    Message* msg = create_cancel_message();
    Serializer* serializer = cbor_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Cancel* cancel = (Cancel*)msg;

    assert(cancel->request_id == request_id);
    assert(int_from_dict(cancel->options, "severity") == error_severity_code);
}

// INTERRUPT Message Test
static Message* create_interrupt_message(void) {
    Dict* options = create_dict();
    dict_insert(options, "severity", value_int(error_severity_code));

    return (Message*)interrupt_new(request_id, options);
}

void test_interrupt_message(void) {
    Message* msg = create_interrupt_message();
    Serializer* serializer = cbor_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Interrupt* interrupt = (Interrupt*)msg;

    assert(interrupt->request_id == request_id);
    assert(int_from_dict(interrupt->options, "severity") == error_severity_code);
}

// GOODBYE Message Test

static char* goodbye_message = "The host is shutting down now";
static char* goodbye_uri = "wamp.close.system_shutdown";
static Message* create_goodbye_message(void) {
    Dict* details = create_dict();
    dict_insert(details, "message", value_str(goodbye_message));

    return (Message*)goodbye_new(details, goodbye_uri);
}

void test_goodbye_message(void) {
    Message* msg = create_goodbye_message();
    Serializer* serializer = cbor_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Goodbye* goodbye = (Goodbye*)msg;

    assert(strcmp(goodbye->uri, goodbye_uri) == 0);
    assert(strcmp(str_from_dict(goodbye->details, "message"), goodbye_message) == 0);
}

// REGISTER Message Test

static char* register_uri = "xconn.io.example";
static Message* create_register_message(void) {
    Dict* options = create_dict();
    dict_insert(options, "uri", value_str(register_uri));

    return (Message*)register_new(request_id, options, register_uri);
}

void test_register_message(void) {
    Message* msg = create_register_message();
    Serializer* serializer = json_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Register* r = (Register*)msg;

    assert(strcmp(r->uri, register_uri) == 0);
    assert(r->request_id == request_id);
    assert(strcmp(str_from_dict(r->options, "uri"), register_uri) == 0);
}

// UNREGISTER Message Test
static int64_t registration_id = 98765;
static Message* create_unregister_message(void) { return (Message*)unregister_new(request_id, registration_id); }

void test_unregister_message(void) {
    Message* msg = create_unregister_message();
    Serializer* serializer = json_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Unregister* unregister = (Unregister*)msg;

    assert(unregister->request_id == request_id);
    assert(unregister->registration_id == registration_id);
}

// UNREGISTERED Message Test

Message* create_unregistered_message(void) { return (Message*)unregistered_new(request_id); }

void test_unregistered_message(void) {
    Message* msg = create_unregistered_message();
    Serializer* serializer = json_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);
    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Unregistered* unregistered = (Unregistered*)msg;

    assert(unregistered->request_id == request_id);
}

// INVOCATION Message Test

static Message* create_invocation_message(void) {
    // CBOR was buggy and so wrote a complex array structure to know if it passes the process
    Dict* details = create_dict();

    Dict* unknown = create_dict();
    dict_insert(unknown, "key", value_str("Nesting started"));
    Value* list = value_list(2);
    Value* sub_list = value_list(1);
    value_list_append(sub_list, value_int(123444));
    value_list_append(list, sub_list);
    value_list_append(list, value_from_dict(unknown));

    dict_insert(details, "unknown", value_str("Hello"));
    dict_insert(details, "extreme", list);

    Value* args = value_list(1);
    value_list_append(args, value_int(request_id));

    Dict* kwargs = create_dict();
    dict_insert(kwargs, "request_id", value_int(request_id));

    return (Message*)invocation_new(request_id, registration_id, details, value_as_list(args), kwargs);
}

void test_invocation_message(void) {
    Message* msg = create_invocation_message();
    Serializer* serializer = cbor_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);

    msg = serializer->deserialize(serializer, bytes);

    Serializer* json = json_serializer_new();
    bytes = json->serialize(json, msg);

    msg = json->deserialize(json, bytes);

    assert(msg != NULL);

    Invocation* invocation = (Invocation*)msg;

    assert(invocation->request_id = request_id);
    assert(invocation->registration_id = registration_id);

    assert(int_from_dict(invocation->kwargs, "request_id") == request_id);
    assert(invocation->args->len == 1);
    assert(value_as_int(invocation->args->items[0]) == request_id);
}

// YIELD Message Test

static char* yield_message_type = "yield";
Message* create_yield_message(void) {
    Dict* options = create_dict();
    dict_insert(options, "request_id", value_int(request_id));

    Value* args = value_list(1);
    value_list_append(args, value_int(request_id));

    Dict* kwargs = create_dict();
    dict_insert(kwargs, "userId", value_int(request_id));
    dict_insert(kwargs, "message_type", value_str(yield_message_type));

    return (Message*)yield_new(request_id, options, value_as_list(args), kwargs);
}

void test_yield_message(void) {
    Message* msg = create_yield_message();
    Serializer* cbor = cbor_serializer_new();
    Bytes bytes = cbor->serialize(cbor, msg);
    msg = cbor->deserialize(cbor, bytes);

    assert(msg != NULL);

    Yield* yield = (Yield*)msg;

    assert(yield->request_id == request_id);
    assert(int_from_dict(yield->options, "request_id") == request_id);

    List* list = yield->args;
    assert(value_as_int(list->items[0]) == request_id);

    assert(strcmp(str_from_dict(yield->kwargs, "message_type"), yield_message_type) == 0);
}

// RESULT Message Test

Message* create_result_message(void) {
    Dict* details = create_dict();
    dict_insert(details, "request_id", value_int(request_id));

    Value* args = value_list(1);
    value_list_append(args, value_int(request_id));

    Dict* kwargs = create_dict();
    dict_insert(kwargs, "userId", value_int(request_id));
    dict_insert(kwargs, "message_type", value_str(yield_message_type));

    return (Message*)result_new(request_id, details, value_as_list(args), kwargs);
}

void test_result_message(void) {
    Message* msg = create_result_message();
    Serializer* cbor = cbor_serializer_new();
    Bytes bytes = cbor->serialize(cbor, msg);
    msg = cbor->deserialize(cbor, bytes);

    assert(msg != NULL);

    Result* result = (Result*)msg;

    assert(result->request_id == request_id);
    assert(int_from_dict(result->details, "request_id") == request_id);

    List* list = result->args;
    assert(list->len == 1);
    assert(value_as_int(list->items[0]) == request_id);

    assert(strcmp(str_from_dict(result->kwargs, "message_type"), yield_message_type) == 0);
}

// PUBLISH Message Test

static char* publish_uri = "xconn.io.sample";
Message* create_publish_message(void) {
    Dict* options = create_dict();

    Value* args_value = value_list(1);
    value_list_append(args_value, value_str(publish_uri));
    List* args = value_as_list(args_value);

    Dict* kwargs = create_dict();
    dict_insert(kwargs, "color", value_str("orange"));

    Value* sizes_value = value_list(3);
    value_list_append(sizes_value, value_int(1));
    value_list_append(sizes_value, value_int(2));
    value_list_append(sizes_value, value_int(3));

    dict_insert(kwargs, "sizes", sizes_value);

    return (Message*)publish_new(request_id, options, publish_uri, args, kwargs);
}

void test_publish_message(void) {
    Message* msg = create_publish_message();
    Serializer* serializer = cbor_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);

    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Publish* publish = (Publish*)msg;

    assert(publish->request_id == request_id);
    assert(strcmp(publish->uri, publish_uri) == 0);

    assert(publish->args->len == 1);
    List* sizes = list_from_dict(publish->kwargs, "sizes");

    assert(sizes->len == 3);
}

// PUBLISHED Message Type

static int64_t publication_id = 9876;
Message* create_published_message(void) { return (Message*)published_new(request_id, publication_id); }

void test_published_message(void) {
    Message* msg = create_published_message();
    Serializer* serializer = msgpack_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);

    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Published* published = (Published*)msg;

    assert(published->request_id == request_id);
    assert(published->publication_id == publication_id);
}

// EVENT Message Test

static int64_t subscription_id = 200;
Message* create_event_message(void) {
    Dict* details = create_dict();

    Value* args_value = value_list(1);
    value_list_append(args_value, value_str(publish_uri));
    List* args = value_as_list(args_value);

    Dict* kwargs = create_dict();
    dict_insert(kwargs, "color", value_str("orange"));

    Value* sizes_value = value_list(3);
    value_list_append(sizes_value, value_int(1));
    value_list_append(sizes_value, value_int(2));
    value_list_append(sizes_value, value_int(3));

    dict_insert(kwargs, "sizes", sizes_value);
    return (Message*)event_new(subscription_id, publication_id, details, args, kwargs);
}

void test_event_message(void) {
    Message* msg = create_event_message();
    Serializer* serializer = cbor_serializer_new();
    Bytes bytes = serializer->serialize(serializer, msg);

    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Event* event = (Event*)msg;

    assert(event->subscription_id == subscription_id);
    assert(event->publication_id == publication_id);

    assert(event->args->len == 1);
    List* sizes = list_from_dict(event->kwargs, "sizes");

    assert(sizes->len == 3);
}

// SUBSCRIBE Message Test

Message* create_subscribe_message(void) {
    Dict* options = create_dict();

    return (Message*)subscribe_new(request_id, options, publish_uri);
}

void test_subscribe_message(void) {
    Message* msg = create_subscribe_message();
    Serializer* serializer = json_serializer_new();

    Bytes bytes = serializer->serialize(serializer, msg);

    msg = serializer->deserialize(serializer, bytes);

    assert(msg != NULL);

    Subscribe* subscribe = (Subscribe*)msg;

    assert(subscribe->request_id == request_id);
    assert(strcmp(subscribe->uri, publish_uri) == 0);
}
