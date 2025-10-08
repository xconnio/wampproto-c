#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "wampproto/authenticators/authenticator.h"
#include "wampproto/dict.h"
#include "wampproto/value.h"
#include <sodium.h>
#include <string.h>

#ifdef HAVE_OPENSSL
#include <openssl/evp.h>
#endif

char *sign_signature(const char *secret, Dict *auth_extra);

static Authenticate *authenticate(const ClientAuthenticator *self, const Challenge *challenge)
{

    Dict *auth_extra = challenge->auth_extra;

    char *signature = sign_signature(self->auth_data, auth_extra);
    return authenticate_new(signature, NULL);
}

ClientAuthenticator *wampcra_authenticator_new(const char *auth_id, const char *secret,
                                               Dict *auth_extra)
{
    ClientAuthenticator *auth = calloc(1, sizeof(ClientAuthenticator));
    auth->auth_id = auth_id;
    auth->auth_extra = auth_extra;
    auth->auth_method = "wampcra";
    auth->auth_data = secret;
    auth->authenticate = authenticate;
    return auth;
}

char *base64_encode(const unsigned char *data, int length)
{
    if (data == NULL || length < 0)
        return NULL;

    if (sodium_init() < 0)
        return NULL;

    size_t b64_maxlen = sodium_base64_ENCODED_LEN((size_t)length, sodium_base64_VARIANT_ORIGINAL);
    char *b64 = malloc(b64_maxlen);
    if (!b64)
        return NULL;

    sodium_bin2base64(b64, b64_maxlen, data, (size_t)length, sodium_base64_VARIANT_ORIGINAL);

    return b64;
}

int hmac_sha256_sign(unsigned char *signature, const unsigned char *challenge, size_t challenge_len,
                     const unsigned char *secret, size_t secret_len)
{

    if (!signature || !challenge || !secret)
        return -1;

    crypto_auth_hmacsha256_state st;
    crypto_auth_hmacsha256_init(&st, secret, secret_len);

    crypto_auth_hmacsha256_update(&st, (const unsigned char *)challenge, challenge_len);

    crypto_auth_hmacsha256_final(&st, signature);

    return 0;
}

#ifdef HAVE_OPENSSL

char *create_derive_secret(const char *secret, const char *salt, int iterations, int key_length)
{
    if (iterations == 0)
        iterations = 1000;
    if (key_length == 0)
        key_length = 32;

    unsigned char derived_key[key_length];

    // Derive key using PBKDF2-HMAC-SHA256
    if (!PKCS5_PBKDF2_HMAC(secret, strlen(secret), (const unsigned char *)salt, strlen(salt),
                           iterations, EVP_sha256(), key_length, derived_key))
    {
        fprintf(stderr, "PBKDF2 failed\n");
        return NULL;
    }

    return base64_encode(derived_key, key_length);
}

#endif /* HAVE_OPENSSL */

void create_signature(unsigned char *signature, const char *secret, size_t secret_len,
                      const unsigned char *challenge, size_t challenge_len,
                      const unsigned char *salt, size_t salt_len, const unsigned int keylen,
                      const unsigned int iterations)
{

    const unsigned char *key = NULL;
    size_t key_len = 0;
    char *b64_encoded = NULL;

#ifdef HAVE_OPENSSL
    size_t len = strlen((const char *)salt);
    if (len > 0)
    {
        b64_encoded = create_derive_secret(secret, (const char *)salt, iterations, keylen);

        key = (const unsigned char *)b64_encoded;
        key_len = strlen(b64_encoded);
    }
    else
    {
        key = (const unsigned char *)secret;
        key_len = strlen(secret);
    }
#endif /* HAVE_OPENSSL */

    hmac_sha256_sign(signature, challenge, challenge_len, key, key_len);
    free(b64_encoded);
}

char *sign_signature(const char *secret, Dict *auth_extra)
{

    char *challenge = value_as_str(dict_get(auth_extra, "challenge"));
    char *salt = value_as_str(dict_get(auth_extra, "salt"));
    int64_t iterations = value_as_int(dict_get(auth_extra, "iterations"));
    int64_t keylen = value_as_int(dict_get(auth_extra, "keylen"));

    unsigned char signature[crypto_auth_hmacsha256_BYTES];
    create_signature(signature, secret, strlen(secret), (const unsigned char *)challenge,
                     strlen(challenge), (const unsigned char *)salt, strlen(salt),
                     (const unsigned int)keylen, (const unsigned int)iterations);

    char b64[sodium_base64_ENCODED_LEN(sizeof(signature), sodium_base64_VARIANT_ORIGINAL)];
    sodium_bin2base64(b64, sizeof(b64), signature, sizeof(signature),
                      sodium_base64_VARIANT_ORIGINAL);
    return base64_encode(signature, sizeof(signature));
}
