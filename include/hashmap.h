#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#include <stdlib.h>

typedef struct {
    const char* key;
    void* value;
} KeyValue;

typedef struct {
    KeyValue* buckets;
    size_t allocated_buckets;
    size_t used_buckets;

    size_t data_size;
} HashMap;

#define get_hashmap_value(map, key, type) (type*)get_hashmap(map, key)->value

void init_hashmap(HashMap* map, size_t data_size, size_t num_buckets);
void insert_hashmap(HashMap* map, const char* key, void* value);
KeyValue* get_hashmap(HashMap* map, const char* key);

unsigned _djb2_hash(const char* msg, unsigned max_size);

#endif // _HASHMAP_H