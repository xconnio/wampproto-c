#include "wampproto/dict.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wampproto/value.h"

#include "uthash.h"

Dict* create_dict() {
    Dict* dict = malloc(sizeof(Dict));
    if (!dict) return NULL;

    dict->table = NULL;
    dict->count = 0;
    return dict;
}

void dict_insert(Dict* dict, const char* key, Value* value) {
    if (!dict || !key) return;

    Entry* entry = NULL;
    HASH_FIND_STR(dict->table, key, entry);

    if (entry) {
        // Replace existing value
        value_free(entry->value);
        entry->value = value;
        return;
    }

    // Create new entry
    entry = malloc(sizeof(*entry));
    if (!entry) return;

    entry->key = strdup(key);
    entry->value = value;

    HASH_ADD_KEYPTR(hh, dict->table, entry->key, strlen(entry->key), entry);
    dict->count++;
}

Value* dict_get(Dict* dict, const char* key) {
    if (!dict || !key) return NULL;

    Entry* entry = NULL;
    HASH_FIND_STR(dict->table, key, entry);
    return entry ? entry->value : NULL;
}

void dict_remove(Dict* dict, const char* key) {
    if (!dict || !key) return;

    Entry* entry = NULL;
    HASH_FIND_STR(dict->table, key, entry);
    if (entry) {
        HASH_DEL(dict->table, entry);
        free(entry->key);
        value_free(entry->value);
        free(entry);
        dict->count--;
    }
}

void dict_free(Dict* dict) {
    if (!dict || dict->count == 0) return;

    Entry *curr, *tmp;
    HASH_ITER(hh, dict->table, curr, tmp) {
        HASH_DEL(dict->table, curr);
        free(curr->key);
        value_free(curr->value);
        free(curr);
    }
    free(dict);
}
