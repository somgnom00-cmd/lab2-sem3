#pragma once
#include "doubly.h"
#include "array.h"
#include "hashtable.h"

struct LRUCache {
    int capacity;
    DoublyList order;
    HashTable index;
    Array keys;
    Array values;
    DoubleNode** nodes = nullptr;

    explicit LRUCache(int capacity);
    ~LRUCache();
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;
};

void cacheSet(LRUCache& cache, int key, int value);
int cacheGet(LRUCache& cache, int key);
