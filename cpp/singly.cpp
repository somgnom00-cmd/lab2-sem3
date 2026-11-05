#include "singly.h"
#include <iostream>
#include <stdexcept>

SingleNode* singlyNodeAt(const SinglyList& list, int index) {
    if (!(index >= 0 && index < list.size)) throw std::runtime_error("индекс вне диапазона");
    SingleNode* node = list.head;
    for (int i = 0; i < index; ++i) node = node->next;
    return node;
}

// Возвращаем адрес найденного узла или nullptr.
SingleNode* singlyFindNode(const SinglyList& list, const std::string& value) {
    SingleNode* node = list.head;
    while (node) {
        if (node->value == value) return node;
        node = node->next;
    }
    return nullptr;
}

SingleNode* singlyPrevious(const SinglyList& list, SingleNode* node) {
    SingleNode* before = nullptr;
    for (SingleNode* current = list.head; current != node; current = current->next)
        before = current;
    return before;
}

// Присоединяем узел после before; nullptr означает начало списка.
void singlyLink(SinglyList& list, SingleNode* before, const std::string& value) {
    SingleNode* added = new SingleNode{value};
    added->next = before ? before->next : list.head;
    if (before) before->next = added;
    else list.head = added;
    if (!added->next) list.tail = added;
    ++list.size;
}

void singlyRemove(SinglyList& list, SingleNode* node) {
    if (!(node != nullptr)) throw std::runtime_error("нет элемента в выбранной позиции");
    SingleNode* before = singlyPrevious(list, node);
    if (before) before->next = node->next;
    else list.head = node->next;
    if (node == list.tail) list.tail = before;
    delete node;
    --list.size;
}

void singlyInsert(SinglyList& list, int index, const std::string& value) {
    if (!(index >= 0 && index <= list.size)) throw std::runtime_error("индекс вне диапазона");
    SingleNode* before = nullptr;
    if (index == list.size) before = list.tail;
    else if (index > 0) before = singlyNodeAt(list, index - 1);
    singlyLink(list, before, value);
}

void singlyErase(SinglyList& list, int index) {
    singlyRemove(list, singlyNodeAt(list, index));
}

std::string singlyGet(const SinglyList& list, int index) {
    return singlyNodeAt(list, index)->value;
}

int singlyFind(const SinglyList& list, const std::string& value) {
    int index = 0;
    for (SingleNode* node = list.head; node; node = node->next) {
        if (node->value == value) return index;
        ++index;
    }
    return -1;
}

void singlyPush(SinglyList& list, const std::string& position,
                const std::string& target, const std::string& value) {
    if (position == "HEAD") {
        singlyLink(list, nullptr, value);
        return;
    }
    if (position == "TAIL") {
        singlyLink(list, list.tail, value);
        return;
    }
    if (!(position == "BEFORE" || position == "AFTER")) throw std::runtime_error("неизвестный способ; HELP");
    SingleNode* found = singlyFindNode(list, target);
    if (!(found != nullptr)) throw std::runtime_error("значение не найдено");
    if (position == "BEFORE") {
        SingleNode* before = singlyPrevious(list, found);
        singlyLink(list, before, value);
    } else {
        singlyLink(list, found, value);
    }
}

void singlyDelete(SinglyList& list, const std::string& position,
                  const std::string& value) {
    if (!(list.head != nullptr)) throw std::runtime_error("список пуст");
    SingleNode* node = nullptr;
    if (position == "HEAD") node = list.head;
    else if (position == "TAIL") node = list.tail;
    else {
        if (!(position == "VALUE" || position == "BEFORE" || position == "AFTER")) throw std::runtime_error("неизвестный способ; HELP");
        node = singlyFindNode(list, value);
        if (!(node != nullptr)) throw std::runtime_error("значение не найдено");
        if (position == "AFTER") node = node->next;
        if (position == "BEFORE") node = singlyPrevious(list, node);
    }
    singlyRemove(list, node);
}

std::vector<std::string> singlyValues(const SinglyList& list, bool reverse) {
    std::vector<std::string> values;
    for (SingleNode* node = list.head; node; node = node->next)
        values.push_back(node->value);
    if (reverse)
        for (int i = 0; i < list.size / 2; ++i)
            std::swap(values[i], values[list.size - 1 - i]);
    return values;
}

void clearSinglyList(SinglyList& list) {
    while (list.head) singlyRemove(list, list.head);
}
