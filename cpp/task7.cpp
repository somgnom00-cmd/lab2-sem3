#include "task7.h"
#include <stdexcept>

LRUCache::LRUCache(int size) : capacity(size) {
    if (size < 0) throw std::runtime_error("ёмкость не может быть отрицательной");
    nodes = new DoubleNode*[size]{};
    index.doubleHash = true;
}

LRUCache::~LRUCache() {
    clearDoublyList(order);
    clearArray(keys);
    clearArray(values);
    delete[] nodes;
}

static void moveToTail(DoublyList& order, DoubleNode* node) {
    if (node == order.tail) return;
    // Отсоединяем известный узел без поиска по списку.
    if (node->prev) node->prev->next = node->next;
    else order.head = node->next;
    node->next->prev = node->prev;
    node->prev = order.tail;
    node->next = nullptr;
    order.tail->next = node;
    order.tail = node;
}

int cacheGet(LRUCache& cache, int key) {
    int slot;
    if (!hashGet(cache.index, key, slot)) return -1;
    moveToTail(cache.order, cache.nodes[slot]);
    return std::stoi(arrayGet(cache.values, slot));
}

void cacheSet(LRUCache& cache, int key, int value) {
    if (cache.capacity == 0) return;
    int slot;
    if (hashGet(cache.index, key, slot)) {
        arraySet(cache.values, slot, std::to_string(value));
        moveToTail(cache.order, cache.nodes[slot]);
        return;
    }
    if (cache.order.size == cache.capacity) {
        slot = std::stoi(cache.order.head->value);
        int oldKey = std::stoi(arrayGet(cache.keys, slot));
        // Переиспользуем узел самого давнего элемента.
        hashSet(cache.index, key, slot);
        hashDel(cache.index, oldKey);
        arraySet(cache.keys, slot, std::to_string(key));
        arraySet(cache.values, slot, std::to_string(value));
        moveToTail(cache.order, cache.nodes[slot]);
    } else {
        slot = cache.order.size;
        arrayPush(cache.keys, std::to_string(key));
        arrayPush(cache.values, std::to_string(value));
        doublyInsert(cache.order, cache.order.size, std::to_string(slot));
        cache.nodes[slot] = cache.order.tail;
        hashSet(cache.index, key, slot);
    }
}
