#pragma once
#include "hash.h"
#include "doubly.h"

// Добавил LRU: таблица хранит номер записи, список — порядок обращений.
struct LRUCache {
    int capacity = 0;
    HashTable index;
    DoublyList order;
    Array keys, values;
    DoubleNode** nodes = nullptr;
};
void cacheInit(LRUCache& cache, int capacity);
void cacheSet(LRUCache& cache, int key, int value);
bool cacheGet(LRUCache& cache, int key, int& value);
void clearCache(LRUCache& cache);
