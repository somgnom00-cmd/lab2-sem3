// Добавил операции LRU на своей таблице и двусвязном списке.
#include "cache.h"
#include <stdexcept>
#include <limits>

void cacheInit(LRUCache& cache, int capacity) {
    if (capacity < 0 || capacity > (std::numeric_limits<int>::max() - 1) / 2)
        throw std::runtime_error("неверная ёмкость кэша");
    cache.capacity = capacity;
    hashInit(cache.index, capacity > 0 ? capacity * 2 + 1 : 3, true, false);
    cache.nodes = new DoubleNode*[capacity]{};
}

bool cacheGet(LRUCache& cache, int key, int& value) {
    int slot;
    if (!hashGet(cache.index, key, slot)) return false;
    value = std::stoi(cache.values.data[slot]);
    doublyMoveToTail(cache.order, cache.nodes[slot]);
    return true;
}

void cacheSet(LRUCache& cache, int key, int value) {
    if (cache.capacity == 0) return;
    int slot;
    if (hashGet(cache.index, key, slot)) {
        arraySet(cache.values, slot, std::to_string(value));
        doublyMoveToTail(cache.order, cache.nodes[slot]);
        return;
    }
    if (cache.order.size == cache.capacity) {
        // Повторно использую запись самого давнего элемента.
        slot = std::stoi(cache.order.head->value);
        hashErase(cache.index, std::stoi(cache.keys.data[slot]));
        arraySet(cache.keys, slot, std::to_string(key));
        arraySet(cache.values, slot, std::to_string(value));
        doublyMoveToTail(cache.order, cache.nodes[slot]);
    } else {
        slot = cache.order.size;
        arrayInsert(cache.keys, slot, std::to_string(key));
        arrayInsert(cache.values, slot, std::to_string(value));
        doublyInsert(cache.order, cache.order.size, std::to_string(slot));
        cache.nodes[slot] = cache.order.tail;
    }
    hashSet(cache.index, key, slot);
}

void clearCache(LRUCache& cache) {
    clearHash(cache.index);
    clearDoublyList(cache.order);
    clearArray(cache.keys);
    clearArray(cache.values);
    delete[] cache.nodes;
    cache.nodes = nullptr;
    cache.capacity = 0;
}
