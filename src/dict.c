#include "wampproto/dict.h"
#include "wampproto/value.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 1024

uint64_t fnv1a(const char *key)
{
    uint64_t hash = 1469598103934665603ULL;
    while (*key)
    {
        hash ^= (unsigned char)(*key++);
        hash *= 1099511628211ULL;
    }
    return hash;
}

Dict *create_dict(size_t size)
{
    Dict *dict = malloc(sizeof(Dict));
    dict->size = size;
    dict->count = 0;
    dict->buckets = calloc(dict->size, sizeof(Entry *));
    return dict;
}

void resize_dict(Dict *dict, size_t new_size)
{
    Entry **new_buckets = calloc(new_size, sizeof(Entry *));

    for (size_t i = 0; i < dict->size; i++)
    {
        Entry *curr = dict->buckets[i];
        while (curr)
        {
            Entry *next = curr->next;

            // Recompute index in new table
            uint64_t hash = fnv1a(curr->key);
            size_t new_index = hash % new_size;

            // Insert at head of new bucket
            curr->next = new_buckets[new_index];
            new_buckets[new_index] = curr;

            curr = next;
        }
    }
    free(dict->buckets);
    dict->buckets = new_buckets;
    dict->size = new_size;
}

void dict_insert(Dict *dict, const char *key, Value *value)
{
    uint64_t hash = fnv1a(key);
    size_t index = hash % dict->size;

    Entry *curr = dict->buckets[index];
    while (curr)
    {
        if (strcmp(curr->key, key) == 0)
        {
            value_free(curr->value);
            curr->value = value;
            return;
        }
        curr = curr->next;
    }

    Entry *new_entry = malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = dict->buckets[index];
    dict->buckets[index] = new_entry;

    dict->count++;

    if ((double)dict->count / dict->size > 0.7)
    {
        resize_dict(dict, dict->size * 2);
    }
}

Value *dict_get(Dict *dict, const char *key)
{
    uint64_t hash = fnv1a(key);
    size_t index = hash % dict->size;

    Entry *curr = dict->buckets[index];
    while (curr)
    {
        if (strcmp(curr->key, key) == 0)
        {
            return curr->value;
        }
        curr = curr->next;
    }
    return NULL; // not found
}

void dict_free(Dict *dict)
{
    for (size_t i = 0; i < dict->size; i++)
    {
        Entry *curr = dict->buckets[i];
        while (curr)
        {
            Entry *tmp = curr;
            curr = curr->next;
            free(tmp->key);
            free(tmp);
        }
    }
    free(dict->buckets);
    free(dict);
}
