#pragma once
#include <vector>
#include <string>

struct HashCell {
    int key = 0;
    int value = 0;
    int state = 0; // 0 — пусто, 1 — занято, 2 — удалено.
};

struct HashTable {
    std::vector<HashCell> cells = std::vector<HashCell>(17);
    int size = 0;
    int deleted = 0;
    bool doubleHash = false;
    bool folding = true;
    bool autoResize = true;
    unsigned long long probes = 0;
};

long long foldKey(int key);
void hashInit(HashTable& table, int capacity, bool doubleHash, bool folding = true);
void hashSet(HashTable& table, int key, int value);
bool hashGet(HashTable& table, int key, int& value);
bool hashDel(HashTable& table, int key);
std::string compareHashing();
