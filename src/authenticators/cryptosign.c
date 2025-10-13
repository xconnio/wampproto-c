#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wampproto/authenticators/authenticator.h"
#include "wampproto/dict.h"
#include "wampproto/value.h"

char* sign_crypto_sign_challenge(const char* private_key_hex, const char* challenge_hex);

static Authenticate* authenticate(const ClientAuthenticator* self, const Challenge* challenge) {
    char* challenge_hex = value_as_str(dict_get(challenge->auth_extra, "challenge"));
    char* signature = sign_crypto_sign_challenge(self->auth_data, challenge_hex);
    return authenticate_new(signature, NULL);
}

ClientAuthenticator* cryptosign_authenticator_new(const char* auth_id, const char* private_key_hex, Dict* auth_extra) {
    ClientAuthenticator* auth = calloc(1, sizeof(ClientAuthenticator));
    auth->auth_id = auth_id;
    auth->auth_extra = auth_extra;
    auth->auth_method = "cryptosign";
    auth->auth_data = private_key_hex;
    auth->authenticate = authenticate;
    return auth;
}

char* sign_crypto_sign_challenge(const char* private_key_hex, const char* challenge_hex) {
    if (sodium_init() < 0) return NULL;

    unsigned char seed[crypto_sign_SEEDBYTES];
    if (sodium_hex2bin(seed, sizeof(seed), private_key_hex, strlen(private_key_hex), NULL, NULL, NULL) != 0)
        return NULL;

    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_SECRETKEYBYTES];
    crypto_sign_seed_keypair(pk, sk, seed);

    size_t challenge_hex_len = strlen(challenge_hex);
    if (challenge_hex_len % 2 != 0) return NULL;

    size_t challenge_len = challenge_hex_len / 2;
    unsigned char* challenge = malloc(challenge_len);
    if (!challenge) return NULL;

    if (sodium_hex2bin(challenge, challenge_len, challenge_hex, challenge_hex_len, NULL, NULL, NULL) != 0) {
        free(challenge);
        return NULL;
    }

    unsigned char sig[crypto_sign_BYTES];
    crypto_sign_detached(sig, NULL, challenge, challenge_len, sk);
    free(challenge);

    char sig_hex[crypto_sign_BYTES * 2 + 1];
    sodium_bin2hex(sig_hex, sizeof(sig_hex), sig, crypto_sign_BYTES);

    size_t total_len = strlen(sig_hex) + strlen(challenge_hex) + 1;
    char* result = malloc(total_len);
    if (!result) return NULL;

    snprintf(result, total_len, "%s%s", sig_hex, challenge_hex);
    return result;
}
