#include "task1.h"
#include "set.h"
#include "task3.h"
#include "task4.h"
#include "task5.h"
#include "hashtable.h"
#include "task7.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) throw std::runtime_error("ввод завершён");
    return line;
}

static int integer(const std::string& text) {
    std::istringstream input(text);
    std::string token, extra;
    if (!(input >> token) || (input >> extra)) throw std::runtime_error("ожидалось целое число");
    std::size_t end;
    int value = std::stoi(token, &end);
    if (end != token.size()) throw std::runtime_error("ожидалось целое число");
    return value;
}

static int readInt(const std::string& prompt) { return integer(readLine(prompt)); }

static void readArray(Array& values, const std::string& prompt, bool numeric) {
    std::istringstream input(readLine(prompt));
    std::string token;
    while (input >> token) arrayPush(values, numeric ? std::to_string(integer(token)) : token);
}

static std::string readOperation(int& key, int& value, bool deletion) {
    std::istringstream input(readLine("Запрос: "));
    std::string op, x, y, extra;
    if (!(input >> op >> x)) throw std::runtime_error("нужны команда и ключ");
    key = integer(x);
    if (op == "SET") {
        if (!(input >> y)) throw std::runtime_error("SET ключ значение");
        value = integer(y);
    } else if (op != "GET" && !(deletion && op == "DEL")) throw std::runtime_error("неизвестная команда");
    if (input >> extra) throw std::runtime_error("лишние аргументы запроса");
    return op;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        try {
            std::string file, query;
            for (int i = 1; i < argc; ++i) {
                std::string argument = argv[i];
                if (argument == "--file" && i + 1 < argc && file.empty()) file = argv[++i];
                else if (argument == "--query" && i + 1 < argc && query.empty()) query = argv[++i];
                else throw std::runtime_error("./lab2 --file файл --query 'SETADD 5'");
            }
            if (file.empty() || query.empty()) throw std::runtime_error("нужны --file и --query");
            std::cout << setFileQuery(file, query) << '\n';
            return 0;
        } catch (const std::exception& error) { std::cerr << "Ошибка: " << error.what() << '\n'; return 1; }
    }
    while (std::cin) {
        Array values, result;
        Set set;
        try {
            int task = readInt("\n1 — выражение; 2 — множество; 3 — разбиение; 4 — подмножества;\n5 — правый край BST; 6 — хеш-таблица; 7 — LRU; 0 — выход.\nВыбор: ");
            if (task == 0) break;
            if (task == 1) {
                std::string expression = readLine("Выражение, например (2 + 3) * 4: ");
                std::cout << calculateExpression(expression) << '\n';
            } else if (task == 2) {
                std::string file = readLine("Файл множества (например set.txt): ");
                std::string query = readLine("Запрос, например SETADD 5: ");
                std::cout << setFileQuery(file, query) << '\n';
            } else if (task == 3) {
                readArray(values, "Множество, например 4 10 5 1 3 7: ", true);
                for (int i = 0; i < values.size; ++i) setAdd(set, std::stoi(arrayGet(values, i)));
                int target = readInt("Сумма каждой группы, например 15: ");
                std::cout << partitionSet(set, target);
            } else if (task == 4) {
                readArray(values, "Массив, например x y z (пустая строка — пустой массив): ", false);
                std::cout << allSubsets(values) << '\n';
            } else if (task == 5) {
                readArray(values, "Узлы в порядке вставки, например 8 5 10 3 9 15 12: ", true);
                bstRightView(values, result);
                for (int i = 0; i < result.size; ++i) std::cout << (i ? " " : "") << arrayGet(result, i);
                std::cout << '\n';
            } else if (task == 6 || task == 7) {
                HashTable table;
                int mode = task == 6 ? readInt("1 — линейное; 2 — двойное; 3 — сравнение методов: ") : 0;
                if (task == 6 && (mode < 1 || mode > 3)) throw std::runtime_error("выберите 1, 2 или 3");
                if (mode == 3) { std::cout << compareHashing(); continue; }
                if (task == 6) {
                    hashInit(table, 17, mode == 2);
                    int key = readInt("Ключ для свёртки, например 523456795: ");
                    std::cout << "Свёртка: " << foldKey(key) << "; индекс: " << foldKey(key) % table.cells.size() << '\n';
                }
                LRUCache cache(task == 7 ? readInt("Ёмкость кеша, например 2: ") : 0);
                int count = readInt("Количество запросов (SET x y, GET x; для таблицы также DEL x): ");
                if (count < 0) throw std::runtime_error("количество не может быть отрицательным");
                for (int i = 0; i < count; ++i) {
                    int key, value = 0;
                    std::string op = readOperation(key, value, task == 6);
                    if (task == 7 && op == "SET") cacheSet(cache, key, value);
                    else if (task == 7) std::cout << cacheGet(cache, key) << '\n';
                    else if (op == "SET") { hashSet(table, key, value); std::cout << "OK\n"; }
                    else if (op == "DEL") std::cout << (hashDel(table, key) ? "TRUE\n" : "FALSE\n");
                    else std::cout << (hashGet(table, key, value) ? value : -1) << '\n';
                }
            } else throw std::runtime_error("выберите задание от 1 до 7");
        } catch (const std::exception& error) { if (std::cin) std::cout << "Ошибка: " << error.what() << '\n'; }
        clearArray(values); clearArray(result); clearSet(set);
    }
}
