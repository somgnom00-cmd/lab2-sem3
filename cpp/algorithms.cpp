// Добавил алгоритмы обработки выражений, множеств и массивов.
#include "algorithms.h"
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <chrono>
#include <iomanip>

static bool unary(char operation) { return operation == '~' || operation == '#'; }
static int priority(char operation) {
    if (unary(operation)) return 3;
    return operation == '*' ? 2 : 1;
}

static void applyOperation(Stack& values, Stack& operations) {
    char operation = stackGet(operations)[0];
    stackPop(operations);
    if (values.size < (unary(operation) ? 1 : 2))
        throw std::runtime_error("в выражении не хватает числа");
    long long right = std::stoll(stackGet(values));
    stackPop(values);
    long long result;
    if (unary(operation)) result = operation == '~' ? -right : right;
    else {
        long long left = std::stoll(stackGet(values));
        stackPop(values);
        if (operation == '+') result = left + right;
        else if (operation == '-') result = left - right;
        else result = left * right;
    }
    if (result < -2000000000LL || result > 2000000000LL)
        throw std::runtime_error("результат вне диапазона от -2000000000 до 2000000000");
    stackPush(values, std::to_string(result));
}

static void applyUnary(Stack& values, Stack& operations) {
    while (operations.size && unary(stackGet(operations)[0]))
        applyOperation(values, operations);
}

// Два экземпляра своего стека: один для чисел, второй для операций.
int calculate(const std::string& expression) {
    Stack values, operations;
    bool operand = true;
    try {
        for (std::size_t i = 0; i < expression.size();) {
            unsigned char symbol = static_cast<unsigned char>(expression[i]);
            if (std::isspace(symbol)) { ++i; continue; }
            if (symbol >= '0' && symbol <= '9') {
                if (!operand) throw std::runtime_error("между числами нужна операция");
                long long value = 0;
                while (i < expression.size() && expression[i] >= '0' && expression[i] <= '9') {
                    value = value * 10 + expression[i++] - '0';
                    if (value > 2000000000LL) throw std::runtime_error("слишком большое число");
                }
                stackPush(values, std::to_string(value));
                applyUnary(values, operations);
                operand = false;
                continue;
            }
            ++i;
            if (symbol == '(') {
                if (!operand) throw std::runtime_error("перед скобкой нужна операция");
                stackPush(operations, "(");
            } else if (symbol == ')') {
                if (operand) throw std::runtime_error("пустые скобки или пропущено число");
                while (operations.size && stackGet(operations) != "(") applyOperation(values, operations);
                if (!operations.size) throw std::runtime_error("лишняя закрывающая скобка");
                stackPop(operations);
                applyUnary(values, operations);
            } else if (symbol == '+' || symbol == '-' || symbol == '*') {
                if (operand) {
                    if (symbol == '*') throw std::runtime_error("перед умножением нужно число");
                    stackPush(operations, symbol == '-' ? "~" : "#");
                } else {
                    while (operations.size && stackGet(operations) != "(" &&
                           priority(stackGet(operations)[0]) >= priority(symbol))
                        applyOperation(values, operations);
                    stackPush(operations, std::string(1, symbol));
                    operand = true;
                }
            } else throw std::runtime_error("недопустимый символ в выражении");
        }
        if (operand) throw std::runtime_error("пустое или незавершённое выражение");
        while (operations.size) {
            if (stackGet(operations) == "(") throw std::runtime_error("не закрыта скобка");
            applyOperation(values, operations);
        }
        if (values.size != 1) throw std::runtime_error("некорректное выражение");
        int result = number(stackGet(values));
        clearStack(values);
        return result;
    } catch (...) {
        clearStack(values);
        clearStack(operations);
        throw;
    }
}

static bool distribute(const Array& items, Array& sums, Array& groups, int index, int target) {
    if (index == items.size) return true;
    int value = number(items.data[index]);
    for (int group = 0; group < sums.size; ++group) {
        int sum = number(sums.data[group]);
        if (1LL * sum + value > target) continue;
        bool repeated = false;
        for (int before = 0; before < group; ++before)
            if (sums.data[before] == sums.data[group]) { repeated = true; break; }
        if (repeated) continue;
        arraySet(sums, group, std::to_string(sum + value));
        arraySet(groups, index, std::to_string(group));
        if (distribute(items, sums, groups, index + 1, target)) return true;
        arraySet(sums, group, std::to_string(sum));
        if (sum == 0) break;
    }
    return false;
}

void printPartition(HashTable& set, int target) {
    if (target <= 0) throw std::runtime_error("сумма подмножества должна быть положительной");
    Array items, sums, groups;
    try {
        long long total = 0;
        for (int i = 0; i < set.states.size; ++i) if (set.states.data[i] == "1") {
            int value = number(set.keys.data[i]);
            if (value <= 0) throw std::runtime_error("для разбиения нужны натуральные числа");
            total += value;
            arrayInsert(items, items.size, std::to_string(value));
        }
        if (!items.size) { std::cout << "Пустое множество\n"; return; }
        // Сортировка вставками: сначала распределяю большие числа.
        for (int i = 1; i < items.size; ++i) {
            std::string value = items.data[i];
            int j = i;
            while (j > 0 && number(items.data[j - 1]) < number(value)) {
                arraySet(items, j, items.data[j - 1]); --j;
            }
            arraySet(items, j, value);
        }
        if (total % target || total / target > items.size || number(items.data[0]) > target) {
            std::cout << "Разбиение невозможно\n";
            clearArray(items); return;
        }
        int count = static_cast<int>(total / target);
        for (int i = 0; i < count; ++i) arrayInsert(sums, i, "0");
        for (int i = 0; i < items.size; ++i) arrayInsert(groups, i, "0");
        if (!distribute(items, sums, groups, 0, target)) std::cout << "Разбиение невозможно\n";
        else for (int group = 0; group < count; ++group) {
            bool first = true;
            std::cout << '{';
            for (int i = 0; i < items.size; ++i) if (number(groups.data[i]) == group) {
                if (!first) std::cout << ", ";
                std::cout << items.data[i]; first = false;
            }
            std::cout << "}\n";
        }
        clearArray(items); clearArray(sums); clearArray(groups);
    } catch (...) { clearArray(items); clearArray(sums); clearArray(groups); throw; }
}

static void combinations(const Array& items, Array& selected, int start, int needed) {
    if (needed == 0) {
        std::cout << '{';
        for (int i = 0; i < selected.size; ++i) {
            if (i) std::cout << ", ";
            writeValue(std::cout, selected.data[i]);
        }
        std::cout << "}\n"; return;
    }
    for (int i = start; i + needed <= items.size; ++i) {
        if (i > start && items.data[i] == items.data[i - 1]) continue;
        arrayInsert(selected, selected.size, items.data[i]);
        combinations(items, selected, i + 1, needed - 1);
        arrayErase(selected, selected.size - 1);
    }
}

// Сортирую копию массива и пропускаю одинаковые выборы, исходный массив не меняю.
void printSubsets(const Array& array) {
    Array items, selected;
    try {
        for (int i = 0; i < array.size; ++i) arrayInsert(items, i, array.data[i]);
        for (int i = 1; i < items.size; ++i) {
            std::string value = items.data[i];
            int j = i;
            while (j > 0 && items.data[j - 1] > value) {
                arraySet(items, j, items.data[j - 1]); --j;
            }
            arraySet(items, j, value);
        }
        for (int size = 0; size <= items.size; ++size) combinations(items, selected, 0, size);
        clearArray(items); clearArray(selected);
    } catch (...) { clearArray(items); clearArray(selected); throw; }
}

// Замеры выполняются на одинаковых ключах, без файлового ввода-вывода.
void compareHashing(int count) {
    if (count < 1 || count > 10000) throw std::runtime_error("число ключей: от 1 до 10000");
    int capacity = hashPrime(count * 2 + 1);
    for (int pattern = 0; pattern < 3; ++pattern) for (int method = 0; method < 2; ++method) {
        HashTable table;
        hashInit(table, capacity, method == 1, false);
        Array keys;
        try {
            unsigned int seed = 17;
            for (int i = 0; i < count; ++i) {
                seed = seed * 1664525u + 1013904223u;
                int key = pattern == 0 ? i : (pattern == 1 ?
                    i * 10000 + static_cast<int>(seed % 10000) : i * capacity);
                arrayInsert(keys, i, std::to_string(key));
            }
            auto started = std::chrono::steady_clock::now();
            for (int i = 0; i < count; ++i) hashSet(table, number(keys.data[i]), i, false);
            auto inserted = std::chrono::steady_clock::now();
            unsigned long long insertProbes = table.probes;
            table.probes = 0;
            int value = 0;
            for (int i = 0; i < count; ++i) {
                if (!hashGet(table, number(keys.data[i]), value) || value != i)
                    throw std::runtime_error("ошибка контрольного поиска");
            }
            auto searched = std::chrono::steady_clock::now();
            unsigned long long findProbes = table.probes;
            table.probes = 0;
            for (int i = 0; i < count; ++i) hashErase(table, number(keys.data[i]));
            auto erased = std::chrono::steady_clock::now();
            std::cout << (pattern == 0 ? "sequential" : pattern == 1 ? "mixed" : "collisions")
                << ' ' << (method == 0 ? "LINEAR" : "DOUBLE")
                << " N=" << count << " M=" << capacity
                << " insert_ns=" << std::chrono::duration_cast<std::chrono::nanoseconds>(inserted - started).count()
                << " find_ns=" << std::chrono::duration_cast<std::chrono::nanoseconds>(searched - inserted).count()
                << " erase_ns=" << std::chrono::duration_cast<std::chrono::nanoseconds>(erased - searched).count()
                << " probes=" << insertProbes << '/' << findProbes << '/' << table.probes << '\n';
            clearHash(table); clearArray(keys);
        } catch (...) { clearHash(table); clearArray(keys); throw; }
    }
}
