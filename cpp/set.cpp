#include "set.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

static int bucketOf(int value, int capacity) {
    long long remainder = static_cast<long long>(value) % capacity;
    return static_cast<int>(remainder < 0 ? remainder + capacity : remainder);
}

static void rehash(Set& set) {
    std::vector<SinglyList> buckets(set.buckets.size() * 2 + 1);
    for (std::size_t i = 0; i < set.buckets.size(); ++i) {
        SingleNode* node = set.buckets[i].head;
        while (node) {
            SingleNode* next = node->next;
            SinglyList& chain = buckets[bucketOf(std::stoi(node->value), buckets.size())];
            node->next = chain.head;
            chain.head = node;
            if (!chain.tail) chain.tail = node;
            ++chain.size;
            node = next;
        }
    }
    set.buckets.swap(buckets);
}

bool setAt(const Set& set, int value) {
    const SinglyList& chain = set.buckets[bucketOf(value, set.buckets.size())];
    return singlyFind(chain, std::to_string(value)) >= 0;
}

bool setAdd(Set& set, int value) {
    if (setAt(set, value)) return false;
    if (set.size + 1 > static_cast<int>(set.buckets.size())) rehash(set);
    singlyInsert(set.buckets[bucketOf(value, set.buckets.size())], 0, std::to_string(value));
    ++set.size;
    return true;
}

bool setDel(Set& set, int value) {
    SinglyList& chain = set.buckets[bucketOf(value, set.buckets.size())];
    int index = singlyFind(chain, std::to_string(value));
    if (index < 0) return false;
    singlyErase(chain, index);
    --set.size;
    return true;
}

void setValues(const Set& set, Array& values) {
    clearArray(values);
    for (std::size_t i = 0; i < set.buckets.size(); ++i)
        for (SingleNode* node = set.buckets[i].head; node; node = node->next)
            arrayPush(values, node->value);
}

void clearSet(Set& set) {
    for (std::size_t i = 0; i < set.buckets.size(); ++i) clearSinglyList(set.buckets[i]);
    set.size = 0;
}

static int readInteger(const std::string& token) {
    std::size_t end;
    int value = std::stoi(token, &end);
    if (end != token.size()) throw std::runtime_error("ожидалось целое число");
    return value;
}

std::string setFileQuery(const std::string& filename, const std::string& query) {
    std::istringstream command(query);
    std::string operation, token, extra;
    if (!(command >> operation >> token) || (command >> extra) ||
        (operation != "SETADD" && operation != "SETDEL" && operation != "SET_AT"))
        throw std::runtime_error("запрос: SETADD число, SETDEL число или SET_AT число");
    int value = readInteger(token);
    Set set;
    try {
        std::ifstream input(filename);
        if (!input && std::filesystem::exists(filename))
            throw std::runtime_error("не удалось открыть файл");
        // Файл содержит только элементы множества, разделённые пробелами.
        while (input >> token) setAdd(set, readInteger(token));
        if (input.bad()) throw std::runtime_error("ошибка чтения файла");
        bool result, changed = false;
        if (operation == "SETADD") result = changed = setAdd(set, value);
        else if (operation == "SETDEL") result = changed = setDel(set, value);
        else result = setAt(set, value);
        input.close();
        if (changed) {
            std::ofstream output(filename);
            if (!output) throw std::runtime_error("не удалось записать файл");
            for (std::size_t i = 0; i < set.buckets.size(); ++i)
                for (SingleNode* node = set.buckets[i].head; node; node = node->next)
                    output << node->value << ' ';
            output << '\n';
            output.close();
            if (!output) throw std::runtime_error("ошибка записи файла");
        }
        clearSet(set);
        return result ? "TRUE" : "FALSE";
    } catch (...) {
        clearSet(set);
        throw;
    }
}
