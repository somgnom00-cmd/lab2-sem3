#include "doubly.h"
#include <iostream>
#include <stdexcept>

DoubleNode* doublyNodeAt(const DoublyList& list, int index) {
    if (!(index >= 0 && index < list.size)) throw std::runtime_error("индекс вне диапазона");
    if (index == list.size - 1) return list.tail;
    DoubleNode* node = list.head;
    for (int i = 0; i < index; ++i) node = node->next;
    return node;
}

// Возвращаем адрес найденного узла или nullptr.
DoubleNode* doublyFindNode(const DoublyList& list, const std::string& value) {
    DoubleNode* node = list.head;
    while (node) {
        if (node->value == value) return node;
        node = node->next;
    }
    return nullptr;
}

// Присоединяем узел после before; nullptr означает начало списка.
void doublyLink(DoublyList& list, DoubleNode* before, const std::string& value) {
    DoubleNode* added = new DoubleNode{value};
    added->next = before ? before->next : list.head;
    added->prev = before;
    if (added->next) added->next->prev = added;
    if (before) before->next = added;
    else list.head = added;
    if (!added->next) list.tail = added;
    ++list.size;
}

void doublyRemove(DoublyList& list, DoubleNode* node) {
    if (!(node != nullptr)) throw std::runtime_error("нет элемента в выбранной позиции");
    DoubleNode* before = node->prev;
    if (before) before->next = node->next;
    else list.head = node->next;
    if (node->next) node->next->prev = before;
    if (node == list.tail) list.tail = before;
    delete node;
    --list.size;
}

void doublyInsert(DoublyList& list, int index, const std::string& value) {
    if (!(index >= 0 && index <= list.size)) throw std::runtime_error("индекс вне диапазона");
    DoubleNode* before = nullptr;
    if (index == list.size) before = list.tail;
    else if (index > 0) before = doublyNodeAt(list, index - 1);
    doublyLink(list, before, value);
}

void doublyErase(DoublyList& list, int index) {
    doublyRemove(list, doublyNodeAt(list, index));
}

std::string doublyGet(const DoublyList& list, int index) {
    return doublyNodeAt(list, index)->value;
}

int doublyFind(const DoublyList& list, const std::string& value) {
    int index = 0;
    for (DoubleNode* node = list.head; node; node = node->next) {
        if (node->value == value) return index;
        ++index;
    }
    return -1;
}

void doublyPush(DoublyList& list, const std::string& position,
                const std::string& target, const std::string& value) {
    if (position == "HEAD") {
        doublyLink(list, nullptr, value);
        return;
    }
    if (position == "TAIL") {
        doublyLink(list, list.tail, value);
        return;
    }
    if (!(position == "BEFORE" || position == "AFTER")) throw std::runtime_error("неизвестный способ; HELP");
    DoubleNode* found = doublyFindNode(list, target);
    if (!(found != nullptr)) throw std::runtime_error("значение не найдено");
    if (position == "BEFORE") {
        doublyLink(list, found->prev, value);
    } else {
        doublyLink(list, found, value);
    }
}

void doublyDelete(DoublyList& list, const std::string& position,
                  const std::string& value) {
    if (!(list.head != nullptr)) throw std::runtime_error("список пуст");
    DoubleNode* node = nullptr;
    if (position == "HEAD") node = list.head;
    else if (position == "TAIL") node = list.tail;
    else {
        if (!(position == "VALUE" || position == "BEFORE" || position == "AFTER")) throw std::runtime_error("неизвестный способ; HELP");
        node = doublyFindNode(list, value);
        if (!(node != nullptr)) throw std::runtime_error("значение не найдено");
        if (position == "AFTER") node = node->next;
        if (position == "BEFORE") node = node->prev;
    }
    doublyRemove(list, node);
}

std::vector<std::string> doublyValues(const DoublyList& list, bool reverse) {
    std::vector<std::string> values;
    DoubleNode* node = reverse ? list.tail : list.head;
    while (node) {
        values.push_back(node->value);
        node = reverse ? node->prev : node->next;
    }
    return values;
}

void clearDoublyList(DoublyList& list) {
    while (list.head) doublyRemove(list, list.head);
}
