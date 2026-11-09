#include "database.h"

#include "hash_table.h"
#include "structures.h"

#include <stdexcept>

std::unique_ptr<Container> makeContainer(const std::string &type) {
    if (type == "Array")
        return std::make_unique<Array>();
    if (type == "SinglyList")
        return std::make_unique<SinglyList>();
    if (type == "DoublyList")
        return std::make_unique<DoublyList>();
    if (type == "Stack")
        return std::make_unique<Stack>();
    if (type == "Queue")
        return std::make_unique<Queue>();
    if (type == "CompleteBinaryTree")
        return std::make_unique<CompleteBinaryTree>();
    if (type == "LinearHashTable")
        return std::make_unique<LinearHashTable>();
    if (type == "DoubleHashTable")
        return std::make_unique<DoubleHashTable>();
    throw std::invalid_argument("неизвестный тип контейнера: " + type);
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): публичный порядок create(name, type).
Container &Database::create(const std::string &name, const std::string &type) {
    if (name.empty())
        throw std::invalid_argument("имя контейнера не может быть пустым");
    if (items_.find(name) != items_.end())
        throw std::invalid_argument("контейнер уже существует");
    auto container = makeContainer(type);
    Container &reference = *container;
    items_.emplace(name, std::move(container));
    return reference;
}

Container &Database::get(const std::string &name) {
    const auto found = items_.find(name);
    if (found == items_.end())
        throw std::out_of_range("контейнер не найден");
    return *found->second;
}

const Container &Database::get(const std::string &name) const {
    const auto found = items_.find(name);
    if (found == items_.end())
        throw std::out_of_range("контейнер не найден");
    return *found->second;
}

bool Database::erase(const std::string &name) noexcept { return items_.erase(name) != 0; }
