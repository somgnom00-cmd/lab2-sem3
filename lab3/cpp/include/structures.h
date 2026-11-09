#pragma once

#include "container.h"

#include <cstddef>
#include <string>
#include <ostream>
#include <vector>
#include <utility>

class Array final : public Container {
  public:
    Array() = default;
    Array(const Array &other);
    Array(Array &&other) noexcept;
    Array &operator=(const Array &other);
    Array &operator=(Array &&other) noexcept;
    ~Array() override;

    void pushBack(const std::string &value);
    void insert(std::size_t index, const std::string &value);
    void erase(std::size_t index);
    const std::string &at(std::size_t index) const;
    void set(std::size_t index, const std::string &value);
    const std::string &operator[](std::size_t index) const { return at(index); }
    friend std::ostream &operator<<(std::ostream &output, const Array &array);
    int find(const std::string &value) const noexcept;

    std::string type() const override { return "Array"; }
    std::size_t size() const noexcept override { return size_; }
    std::vector<std::string> values() const override;
    void clear() noexcept override;

  protected:
    void swapContents(Container &other) noexcept override { *this = std::move(static_cast<Array &>(other)); }
    void appendSerialized(const std::string &value) override { pushBack(value); }

  private:
    void reserve(std::size_t newCapacity);

    std::string *data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};

class SinglyList final : public Container {
  public:
    SinglyList() = default;
    SinglyList(const SinglyList &other);
    SinglyList(SinglyList &&other) noexcept;
    SinglyList &operator=(const SinglyList &other);
    SinglyList &operator=(SinglyList &&other) noexcept;
    ~SinglyList() override;

    void pushFront(const std::string &value);
    void pushBack(const std::string &value);
    void insert(std::size_t index, const std::string &value);
    void erase(std::size_t index);
    const std::string &at(std::size_t index) const;
    int find(const std::string &value) const noexcept;

    std::string type() const override { return "SinglyList"; }
    std::size_t size() const noexcept override { return size_; }
    std::vector<std::string> values() const override;
    void clear() noexcept override;

  protected:
    void swapContents(Container &other) noexcept override { swap(static_cast<SinglyList &>(other)); }
    void appendSerialized(const std::string &value) override { pushBack(value); }

  private:
    struct Node {
        std::string value;
        Node *next = nullptr;
    };

    Node *nodeAt(std::size_t index) const;
    void swap(SinglyList &other) noexcept;

    Node *head_ = nullptr;
    Node *tail_ = nullptr;
    std::size_t size_ = 0;
};

class DoublyList final : public Container {
  public:
    DoublyList() = default;
    DoublyList(const DoublyList &other);
    DoublyList(DoublyList &&other) noexcept;
    DoublyList &operator=(const DoublyList &other);
    DoublyList &operator=(DoublyList &&other) noexcept;
    ~DoublyList() override;

    void pushFront(const std::string &value);
    void pushBack(const std::string &value);
    void insert(std::size_t index, const std::string &value);
    void erase(std::size_t index);
    const std::string &at(std::size_t index) const;
    int find(const std::string &value) const noexcept;
    std::vector<std::string> reverseValues() const;

    std::string type() const override { return "DoublyList"; }
    std::size_t size() const noexcept override { return size_; }
    std::vector<std::string> values() const override;
    void clear() noexcept override;

  protected:
    void swapContents(Container &other) noexcept override { swap(static_cast<DoublyList &>(other)); }
    void appendSerialized(const std::string &value) override { pushBack(value); }

  private:
    struct Node {
        std::string value;
        Node *prev = nullptr;
        Node *next = nullptr;
    };

    Node *nodeAt(std::size_t index) const;
    void linkAfter(Node *before, const std::string &value);
    void swap(DoublyList &other) noexcept;

    Node *head_ = nullptr;
    Node *tail_ = nullptr;
    std::size_t size_ = 0;
};

class Stack final : public Container {
  public:
    Stack() = default;
    Stack(const Stack &other);
    Stack(Stack &&other) noexcept;
    Stack &operator=(const Stack &other);
    Stack &operator=(Stack &&other) noexcept;
    ~Stack() override;

    void push(const std::string &value);
    std::string pop();
    const std::string &top() const;

    std::string type() const override { return "Stack"; }
    std::size_t size() const noexcept override { return size_; }
    std::vector<std::string> values() const override;
    void clear() noexcept override;

  protected:
    void swapContents(Container &other) noexcept override { swap(static_cast<Stack &>(other)); }
    void appendSerialized(const std::string &value) override;

  private:
    struct Node {
        std::string value;
        Node *next = nullptr;
    };

    void swap(Stack &other) noexcept;
    Node *top_ = nullptr;
    std::size_t size_ = 0;
};

class Queue final : public Container {
  public:
    Queue() = default;
    Queue(const Queue &other);
    Queue(Queue &&other) noexcept;
    Queue &operator=(const Queue &other);
    Queue &operator=(Queue &&other) noexcept;
    ~Queue() override;

    void push(const std::string &value);
    std::string pop();
    const std::string &front() const;

    std::string type() const override { return "Queue"; }
    std::size_t size() const noexcept override { return size_; }
    std::vector<std::string> values() const override;
    void clear() noexcept override;

  protected:
    void swapContents(Container &other) noexcept override { swap(static_cast<Queue &>(other)); }
    void appendSerialized(const std::string &value) override { push(value); }

  private:
    struct Node {
        std::string value;
        Node *next = nullptr;
    };

    void swap(Queue &other) noexcept;
    Node *head_ = nullptr;
    Node *tail_ = nullptr;
    std::size_t size_ = 0;
};

// Вариант 3 ЛР1: полное бинарное дерево, заполнение по уровням.
class CompleteBinaryTree final : public Container {
  public:
    CompleteBinaryTree() = default;
    CompleteBinaryTree(const CompleteBinaryTree &other);
    CompleteBinaryTree(CompleteBinaryTree &&other) noexcept;
    CompleteBinaryTree &operator=(const CompleteBinaryTree &other);
    CompleteBinaryTree &operator=(CompleteBinaryTree &&other) noexcept;
    ~CompleteBinaryTree() override;

    void insert(const std::string &value);
    bool contains(const std::string &value) const;
    bool isComplete() const;

    std::string type() const override { return "CompleteBinaryTree"; }
    std::size_t size() const noexcept override { return size_; }
    std::vector<std::string> values() const override;
    void clear() noexcept override;

  protected:
    void swapContents(Container &other) noexcept override { swap(static_cast<CompleteBinaryTree &>(other)); }
    void appendSerialized(const std::string &value) override { insert(value); }

  private:
    struct Node {
        std::string value;
        Node *left = nullptr;
        Node *right = nullptr;
    };

    static void destroy(Node *node) noexcept;
    void swap(CompleteBinaryTree &other) noexcept;

    Node *root_ = nullptr;
    std::size_t size_ = 0;
};
