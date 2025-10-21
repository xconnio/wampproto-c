#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <uthash.h>

#include "wampproto/dict.h"
#include "wampproto/messages/call.h"
#include "wampproto/messages/error.h"
#include "wampproto/messages/event.h"
#include "wampproto/messages/invocation.h"
#include "wampproto/messages/message.h"
#include "wampproto/messages/publish.h"
#include "wampproto/messages/published.h"
#include "wampproto/messages/register.h"
#include "wampproto/messages/registered.h"
#include "wampproto/messages/result.h"
#include "wampproto/messages/subscribe.h"
#include "wampproto/messages/subscribed.h"
#include "wampproto/messages/unregister.h"
#include "wampproto/messages/unregistered.h"
#include "wampproto/messages/unsubscribe.h"
#include "wampproto/messages/unsubscribed.h"
#include "wampproto/messages/yield.h"
#include "wampproto/serializers/json.h"
#include "wampproto/session.h"
#include "wampproto/value.h"

void test_registering_and_unregistering(void);
void test_calling_and_invocation(void);

void test_subscribing_and_unsubscribing(void);
void test_publishing_and_events(void);

int main(void) {
    test_registering_and_unregistering();
    test_calling_and_invocation();

    test_subscribing_and_unsubscribing();
    test_publishing_and_events();

    return 0;
}

char *procedure = "xconn.io.procedure.echo";
const int64_t register_request_id = 25349185;
const int64_t registration_id = 2103333224;

Message *create_register_message(void) {
    Register *register_message = register_new(register_request_id, create_dict(), procedure);
    return (Message *)register_message;
}

Message *create_registered_message(void) {
    Registered *registered = registered_new(register_request_id, registration_id);
    return (Message *)registered;
}

char *error_procedure = "wamp.error.procedure_already_exists";
Message *create_register_error_message(void) {
    Error *error = error_new(MESSAGE_TYPE_REGISTER, register_request_id, create_dict(), error_procedure, NULL, NULL);

    return (Message *)error;
}

const int64_t unregister_request_id = 788923562;
Message *create_unregister_message(void) {
    Unregister *unregister = unregister_new(unregister_request_id, registration_id);
    return (Message *)unregister;
}

Message *create_unregistered_message(void) {
    Unregistered *unregistered = unregistered_new(unregister_request_id);
    return (Message *)unregistered;
}

char *error_unregister_procedure = "wamp.error.no_such_registration";
Message *create_unregister_error_message(void) {
    Error *error = error_new(MESSAGE_TYPE_UNREGISTER, unregister_request_id, create_dict(), error_unregister_procedure,
                             NULL, NULL);

    return (Message *)error;
}

void test_registering_and_unregistering(void) {
    // Register a procedure successfully.
    wampproto_Session *session = session_new(json_serializer_new());
    Bytes bytes = session->send_message(session, create_register_message());
    Message *msg = session->receive_message(session, create_registered_message());
    assert(msg != NULL);

    // Unregister a registered procedure successfully.
    bytes = session->send_message(session, create_unregister_message());
    msg = session->receive_message(session, create_unregistered_message());
    assert(msg != NULL);

    // Registering procedure returns an error message
    bytes = session->send_message(session, create_register_message());
    msg = session->receive_message(session, create_register_error_message());
    assert(msg != NULL);

    // Unregistering procedure returns an error message
    bytes = session->send_message(session, create_unregister_message());
    msg = session->receive_message(session, create_unregister_error_message());
    assert(msg != NULL);
}

char *topic = "xconn.io.topic.echo";
char *error_subscribe_topic = "wamp.error.not_authorized";
char *error_unubscribe_topic = "wamp.error.no_such_subscription";
const int64_t subscribe_request_id = 713845233;
const int64_t subscription_id = 5512315355;
const int64_t unsubscribe_request_id = 85346237;

Message *create_subscribe_message(void) {
    Subscribe *subscribe = subscribe_new(subscribe_request_id, create_dict(), topic);
    return (Message *)subscribe;
}

Message *create_subscribed_message(void) {
    Subscribed *subscribed = subscribed_new(subscribe_request_id, subscription_id);
    return (Message *)subscribed;
}

Message *create_subscribe_error_message(void) {
    Error *error =
        error_new(MESSAGE_TYPE_SUBSCRIBE, subscribe_request_id, create_dict(), error_subscribe_topic, NULL, NULL);

    return (Message *)error;
}

Message *create_unsubscribe_message(void) {
    Unsubscribe *unsubscribe = unsubscribe_new(unsubscribe_request_id, subscription_id);
    return (Message *)unsubscribe;
}

Message *create_unsubscribed_message(void) {
    Unsubscribed *unsubscribed = unsubscribed_new(unsubscribe_request_id);
    return (Message *)unsubscribed;
}

Message *create_unsubscribe_error_message(void) {
    Error *error =
        error_new(MESSAGE_TYPE_UNSUBSCRIBE, unsubscribe_request_id, create_dict(), error_unubscribe_topic, NULL, NULL);

    return (Message *)error;
}

void test_subscribing_and_unsubscribing(void) {
    // Subscribe to a topic successfully.
    wampproto_Session *session = session_new(json_serializer_new());
    Bytes bytes = session->send_message(session, create_subscribe_message());
    Message *msg = session->receive_message(session, create_subscribed_message());
    assert(msg != NULL);

    // Unsubscribe to a topic successfully.
    bytes = session->send_message(session, create_unsubscribe_message());
    msg = session->receive_message(session, create_unsubscribed_message());
    assert(msg != NULL);

    // Subscribing a topic returns an error.
    bytes = session->send_message(session, create_subscribe_message());
    msg = session->receive_message(session, create_subscribe_error_message());
    assert(msg != NULL);

    // Unsubscribing a topic returns an error.
    bytes = session->send_message(session, create_unsubscribe_message());
    msg = session->receive_message(session, create_unsubscribe_error_message());
    assert(msg != NULL);
}

int64_t publish_request_id = 239714735;
int64_t publish_publication_id = 4429313566;

char *publish_topic = "xconn.io.topic.publish";
char *error_publish_topic = "wamp.error.not_authorized";
Message *create_publish_message(void) {
    Dict *options = create_dict();
    dict_insert(options, "acknowledge", value_int(1));
    Publish *publish = publish_new(publish_request_id, options, publish_topic, NULL, NULL);
    return (Message *)publish;
}

Message *create_published_message(void) {
    Published *published = published_new(publish_request_id, publish_publication_id);
    return (Message *)published;
}

Message *create_publish_error_message(void) {
    Error *error = NULL;
    error = error_new(MESSAGE_TYPE_PUBLISH, publish_request_id, create_dict(), error_publish_topic, NULL, NULL);

    return (Message *)error;
}

const int64_t sum = 1 + 2;
Message *create_event_message(void) {
    Value *args_value = value_list(1);
    value_list_append(args_value, value_int(sum));
    List *args = value_as_list(args_value);

    Dict *kwargs = create_dict();
    dict_insert(kwargs, "sum", value_int(sum));

    Event *event = event_new(subscription_id, publish_publication_id, create_dict(), args, kwargs);

    return (Message *)event;
}

void test_publishing_and_events(void) {
    // Publish with acknowledge
    wampproto_Session *session = session_new(json_serializer_new());
    Bytes bytes = session->send_message(session, create_publish_message());
    Message *msg = session->receive_message(session, create_published_message());
    assert(msg != NULL);

    // Publish without acknowledge
    Message *message = create_publish_error_message();
    Publish *publish = (Publish *)message;
    publish->options = create_dict();
    bytes = session->send_message(session, message);
    msg = session->receive_message(session, create_published_message());
    assert(msg == NULL);

    // Publish returning an error message.
    bytes = session->send_message(session, create_publish_message());
    msg = session->receive_message(session, create_publish_error_message());
    assert(msg != NULL);

    // Event received
    bytes = session->send_message(session, create_subscribe_message());
    msg = session->receive_message(session, create_subscribed_message());
    assert(msg != NULL);

    msg = session->receive_message(session, create_event_message());
    assert(msg != NULL);
    Event *event = (Event *)msg;
    assert(event->args->len == 1);
    assert(value_as_int(event->args->items[0]) == sum);
}

int64_t call_request_id = 7814135;
char *call_procedure = "xconn.io.procedure.ping";
Message *create_call_message(void) {
    Value *list_value = value_list(1);
    value_list_append(list_value, value_int(sum));

    List *args = value_as_list(list_value);

    Call *call = call_new(call_request_id, create_dict(), call_procedure, args, NULL);
    return (Message *)call;
}

Message *create_result_message(void) {
    Value *list_value = value_list(1);
    value_list_append(list_value, value_int(sum));

    List *args = value_as_list(list_value);

    Result *result = result_new(call_request_id, create_dict(), args, NULL);

    return (Message *)result;
}

char *error_call_procedure = "com.myapp.error.object_write_protected";
Message *create_call_error_message(void) {
    Error *error = NULL;
    error = error_new(MESSAGE_TYPE_CALL, call_request_id, create_dict(), error_call_procedure, NULL, NULL);

    return (Message *)error;
}

int64_t invocation_request_id = 6131533;
Message *create_invocation_message(void) {
    Invocation *invocation = invocation_new(invocation_request_id, registration_id, create_dict(), NULL, NULL);
    return (Message *)invocation;
}

Message *create_yield_message(void) {
    Yield *yield = yield_new(invocation_request_id, create_dict(), NULL, NULL);
    return (Message *)yield;
}

Message *create_invocation_error_message(void) {
    Error *error = NULL;
    error = error_new(MESSAGE_TYPE_INVOCATION, invocation_request_id, create_dict(), error_call_procedure, NULL, NULL);

    return (Message *)error;
}

void test_calling_and_invocation(void) {
    // Call and Result
    wampproto_Session *session = session_new(json_serializer_new());
    Bytes bytes = session->send_message(session, create_call_message());
    Message *msg = session->receive_message(session, create_result_message());
    assert(msg != NULL);

    // Call and Call Error
    bytes = session->send_message(session, create_call_message());
    msg = session->receive_message(session, create_call_error_message());
    assert(msg != NULL);

    // Invocation and Yield
    bytes = session->send_message(session, create_register_message());
    msg = session->receive_message(session, create_registered_message());
    assert(msg != NULL);

    msg = session->receive_message(session, create_invocation_message());
    assert(msg != NULL);
    bytes = session->send_message(session, create_yield_message());
    assert(bytes.len > 0);

    // Invocation and Error
    msg = session->receive_message(session, create_invocation_message());
    assert(msg != NULL);
    bytes = session->send_message(session, create_invocation_error_message());
    assert(bytes.len > 0);

    session->free(session);
}
