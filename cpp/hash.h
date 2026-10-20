#pragma once
#include "array.h"

// Добавил хеш-таблицу на собственных массивах: состояния 0, 1, 2 — пусто, занято, удалено.
struct HashTable {
    Array keys, values, states;
    int size = 0;
    int deleted = 0;
    bool doubleHash = false;
    bool folding = false;
    unsigned long long probes = 0;
};

int hashPrime(int minimum);
long long hashFold(int key);
void hashInit(HashTable& table, int capacity = 17, bool doubleHash = false, bool folding = false);
int hashFind(HashTable& table, int key);
bool hashGet(HashTable& table, int key, int& value);
void hashSet(HashTable& table, int key, int value, bool grow = true);
bool hashErase(HashTable& table, int key);
void hashRebuild(HashTable& table, int capacity);
void clearHash(HashTable& table);
