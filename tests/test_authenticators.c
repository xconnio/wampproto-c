#include "wampproto/authenticators/authenticator.h"
#include "wampproto/authenticators/cryptosign.h"
#include "wampproto/authenticators/ticket.h"
#include "wampproto/authenticators/wampcra.h"
#include "wampproto/dict.h"
#include "wampproto/messages/authenticate.h"
#include "wampproto/messages/challenge.h"
#include "wampproto/value.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_wampcra_authenticator(void);
void test_salty_wampcra_authenticator(void);
void test_ticket_authenticator(void);
void test_cryptosign_authenticator(void);

ClientAuthenticator *create_wampcra_authenticator(void);

int main(void)
{
    test_wampcra_authenticator();
    test_salty_wampcra_authenticator();
    test_ticket_authenticator();
    test_cryptosign_authenticator();
    return 0;
}

// WAMPCRA authenticator helper functions
Challenge *create_wampcra_challenge(void)
{
    char *auth_method = "wampcra";
    char *challenge = "challenge_string";
    Dict *auth_extra = create_dict(2);
    Value *value = value_str(challenge);
    dict_insert(auth_extra, "challenge", value);

    return challenge_new("wampcra", auth_extra);
}

Challenge *create_wampcra_salty_challenge(void)
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

    Challenge *challenge = create_wampcra_challenge();

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

// Ticket authenticator helper functions
Challenge *create_ticket_challenge(void)
{
    char *auth_method = "ticket";
    Challenge *challenge = challenge_new(auth_method, NULL);
    return challenge;
}

ClientAuthenticator *create_ticket_authenticator(void)
{
    char *auth_id = "john";
    char *ticket = "mysupersecret";

    ClientAuthenticator *auth = ticket_authenticator_new(auth_id, ticket, NULL);

    return auth;
}

// Cryptosign authenticator helper functions
Challenge *create_cryptosign_challenge(void)
{
    char *challenge_hex = "fa034062ad76352b53a25358854577730db82f367aa439709c91296d04a5716c";

    Dict *auth_extra = create_dict(1);
    dict_insert(auth_extra, "challenge", value_str(challenge_hex));

    Challenge *challenge = challenge_new("cryptosign", auth_extra);

    return challenge;
}

ClientAuthenticator *create_cryptosign_authenticator(void)
{
    char *private_key = "81224318300517874e18eb09d4e9dc7327d466ebb588a6183ae661e160cf5b3e";
    char *auth_id = "john";

    ClientAuthenticator *auth = cryptosign_authenticator_new(auth_id, private_key, NULL);

    return auth;
}

void test_wampcra_authenticator(void)
{

    char *expected_signature = "C2/eApA0ypN678de0e+bb7pFXQxZOKfW9xngWFpDWQc=";

    Challenge *challenge = create_wampcra_challenge();

    ClientAuthenticator *auth = create_wampcra_authenticator();

    Authenticate *authenticate = auth->authenticate(auth, challenge);

    assert(strcmp(expected_signature, authenticate->signature) == 0);
}

void test_salty_wampcra_authenticator(void)
{

    char *expected_signature = "RlOepKW4hJ7mONikJMrUyA4uwT/qPrZet2q8rKrQqXU=";

    Challenge *challenge = create_wampcra_salty_challenge();

    ClientAuthenticator *auth = create_salty_wampcra_authenticator();

    Authenticate *authenticate = auth->authenticate(auth, challenge);

    assert(strcmp(expected_signature, authenticate->signature) == 0);
}

void test_ticket_authenticator(void)
{

    char *expected_ticket = "mysupersecret";

    Challenge *challenge = create_ticket_challenge();

    ClientAuthenticator *auth = create_ticket_authenticator();

    Authenticate *authenticate = auth->authenticate(auth, challenge);

    assert(strcmp(expected_ticket, authenticate->signature) == 0);
}

void test_cryptosign_authenticator(void)
{
    char *expected_signature = "51732c99b47e805a327a7ef296fdd93d640d9ded63c995f1c2ddb72290ee5c87406"
                               "2a881c9383ffbac0c44be18eb7def4bceb8f73f1108514c25efdd2464d002fa0340"
                               "62ad76352b53a25358854577730db82f367aa439709c91296d04a5716c";

    Challenge *challenge = create_cryptosign_challenge();

    ClientAuthenticator *auth = create_cryptosign_authenticator();

    Authenticate *authenticate = auth->authenticate(auth, challenge);

    assert(strcmp(expected_signature, authenticate->signature) == 0);
}
