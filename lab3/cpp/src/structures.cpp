#include "structures.h"

#include <algorithm>
#include <memory>
#include <queue>
#include <stdexcept>
#include <utility>

Array::Array(const Array &other) : Array() {
    reserve(other.size_);
    for (std::size_t i = 0; i < other.size_; ++i)
        data_[i] = other.data_[i];
    size_ = other.size_;
}

Array::Array(Array &&other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = other.capacity_ = 0;
}

Array &Array::operator=(const Array &other) {
    if (this == &other)
        return *this;
    Array copy(other);
    std::swap(data_, copy.data_);
    std::swap(size_, copy.size_);
    std::swap(capacity_, copy.capacity_);
    return *this;
}

Array &Array::operator=(Array &&other) noexcept {
    if (this == &other)
        return *this;
    clear();
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    other.data_ = nullptr;
    other.size_ = other.capacity_ = 0;
    return *this;
}

Array::~Array() { clear(); }

void Array::reserve(std::size_t newCapacity) {
    if (newCapacity <= capacity_)
        return;
    auto *replacement = new std::string[newCapacity];
    for (std::size_t i = 0; i < size_; ++i)
        replacement[i] = std::move(data_[i]);
    delete[] data_;
    data_ = replacement;
    capacity_ = newCapacity;
}

void Array::pushBack(const std::string &value) {
    insert(size_, value);
}

void Array::insert(std::size_t index, const std::string &value) {
    if (index > size_)
        throw std::out_of_range("индекс массива вне диапазона");
    std::string saved(value);
    if (size_ == capacity_)
        reserve(capacity_ == 0 ? 4 : capacity_ * 2);
    for (std::size_t i = size_; i > index; --i)
        data_[i] = std::move(data_[i - 1]);
    data_[index] = std::move(saved);
    ++size_;
}

void Array::erase(std::size_t index) {
    if (index >= size_)
        throw std::out_of_range("индекс массива вне диапазона");
    for (std::size_t i = index; i + 1 < size_; ++i)
        data_[i] = std::move(data_[i + 1]);
    --size_;
    data_[size_].clear();
}

const std::string &Array::at(std::size_t index) const {
    if (index >= size_)
        throw std::out_of_range("индекс массива вне диапазона");
    return data_[index];
}

void Array::set(std::size_t index, const std::string &value) {
    if (index >= size_)
        throw std::out_of_range("индекс массива вне диапазона");
    data_[index] = value;
}

int Array::find(const std::string &value) const noexcept {
    for (std::size_t i = 0; i < size_; ++i) {
        if (data_[i] == value)
            return static_cast<int>(i);
    }
    return -1;
}

std::vector<std::string> Array::values() const {
    std::vector<std::string> result;
    result.reserve(size_);
    for (std::size_t i = 0; i < size_; ++i)
        result.push_back(data_[i]);
    return result;
}

std::ostream &operator<<(std::ostream &output, const Array &array) {
    output << '[';
    for (std::size_t i = 0; i < array.size(); ++i) {
        if (i != 0)
            output << ", ";
        output << array[i];
    }
    return output << ']';
}

void Array::clear() noexcept {
    delete[] data_;
    data_ = nullptr;
    size_ = capacity_ = 0;
}

SinglyList::SinglyList(const SinglyList &other) : SinglyList() {
    for (const auto &value : other.values())
        pushBack(value);
}

SinglyList::SinglyList(SinglyList &&other) noexcept { swap(other); }

SinglyList &SinglyList::operator=(const SinglyList &other) {
    if (this != &other) {
        SinglyList copy(other);
        swap(copy);
    }
    return *this;
}

SinglyList &SinglyList::operator=(SinglyList &&other) noexcept {
    if (this != &other) {
        clear();
        swap(other);
    }
    return *this;
}

SinglyList::~SinglyList() { clear(); }

void SinglyList::swap(SinglyList &other) noexcept {
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    std::swap(size_, other.size_);
}

SinglyList::Node *SinglyList::nodeAt(std::size_t index) const {
    if (index >= size_)
        throw std::out_of_range("индекс списка вне диапазона");
    Node *node = head_;
    for (std::size_t i = 0; i < index; ++i)
        node = node->next;
    return node;
}

void SinglyList::pushFront(const std::string &value) {
    Node *added = new Node{value, head_};
    head_ = added;
    if (!tail_)
        tail_ = added;
    ++size_;
}

void SinglyList::pushBack(const std::string &value) {
    Node *added = new Node{value};
    if (tail_)
        tail_->next = added;
    else
        head_ = added;
    tail_ = added;
    ++size_;
}

void SinglyList::insert(std::size_t index, const std::string &value) {
    if (index > size_)
        throw std::out_of_range("индекс списка вне диапазона");
    if (index == 0)
        return pushFront(value);
    if (index == size_)
        return pushBack(value);
    Node *before = nodeAt(index - 1);
    before->next = new Node{value, before->next};
    ++size_;
}

void SinglyList::erase(std::size_t index) {
    if (index >= size_)
        throw std::out_of_range("индекс списка вне диапазона");
    Node *removed = nullptr;
    if (index == 0) {
        removed = head_;
        head_ = head_->next;
    } else {
        Node *before = nodeAt(index - 1);
        removed = before->next;
        before->next = removed->next;
        if (removed == tail_)
            tail_ = before;
    }
    delete removed;
    --size_;
    if (size_ == 0)
        tail_ = nullptr;
}

const std::string &SinglyList::at(std::size_t index) const { return nodeAt(index)->value; }

int SinglyList::find(const std::string &value) const noexcept {
    std::size_t index = 0;
    for (Node *node = head_; node; node = node->next, ++index) {
        if (node->value == value)
            return static_cast<int>(index);
    }
    return -1;
}

std::vector<std::string> SinglyList::values() const {
    std::vector<std::string> result;
    result.reserve(size_);
    for (Node *node = head_; node; node = node->next)
        result.push_back(node->value);
    return result;
}

void SinglyList::clear() noexcept {
    while (head_) {
        Node *removed = head_;
        head_ = head_->next;
        delete removed;
    }
    tail_ = nullptr;
    size_ = 0;
}

DoublyList::DoublyList(const DoublyList &other) : DoublyList() {
    for (const auto &value : other.values())
        pushBack(value);
}

DoublyList::DoublyList(DoublyList &&other) noexcept { swap(other); }

DoublyList &DoublyList::operator=(const DoublyList &other) {
    if (this != &other) {
        DoublyList copy(other);
        swap(copy);
    }
    return *this;
}

DoublyList &DoublyList::operator=(DoublyList &&other) noexcept {
    if (this != &other) {
        clear();
        swap(other);
    }
    return *this;
}

DoublyList::~DoublyList() { clear(); }

void DoublyList::swap(DoublyList &other) noexcept {
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    std::swap(size_, other.size_);
}

DoublyList::Node *DoublyList::nodeAt(std::size_t index) const {
    if (index >= size_)
        throw std::out_of_range("индекс списка вне диапазона");
    if (index < size_ / 2) {
        Node *node = head_;
        for (std::size_t i = 0; i < index; ++i)
            node = node->next;
        return node;
    }
    Node *node = tail_;
    for (std::size_t i = size_ - 1; i > index; --i)
        node = node->prev;
    return node;
}

void DoublyList::linkAfter(Node *before, const std::string &value) {
    auto owner = std::make_unique<Node>();
    owner->value = value;
    owner->prev = before;
    owner->next = before != nullptr ? before->next : head_;
    Node *added = owner.release();
    if (added->next != nullptr)
        added->next->prev = added;
    if (before)
        before->next = added;
    else
        head_ = added;
    if (!added->next)
        tail_ = added;
    ++size_;
}

void DoublyList::pushFront(const std::string &value) { linkAfter(nullptr, value); }

void DoublyList::pushBack(const std::string &value) { linkAfter(tail_, value); }

void DoublyList::insert(std::size_t index, const std::string &value) {
    if (index > size_)
        throw std::out_of_range("индекс списка вне диапазона");
    linkAfter(index == 0 ? nullptr : nodeAt(index - 1), value);
}

void DoublyList::erase(std::size_t index) {
    Node *removed = nodeAt(index);
    if (removed->prev)
        removed->prev->next = removed->next;
    else
        head_ = removed->next;
    if (removed->next)
        removed->next->prev = removed->prev;
    else
        tail_ = removed->prev;
    delete removed;
    --size_;
}

const std::string &DoublyList::at(std::size_t index) const { return nodeAt(index)->value; }

int DoublyList::find(const std::string &value) const noexcept {
    std::size_t index = 0;
    for (Node *node = head_; node; node = node->next, ++index) {
        if (node->value == value)
            return static_cast<int>(index);
    }
    return -1;
}

std::vector<std::string> DoublyList::values() const {
    std::vector<std::string> result;
    result.reserve(size_);
    for (Node *node = head_; node; node = node->next)
        result.push_back(node->value);
    return result;
}

std::vector<std::string> DoublyList::reverseValues() const {
    std::vector<std::string> result;
    result.reserve(size_);
    for (Node *node = tail_; node; node = node->prev)
        result.push_back(node->value);
    return result;
}

void DoublyList::clear() noexcept {
    while (head_) {
        Node *removed = head_;
        head_ = head_->next;
        delete removed;
    }
    tail_ = nullptr;
    size_ = 0;
}

Stack::Stack(const Stack &other) : Stack() {
    auto stored = other.values();
    for (auto it = stored.rbegin(); it != stored.rend(); ++it)
        push(*it);
}

Stack::Stack(Stack &&other) noexcept { swap(other); }

Stack &Stack::operator=(const Stack &other) {
    if (this != &other) {
        Stack copy(other);
        swap(copy);
    }
    return *this;
}

Stack &Stack::operator=(Stack &&other) noexcept {
    if (this != &other) {
        clear();
        swap(other);
    }
    return *this;
}

Stack::~Stack() { clear(); }

void Stack::swap(Stack &other) noexcept {
    std::swap(top_, other.top_);
    std::swap(size_, other.size_);
}

void Stack::push(const std::string &value) {
    top_ = new Node{value, top_};
    ++size_;
}

std::string Stack::pop() {
    if (!top_)
        throw std::out_of_range("стек пуст");
    Node *removed = top_;
    std::string value = std::move(removed->value);
    top_ = removed->next;
    delete removed;
    --size_;
    return value;
}

const std::string &Stack::top() const {
    if (!top_)
        throw std::out_of_range("стек пуст");
    return top_->value;
}

std::vector<std::string> Stack::values() const {
    std::vector<std::string> result;
    result.reserve(size_);
    for (Node *node = top_; node; node = node->next)
        result.push_back(node->value);
    return result;
}

void Stack::appendSerialized(const std::string &value) {
    Node *added = new Node{value};
    if (!top_) {
        top_ = added;
    } else {
        Node *bottom = top_;
        while (bottom->next)
            bottom = bottom->next;
        bottom->next = added;
    }
    ++size_;
}

void Stack::clear() noexcept {
    while (top_) {
        Node *removed = top_;
        top_ = top_->next;
        delete removed;
    }
    size_ = 0;
}

Queue::Queue(const Queue &other) : Queue() {
    for (const auto &value : other.values())
        push(value);
}

Queue::Queue(Queue &&other) noexcept { swap(other); }

Queue &Queue::operator=(const Queue &other) {
    if (this != &other) {
        Queue copy(other);
        swap(copy);
    }
    return *this;
}

Queue &Queue::operator=(Queue &&other) noexcept {
    if (this != &other) {
        clear();
        swap(other);
    }
    return *this;
}

Queue::~Queue() { clear(); }

void Queue::swap(Queue &other) noexcept {
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    std::swap(size_, other.size_);
}

void Queue::push(const std::string &value) {
    Node *added = new Node{value};
    if (tail_)
        tail_->next = added;
    else
        head_ = added;
    tail_ = added;
    ++size_;
}

std::string Queue::pop() {
    if (!head_)
        throw std::out_of_range("очередь пуста");
    Node *removed = head_;
    std::string value = std::move(removed->value);
    head_ = removed->next;
    if (!head_)
        tail_ = nullptr;
    delete removed;
    --size_;
    return value;
}

const std::string &Queue::front() const {
    if (!head_)
        throw std::out_of_range("очередь пуста");
    return head_->value;
}

std::vector<std::string> Queue::values() const {
    std::vector<std::string> result;
    result.reserve(size_);
    for (Node *node = head_; node; node = node->next)
        result.push_back(node->value);
    return result;
}

void Queue::clear() noexcept {
    while (head_) {
        Node *removed = head_;
        head_ = head_->next;
        delete removed;
    }
    tail_ = nullptr;
    size_ = 0;
}

CompleteBinaryTree::CompleteBinaryTree(const CompleteBinaryTree &other) : CompleteBinaryTree() {
    for (const auto &value : other.values())
        insert(value);
}

CompleteBinaryTree::CompleteBinaryTree(CompleteBinaryTree &&other) noexcept { swap(other); }

CompleteBinaryTree &CompleteBinaryTree::operator=(const CompleteBinaryTree &other) {
    if (this != &other) {
        CompleteBinaryTree copy(other);
        swap(copy);
    }
    return *this;
}

CompleteBinaryTree &CompleteBinaryTree::operator=(CompleteBinaryTree &&other) noexcept {
    if (this != &other) {
        clear();
        swap(other);
    }
    return *this;
}

CompleteBinaryTree::~CompleteBinaryTree() { clear(); }

void CompleteBinaryTree::swap(CompleteBinaryTree &other) noexcept {
    std::swap(root_, other.root_);
    std::swap(size_, other.size_);
}

void CompleteBinaryTree::insert(const std::string &value) {
    auto owner = std::make_unique<Node>();
    owner->value = value;
    if (!root_) {
        root_ = owner.release();
        ++size_;
        return;
    }
    std::queue<Node *> pending;
    pending.push(root_);
    while (!pending.empty()) {
        Node *current = pending.front();
        pending.pop();
        if (!current->left) {
            current->left = owner.release();
            ++size_;
            return;
        }
        if (!current->right) {
            current->right = owner.release();
            ++size_;
            return;
        }
        pending.push(current->left);
        pending.push(current->right);
    }
}

bool CompleteBinaryTree::contains(const std::string &value) const {
    const auto stored = values();
    return std::find(stored.begin(), stored.end(), value) != stored.end();
}

bool CompleteBinaryTree::isComplete() const {
    if (!root_)
        return true;
    std::queue<Node *> pending;
    pending.push(root_);
    bool gap = false;
    while (!pending.empty()) {
        Node *current = pending.front();
        pending.pop();
        if (!current) {
            gap = true;
        } else {
            if (gap)
                return false;
            pending.push(current->left);
            pending.push(current->right);
        }
    }
    return true;
}

std::vector<std::string> CompleteBinaryTree::values() const {
    std::vector<std::string> result;
    result.reserve(size_);
    if (!root_)
        return result;
    std::queue<Node *> pending;
    pending.push(root_);
    while (!pending.empty()) {
        Node *current = pending.front();
        pending.pop();
        result.push_back(current->value);
        if (current->left)
            pending.push(current->left);
        if (current->right)
            pending.push(current->right);
    }
    return result;
}

void CompleteBinaryTree::destroy(Node *node) noexcept {
    if (!node)
        return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

void CompleteBinaryTree::clear() noexcept {
    destroy(root_);
    root_ = nullptr;
    size_ = 0;
}
