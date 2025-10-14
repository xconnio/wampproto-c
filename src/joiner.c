#include "wampproto/joiner.h"

#include <stdio.h>
#include <stdlib.h>

#include "wampproto/authenticators/authenticator.h"
#include "wampproto/dict.h"
#include "wampproto/messages/authenticate.h"
#include "wampproto/messages/challenge.h"
#include "wampproto/messages/hello.h"
#include "wampproto/messages/message.h"
#include "wampproto/messages/welcome.h"
#include "wampproto/serializers/serializer.h"
#include "wampproto/session_details.h"
#include "wampproto/value.h"

static Dict *client_roles(void) {
    Value *caller_features = value_dict();
    value_dict_set(caller_features, "features", value_dict());

    Value *callee_features = value_dict();
    value_dict_set(callee_features, "features", value_dict());

    Value *publisher_features = value_dict();
    value_dict_set(publisher_features, "features", value_dict());

    Value *subscriber_features = value_dict();
    value_dict_set(subscriber_features, "features", value_dict());

    Value *roles = value_dict();
    value_dict_set(roles, "caller", caller_features);
    value_dict_set(roles, "callee", callee_features);
    value_dict_set(roles, "publisher", publisher_features);
    value_dict_set(roles, "subscriber", subscriber_features);

    return value_as_dict(roles);
}

static Bytes send_hello(Joiner *self) {
    Joiner *joiner = self;
    ClientAuthenticator *authenticator = joiner->authenticator;
    Serializer *serializer = joiner->serializer;
    Dict *roles = create_dict();

    Value *value_auth_methods = value_list(1);
    value_list_append(value_auth_methods, value_str(authenticator->auth_method));
    List *auth_methods = value_as_list(value_auth_methods);

    Hello *hello = hello_new(joiner->realm, (char *)authenticator->auth_id, authenticator->auth_extra, client_roles(),
                             auth_methods);

    joiner->state = STATE_HELLO_SENT;

    return serializer->serialize(serializer, (Message *)hello);
}

static Message *receive_message(Joiner *self, Message *msg) {
    Joiner *joiner = self;
    if (msg->message_type == MESSAGE_TYPE_WELCOME) {
        if (joiner->state != STATE_HELLO_SENT && joiner->state != STATE_AUTHENTICATE_SENT) {
            return NULL;
        }

        Welcome *welcome = (Welcome *)msg;

        joiner->session_details =
            session_details_new(welcome->session_id, joiner->realm, welcome->authid, welcome->authrole);
        joiner->state = STATE_JOINED;
        return NULL;

    } else if (msg->message_type == MESSAGE_TYPE_CHALLENGE) {
        if (joiner->state != STATE_HELLO_SENT) {
            return NULL;
        }

        ClientAuthenticator *authenticator = joiner->authenticator;
        Authenticate *authenticate = authenticator->authenticate(authenticator, (Challenge *)msg);

        joiner->state = STATE_AUTHENTICATE_SENT;
        return (Message *)authenticate;

    } else {
        return NULL;
    }
}

static Bytes receive(Joiner *self, Bytes bytes) {
    Joiner *joiner = self;
    Serializer *serializer = joiner->serializer;

    Message *msg = serializer->deserialize(serializer, bytes);
    Message *received_message = receive_message(self, msg);

    if (received_message && received_message->message_type == MESSAGE_TYPE_AUTHENTICATE)
        return serializer->serialize(serializer, received_message);

    Value *empty_bytes_value = value_bytes(NULL, 0);
    return empty_bytes_value->bytes_val;
}

Joiner *joiner_new(char *realm, Serializer *serializer, ClientAuthenticator *authenticator) {
    Joiner *joiner = calloc(1, sizeof(*joiner));
    joiner->realm = realm;
    joiner->serializer = serializer;
    joiner->authenticator = authenticator;
    joiner->state = STATE_NONE;
    joiner->send_hello = send_hello;
    joiner->receive = receive;

    return joiner;
}
