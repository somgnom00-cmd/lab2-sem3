#include "task1.h"
#include "stack.h"
#include <cctype>
#include <stdexcept>

static int priority(char operation) {
    if (operation == '~' || operation == '#') return 3;
    if (operation == '*') return 2;
    return 1;
}

static void apply(Stack& numbers, Stack& operations) {
    char operation = stackGet(operations)[0];
    stackPop(operations);
    if (!numbers.top) throw std::runtime_error("не хватает числа");
    long long right = std::stoi(stackGet(numbers));
    stackPop(numbers);
    long long result;
    if (operation == '~' || operation == '#') {
        result = operation == '~' ? -right : right;
    } else {
        if (!numbers.top) throw std::runtime_error("не хватает числа");
        long long left = std::stoi(stackGet(numbers));
        stackPop(numbers);
        if (operation == '+') result = left + right;
        else if (operation == '-') result = left - right;
        else result = left * right;
    }
    if (result < -2000000000LL || result > 2000000000LL)
        throw std::runtime_error("результат выходит за пределы 2*10^9");
    stackPush(numbers, std::to_string(result));
}

long long calculateExpression(const std::string& expression) {
    Stack numbers, operations;
    bool needNumber = true;
    try {
        for (std::size_t i = 0; i < expression.size();) {
            char c = expression[i];
            if (std::isspace(static_cast<unsigned char>(c))) { ++i; continue; }
            if (std::isdigit(static_cast<unsigned char>(c))) {
                if (!needNumber) throw std::runtime_error("между числами нет операции");
                long long value = 0;
                while (i < expression.size() &&
                       std::isdigit(static_cast<unsigned char>(expression[i]))) {
                    value = value * 10 + expression[i++] - '0';
                    if (value > 2000000000LL)
                        throw std::runtime_error("число выходит за пределы 2*10^9");
                }
                stackPush(numbers, std::to_string(value));
                needNumber = false;
            } else if (c == '(') {
                if (!needNumber) throw std::runtime_error("перед скобкой нет операции");
                stackPush(operations, "(");
                ++i;
            } else if (c == ')') {
                if (needNumber) throw std::runtime_error("пустая или неверная скобка");
                while (operations.top && stackGet(operations) != "(") apply(numbers, operations);
                if (!operations.top) throw std::runtime_error("лишняя закрывающая скобка");
                stackPop(operations);
                ++i;
            } else if (c == '+' || c == '-' || c == '*') {
                if (needNumber) {
                    if (c == '*') throw std::runtime_error("нет левого операнда");
                    // Унарные знаки отличаются от бинарных операций.
                    stackPush(operations, c == '-' ? "~" : "#");
                } else {
                    while (operations.top && stackGet(operations) != "(" &&
                           priority(stackGet(operations)[0]) >= priority(c)) apply(numbers, operations);
                    stackPush(operations, std::string(1, c));
                    needNumber = true;
                }
                ++i;
            } else throw std::runtime_error("недопустимый символ в выражении");
        }
        if (needNumber) throw std::runtime_error("выражение не закончено");
        while (operations.top) {
            if (stackGet(operations) == "(") throw std::runtime_error("скобка не закрыта");
            apply(numbers, operations);
        }
        long long result = std::stoi(stackGet(numbers));
        clearStack(numbers);
        return result;
    } catch (...) {
        clearStack(numbers);
        clearStack(operations);
        throw;
    }
}
