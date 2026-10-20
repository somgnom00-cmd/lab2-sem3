#pragma once
#include "array.h"
#include "singly.h"
#include "doubly.h"
#include "tree.h"
#include "stack.h"
#include "queue.h"
#include "hash.h"
#include "cache.h"
#include <map>
#include <string>
#include <istream>
#include <ostream>

// Хранилище именованных структур.
struct Database {
    std::map<std::string, HashTable> sets;
    std::map<std::string, HashTable> hashes;
    std::map<std::string, LRUCache> caches;

    // Оставил отдельное хранение деревьев поиска.
    std::map<std::string, SearchTree> searchTrees;

    std::map<std::string, Array> arrays;
    std::map<std::string, SinglyList> lists;
    std::map<std::string, Stack> stacks;
    std::map<std::string, Queue> queues;
    std::map<std::string, DoublyList> doubles;
};

bool dbExists(Database& database, char type, const std::string& name);
void dbCreate(Database& database, char type, const std::string& name);
int dbSize(Database& database, char type, const std::string& name);

std::vector<std::string> dbValues(
    Database& database,
    char type,
    const std::string& name,
    bool reverse = false
);

void dbInsert(
    Database& database,
    char type,
    const std::string& name,
    int index,
    const std::string& value
);

void dbErase(
    Database& database,
    char type,
    const std::string& name,
    int index
);

std::string dbGet(
    Database& database,
    char type,
    const std::string& name,
    int index
);

void dbSet(
    Database& database,
    const std::string& name,
    int index,
    const std::string& value
);

int dbFind(
    Database& database,
    char type,
    const std::string& name,
    const std::string& value
);

void dbLoad(Database& database, const std::string& file);
void dbSave(Database& database, const std::string& file);
void clearDatabase(Database& database);

int number(const std::string& token);
void checkName(const std::string& name);
bool readToken(std::istream& input, std::string& token);
void writeValue(std::ostream& output, const std::string& value);