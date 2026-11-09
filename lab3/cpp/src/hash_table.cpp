#include "hash_table.h"

#include <charconv>
#include <cstdlib>
#include <limits>
#include <stdexcept>

OpenAddressHashTable::OpenAddressHashTable(std::size_t capacity)
    : cells_(nextPrime(capacity < 3 ? 3 : capacity)) {}

long long OpenAddressHashTable::foldKey(int key) noexcept {
    long long number = key;
    if (number < 0)
        number = -number;
    long long sum = 0;
    do {
        sum += number % 1000;
        number /= 1000;
    } while (number != 0);
    return sum;
}

std::size_t OpenAddressHashTable::nextPrime(std::size_t value) {
    const auto prime = [](std::size_t number) {
        if (number < 2)
            return false;
        for (std::size_t divisor = 2; divisor <= number / divisor; ++divisor) {
            if (number % divisor == 0)
                return false;
        }
        return true;
    };
    while (!prime(value)) {
        if (value == std::numeric_limits<std::size_t>::max()) {
            throw std::overflow_error("слишком большая хеш-таблица");
        }
        ++value;
    }
    return value;
}

std::size_t OpenAddressHashTable::startIndex(int key) const noexcept {
    return static_cast<std::size_t>(foldKey(key)) % cells_.size();
}

std::size_t OpenAddressHashTable::locate(int key) const noexcept {
    const std::size_t start = startIndex(key);
    const std::size_t step = probeStep(key);
    for (std::size_t attempt = 0; attempt < cells_.size(); ++attempt) {
        const std::size_t index = (start + attempt * step) % cells_.size();
        if (cells_[index].state == State::empty)
            return cells_.size();
        if (cells_[index].state == State::occupied && cells_[index].key == key)
            return index;
    }
    return cells_.size();
}

void OpenAddressHashTable::rebuild(std::size_t capacity) {
    const auto old = cells_;
    cells_.assign(nextPrime(capacity), Cell{});
    size_ = 0;
    deleted_ = 0;
    for (const auto &cell : old) {
        if (cell.state == State::occupied)
            set(cell.key, cell.value);
    }
}

void OpenAddressHashTable::set(int key, int value) {
    const std::size_t existing = locate(key);
    if (existing != cells_.size()) {
        cells_[existing].value = value;
        return;
    }

    if ((size_ + deleted_ + 1) * 10 > cells_.size() * 7) {
        const bool grow = (size_ + 1) * 10 > cells_.size() * 7;
        rebuild(grow ? cells_.size() * 2 + 1 : cells_.size());
    }

    const std::size_t start = startIndex(key);
    const std::size_t step = probeStep(key);
    std::size_t freeIndex = cells_.size();
    for (std::size_t attempt = 0; attempt < cells_.size(); ++attempt) {
        const std::size_t index = (start + attempt * step) % cells_.size();
        if (cells_[index].state == State::deleted && freeIndex == cells_.size())
            freeIndex = index;
        if (cells_[index].state == State::empty) {
            if (freeIndex == cells_.size())
                freeIndex = index;
            break;
        }
    }
    if (freeIndex == cells_.size())
        throw std::runtime_error("хеш-таблица заполнена");
    if (cells_[freeIndex].state == State::deleted)
        --deleted_;
    cells_[freeIndex] = Cell{key, value, State::occupied};
    ++size_;
}

bool OpenAddressHashTable::get(int key, int &value) const {
    const std::size_t index = locate(key);
    if (index == cells_.size())
        return false;
    value = cells_[index].value;
    return true;
}

bool OpenAddressHashTable::erase(int key) {
    const std::size_t index = locate(key);
    if (index == cells_.size())
        return false;
    cells_[index].state = State::deleted;
    --size_;
    ++deleted_;
    return true;
}

bool OpenAddressHashTable::contains(int key) const {
    int value = 0;
    return get(key, value);
}

std::vector<std::string> OpenAddressHashTable::values() const {
    std::vector<std::string> result;
    result.reserve(size_);
    for (const auto &cell : cells_) {
        if (cell.state == State::occupied) {
            result.push_back(std::to_string(cell.key) + "=" + std::to_string(cell.value));
        }
    }
    return result;
}

void OpenAddressHashTable::clear() noexcept {
    for (auto &cell : cells_)
        cell = Cell{};
    size_ = 0;
    deleted_ = 0;
}

void OpenAddressHashTable::appendSerialized(const std::string &value) {
    const std::size_t separator = value.find('=');
    if (separator == std::string::npos)
        throw std::runtime_error("повреждённая запись хеш-таблицы");
    int key = 0;
    int stored = 0;
    const auto parse = [](const char *first, const char *last, int &result) {
        const auto converted = std::from_chars(first, last, result);
        return converted.ec == std::errc{} && converted.ptr == last;
    };
    if (!parse(value.data(), value.data() + separator, key) ||
        !parse(value.data() + separator + 1, value.data() + value.size(), stored)) {
        throw std::runtime_error("повреждённая запись хеш-таблицы");
    }
    set(key, stored);
}

std::size_t DoubleHashTable::probeStep(int key) const {
    long long number = key;
    if (number < 0)
        number = -number;
    return 1 + static_cast<std::size_t>(number) % (capacity() - 1);
}


void OpenAddressHashTable::swapContents(Container &other) noexcept {
    auto &table = static_cast<OpenAddressHashTable &>(other);
    cells_.swap(table.cells_);
    std::swap(size_, table.size_);
    std::swap(deleted_, table.deleted_);
}
