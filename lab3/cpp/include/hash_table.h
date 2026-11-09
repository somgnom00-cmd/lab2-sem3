#pragma once

#include "container.h"

#include <cstddef>
#include <string>
#include <vector>

class OpenAddressHashTable : public Container {
  public:
    explicit OpenAddressHashTable(std::size_t capacity = 17);

    void set(int key, int value);
    bool get(int key, int &value) const;
    bool erase(int key);
    bool contains(int key) const;

    std::size_t size() const noexcept override { return size_; }
    std::vector<std::string> values() const override;
    void clear() noexcept override;
    std::size_t capacity() const noexcept { return cells_.size(); }

  protected:
    void swapContents(Container &other) noexcept override;
    enum class State { empty, occupied, deleted };
    struct Cell {
        int key = 0;
        int value = 0;
        State state = State::empty;
    };

    virtual std::size_t probeStep(int key) const = 0;
    void appendSerialized(const std::string &value) override;

  private:
    static long long foldKey(int key) noexcept;
    static std::size_t nextPrime(std::size_t value);
    std::size_t startIndex(int key) const noexcept;
    std::size_t locate(int key) const noexcept;
    void rebuild(std::size_t capacity);

    std::vector<Cell> cells_;
    std::size_t size_ = 0;
    std::size_t deleted_ = 0;
};

// Вариант 1 ЛР2: линейное пробирование.
class LinearHashTable final : public OpenAddressHashTable {
  public:
    using OpenAddressHashTable::OpenAddressHashTable;
    std::string type() const override { return "LinearHashTable"; }

  protected:
    std::size_t probeStep(int) const override { return 1; }
};

// Вариант 1 ЛР2: двойное хеширование.
class DoubleHashTable final : public OpenAddressHashTable {
  public:
    using OpenAddressHashTable::OpenAddressHashTable;
    std::string type() const override { return "DoubleHashTable"; }

  protected:
    std::size_t probeStep(int key) const override;
};
