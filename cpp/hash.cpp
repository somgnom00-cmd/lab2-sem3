// Добавил операции хеш-таблицы с двумя способами пробирования.
#include "hash.h"
#include <stdexcept>
#include <limits>
#include <string>

static bool isPrime(int value) {
    if (value < 2) return false;
    for (int divisor = 2; 1LL * divisor * divisor <= value; ++divisor)
        if (value % divisor == 0) return false;
    return true;
}

int hashPrime(int minimum) {
    int value = minimum < 3 ? 3 : minimum;
    while (!isPrime(value)) {
        if (value == std::numeric_limits<int>::max())
            throw std::runtime_error("слишком большой размер таблицы");
        ++value;
    }
    return value;
}

// Добавил свёртку: десятичные группы по три цифры складываются.
long long hashFold(int key) {
    long long remaining = key;
    if (remaining < 0) remaining = -remaining;
    long long result = 0;
    do {
        result += remaining % 1000;
        remaining /= 1000;
    } while (remaining);
    return result;
}

static int positiveMod(long long value, int modulus) {
    int result = static_cast<int>(value % modulus);
    return result < 0 ? result + modulus : result;
}

void hashInit(HashTable& table, int capacity, bool doubleHash, bool folding) {
    if (table.states.size) throw std::runtime_error("таблица уже инициализирована");
    if (capacity < 1) throw std::runtime_error("размер должен быть положительным");
    capacity = hashPrime(capacity);
    table.doubleHash = doubleHash;
    table.folding = folding;
    for (int i = 0; i < capacity; ++i) {
        arrayInsert(table.keys, i, "0");
        arrayInsert(table.values, i, "0");
        arrayInsert(table.states, i, "0");
    }
}

static int hashStart(const HashTable& table, int key) {
    return positiveMod(table.folding ? hashFold(key) : key, table.states.size);
}

static int hashStep(const HashTable& table, int key) {
    // Простой размер и шаг от 1 до M-1 обеспечивают обход всех ячеек.
    return table.doubleHash ? 1 + positiveMod(key, table.states.size - 1) : 1;
}

int hashFind(HashTable& table, int key) {
    if (!table.states.size) return -1;
    int index = hashStart(table, key);
    int step = hashStep(table, key);
    std::string wanted = std::to_string(key);
    for (int attempt = 0; attempt < table.states.size; ++attempt) {
        ++table.probes;
        const std::string& state = table.states.data[index];
        if (state == "0") return -1;
        if (state == "1" && table.keys.data[index] == wanted) return index;
        index = static_cast<int>((1LL * index + step) % table.states.size);
    }
    return -1;
}

bool hashGet(HashTable& table, int key, int& value) {
    int index = hashFind(table, key);
    if (index < 0) return false;
    value = std::stoi(table.values.data[index]);
    return true;
}

void clearHash(HashTable& table) {
    clearArray(table.keys);
    clearArray(table.values);
    clearArray(table.states);
    table.size = table.deleted = 0;
    table.probes = 0;
}

// При изменении размера заново вычисляю позиции всех занятых ключей.
void hashRebuild(HashTable& table, int capacity) {
    HashTable expanded;
    hashInit(expanded, capacity, table.doubleHash, table.folding);
    try {
        for (int i = 0; i < table.states.size; ++i)
            if (table.states.data[i] == "1")
                hashSet(expanded, std::stoi(table.keys.data[i]), std::stoi(table.values.data[i]), false);
    } catch (...) { clearHash(expanded); throw; }
    unsigned long long previous = table.probes;
    clearHash(table);
    table = expanded;
    // Передал владение массивами; временная структура больше ими не пользуется.
    expanded = HashTable{};
    table.probes += previous;
}

void hashSet(HashTable& table, int key, int value, bool grow) {
    if (!table.states.size) hashInit(table);
    int existing = hashFind(table, key);
    if (existing >= 0) {
        arraySet(table.values, existing, std::to_string(value));
        return;
    }
    if (grow && 10LL * (table.size + table.deleted + 1) > 7LL * table.states.size) {
        int capacity = table.states.size;
        if (10LL * (table.size + 1) > 7LL * capacity) {
            if (capacity > (std::numeric_limits<int>::max() - 1) / 2)
                throw std::runtime_error("слишком большая таблица");
            capacity = capacity * 2 + 1;
        }
        hashRebuild(table, capacity);
    }
    int index = hashStart(table, key), step = hashStep(table, key), removed = -1;
    for (int attempt = 0; attempt < table.states.size; ++attempt) {
        ++table.probes;
        const std::string& state = table.states.data[index];
        if (state == "2" && removed < 0) removed = index;
        if (state == "0") {
            if (removed < 0) removed = index;
            break;
        }
        index = static_cast<int>((1LL * index + step) % table.states.size);
    }
    if (removed < 0) throw std::runtime_error("хеш-таблица заполнена");
    if (table.states.data[removed] == "2") --table.deleted;
    arraySet(table.keys, removed, std::to_string(key));
    arraySet(table.values, removed, std::to_string(value));
    arraySet(table.states, removed, "1");
    ++table.size;
}

bool hashErase(HashTable& table, int key) {
    int index = hashFind(table, key);
    if (index < 0) return false;
    // Пометка удаления сохраняет цепочку пробирования для соседних ключей.
    arraySet(table.states, index, "2");
    --table.size;
    ++table.deleted;
    return true;
}
