#include <memory.h>

#include "../include/hashmap.h"
#include "../include/utils.h"

void init_hashmap(HashMap* map, size_t data_size, size_t num_buckets) {
    map->used_buckets = 0;
    map->allocated_buckets = num_buckets;
    map->buckets = NULL;

    map->data_size = data_size;

    if (data_size > 0 && num_buckets > 0) {
        map->buckets = malloc(sizeof(KeyValue) * num_buckets);
        for (unsigned i = 0; i < num_buckets; i++) {
            map->buckets[i].key = NULL;
            map->buckets[i].value = malloc(data_size);
        }
    }
}

void insert_hashmap(HashMap* map, const char* key, void* value) {
    float load_factor = (float)map->used_buckets / (float)map->allocated_buckets;
    if (load_factor >= 0.8f) {
        KeyValue** old_buckets = &map->buckets;
        size_t new_size = map->allocated_buckets * 2;
        
        map->buckets = malloc(sizeof(KeyValue) * new_size);
        if(old_buckets != NULL) {
            memcpy(map->buckets, *old_buckets, sizeof(KeyValue) * map->allocated_buckets);
        }

        for (unsigned i = map->used_buckets; i < new_size; i++) {
            map->buckets[i].key = NULL;
            map->buckets[i].value = malloc(map->data_size);
        }

        map->allocated_buckets = new_size;
    }

    unsigned hash = _djb2_hash(key, map->allocated_buckets);
    while (map->buckets[hash].key != NULL) {
        hash  = (hash * 2) % map->allocated_buckets;
    }
    map->buckets[hash].key = key;
    map->buckets[hash].value = value;
}

KeyValue* get_hashmap(HashMap* map, const char* key) {
    unsigned hash = _djb2_hash(key, map->allocated_buckets);
    while (map->buckets[hash].key != NULL && strncmp(map->buckets[hash].key, key, 255) != 0) {
        hash = (hash * 2) % map->allocated_buckets;
    }
    KeyValue* kv = &map->buckets[hash];

    if (strncmp(kv->key, key, 255) != 0) {
        return NULL;
    }

    return kv;
}

unsigned _djb2_hash(const char* msg, unsigned max_size) {
    unsigned address = 5381;
    for (unsigned i = 0; msg[i] != '\0'; i++) {
        address = ((address << 5) + address) + msg[i];
    }

    return address % max_size;
}
