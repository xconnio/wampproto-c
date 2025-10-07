#include "wampproto/authenticators/authenticator.h"
#include "wampproto/authenticators/wampcra.h"
#include "wampproto/dict.h"
#include "wampproto/messages/authenticate.h"
#include "wampproto/messages/challenge.h"
#include "wampproto/value.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void test_wampcra(void);
void test_salty_wampcra(void);

ClientAuthenticator *create_wampcra_authenticator(void);

int main(void)
{
    test_wampcra();
    test_salty_wampcra();
    return 0;
}

Challenge *create_challenge(void)
{
    char *auth_method = "wampcra";
    char *challenge = "challenge_string";
    Dict *auth_extra = create_dict(2);
    Value *value = value_str(challenge);
    dict_insert(auth_extra, "challenge", value);

    return challenge_new("wampcra", auth_extra);
}

Challenge *create_salty_challenge(void)
{
    char *auth_method = "wampcra";
    char *challenge = "challenge_string";
    char *salt = "salty";
    int64_t keylen = 32;
    int64_t iterations = 100;

    Dict *auth_extra = create_dict(2);
    dict_insert(auth_extra, "challenge", value_str(challenge));
    dict_insert(auth_extra, "salt", value_str(salt));
    dict_insert(auth_extra, "keylen", value_int(keylen));
    dict_insert(auth_extra, "iterations", value_int(iterations));

    return challenge_new("wampcra", auth_extra);
}

ClientAuthenticator *create_wampcra_authenticator(void)
{
    char *auth_id = "john";
    char *secret = "mysupersecret";
    Dict *auth_extra = create_dict(2);

    Challenge *challenge = create_challenge();

    ClientAuthenticator *auth = wampcra_authenticator_new(auth_id, secret, auth_extra);

    Authenticate *authenticate = auth->authenticate(auth, challenge);

    return auth;
}

ClientAuthenticator *create_salty_wampcra_authenticator(void)
{
    char *auth_id = "john";
    char *secret = "mysupersecret";
    Dict *auth_extra = create_dict(2);

    ClientAuthenticator *auth = wampcra_authenticator_new(auth_id, secret, auth_extra);

    return auth;
}

void test_wampcra(void)
{

    char *expected_signature = "C2/eApA0ypN678de0e+bb7pFXQxZOKfW9xngWFpDWQc=";

    Challenge *challenge = create_challenge();

    ClientAuthenticator *auth = create_wampcra_authenticator();

    Authenticate *authenticate = auth->authenticate(auth, challenge);

    assert(strcmp(expected_signature, authenticate->signature) == 0);
}

void test_salty_wampcra(void)
{

    char *expected_signature = "RlOepKW4hJ7mONikJMrUyA4uwT/qPrZet2q8rKrQqXU=";

    Challenge *challenge = create_salty_challenge();

    ClientAuthenticator *auth = create_salty_wampcra_authenticator();

    Authenticate *authenticate = auth->authenticate(auth, challenge);

    assert(strcmp(expected_signature, authenticate->signature) == 0);
}
