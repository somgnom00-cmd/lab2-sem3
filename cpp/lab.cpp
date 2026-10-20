#include "lab.h"
#include "database.h"
#include "algorithms.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

// Дерево поиска использует структуру SearchTree.
static void need(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void labHelp() {
    std::cout << "\nДобавленные команды:\n"
        "CALC выражение\n"
        "SETADD имя число | SETDEL имя число | SET_AT имя число\n"
        "SETPRINT имя | PARTITION имя сумма_подмножества\n"
        "SUBSETS имя_массива\n"
        "BCREATE имя | BINSERT имя число | BFIND имя число\n"
        "BGET имя | BRIGHT имя\n"
        "HCREATE имя LINEAR|DOUBLE [размер] [FOLD]\n"
        "HSET имя ключ значение | HGET имя ключ | HDEL имя ключ\n"
        "HPRINT имя | HSTATS имя | HCMP [количество_ключей]\n"
        "FOLD число\n"
        "CCREATE имя ёмкость | SET имя ключ значение | GET имя ключ\n"
        "CPRINT имя\n"
        "SETADD создаёт множество при первом добавлении.\n"
        "BRIGHT выводит вид справа. CPRINT — от давнего к недавнему.\n"
        "GET и HGET возвращают значение, при отсутствии — -1.\n";
}

static bool known(const std::string& command) {
    const std::string names =
        " CALC SETADD SETDEL SET_AT SETPRINT PARTITION SUBSETS "
        "BCREATE BINSERT BFIND BGET BRIGHT HCREATE HSET HGET HDEL "
        "HPRINT HSTATS HCMP FOLD CCREATE SET GET CPRINT ";

    return !command.empty() &&
        names.find(" " + command + " ") != std::string::npos;
}

bool executeLab(
    Database& database,
    const std::string& query,
    bool& handled
) {
    std::istringstream input(query);
    std::string command;
    input >> command;

    handled = known(command);
    if (!handled) return false;

    std::vector<std::string> parts;
    std::string token;
    parts.push_back(command);

    while (readToken(input, token)) parts.push_back(token);

    if (command == "CALC") {
        need(parts.size() >= 2, "формат: CALC выражение");

        std::string expression;

        for (std::size_t i = 1; i < parts.size(); ++i) {
            if (i > 1) expression += ' ';
            expression += parts[i];
        }

        std::cout << calculate(expression) << '\n';
        return false;
    }

    if (command == "FOLD") {
        need(parts.size() == 2, "формат: FOLD число");
        std::cout << hashFold(number(parts[1])) << '\n';
        return false;
    }

    if (command == "HCMP") {
        need(parts.size() <= 2, "формат: HCMP [количество_ключей]");
        compareHashing(parts.size() == 2 ? number(parts[1]) : 1000);
        return false;
    }

    need(parts.size() >= 2, "не указано имя структуры");

    const std::string& name = parts[1];
    checkName(name);

    if (command == "SUBSETS") {
        need(parts.size() == 2, "формат: SUBSETS имя_массива");
        need(database.arrays.count(name), "массив не создан");

        printSubsets(database.arrays.at(name));
        return false;
    }

    if (command == "SETADD" || command == "SETDEL" ||
        command == "SET_AT" || command == "SETPRINT" ||
        command == "PARTITION") {
        need(
            parts.size() == (command == "SETPRINT" ? 2u : 3u),
            "неверное число аргументов"
        );

        int value = parts.size() == 3 ? number(parts[2]) : 0;

        if (command == "SETADD" && !database.sets.count(name)) {
            hashInit(database.sets[name], 17, true, false);
        }

        need(database.sets.count(name), "множество не создано");
        HashTable& set = database.sets.at(name);

        if (command == "SETPRINT") {
            std::cout << '{';
            bool first = true;

            for (int i = 0; i < set.states.size; ++i) {
                if (set.states.data[i] == "1") {
                    if (!first) std::cout << ", ";
                    std::cout << set.keys.data[i];
                    first = false;
                }
            }

            std::cout << "}\n";
            return false;
        }

        if (command == "PARTITION") {
            printPartition(set, value);
            return false;
        }

        if (command == "SET_AT") {
            std::cout << (hashFind(set, value) >= 0 ? "TRUE\n" : "FALSE\n");
            return false;
        }

        if (command == "SETDEL") {
            bool changed = hashErase(set, value);
            std::cout << (changed ? "Удалено\n" : "Не найдено\n");
            return changed;
        }

        hashSet(set, value, 0);
        std::cout << "Выполнено\n";
        return true;
    }

    if (command == "BCREATE") {
        need(parts.size() == 2, "формат: BCREATE имя");
        need(!database.searchTrees.count(name), "дерево уже существует");

        database.searchTrees.try_emplace(name);
        std::cout << "Создано: B " << name << '\n';
        return true;
    }

    if (command == "BINSERT" || command == "BFIND" ||
        command == "BGET" || command == "BRIGHT") {
        bool withValue = command == "BINSERT" || command == "BFIND";

        need(
            parts.size() == (withValue ? 3u : 2u),
            "неверное число аргументов"
        );
        need(database.searchTrees.count(name), "дерево не создано");

        SearchTree& tree = database.searchTrees.at(name);

        if (withValue) {
            int value = number(parts[2]);

            if (command == "BFIND") {
                std::cout << (
                    treeSearchFind(tree, value) ? "TRUE\n" : "FALSE\n"
                );
                return false;
            }

            treeSearchInsert(tree, value);
            std::cout << "Выполнено\n";
            return true;
        }

        std::vector<std::string> values = command == "BRIGHT"
            ? treeRightView(tree)
            : treeValues(tree);

        for (std::size_t i = 0; i < values.size(); ++i) {
            if (i) std::cout << ' ';
            std::cout << values[i];
        }

        std::cout << '\n';
        return false;
    }

    if (command == "HCREATE") {
        need(
            parts.size() >= 3 && parts.size() <= 5,
            "формат: HCREATE имя LINEAR|DOUBLE [размер] [FOLD]"
        );
        need(
            parts[2] == "LINEAR" || parts[2] == "DOUBLE",
            "метод: LINEAR или DOUBLE"
        );
        need(!database.hashes.count(name), "таблица уже существует");

        int capacity = 17;
        bool folding = false;

        if (parts.size() > 3) {
            if (parts[3] == "FOLD") {
                need(parts.size() == 4, "лишний аргумент");
                folding = true;
            } else {
                capacity = number(parts[3]);
            }
        }

        if (parts.size() == 5) {
            need(parts[4] == "FOLD", "ожидается FOLD");
            folding = true;
        }

        need(
            capacity >= 1 && capacity <= 1000000,
            "размер таблицы: от 1 до 1000000"
        );

        hashInit(
            database.hashes[name],
            capacity,
            parts[2] == "DOUBLE",
            folding
        );

        std::cout << "Создано: H " << name << '\n';
        return true;
    }

    if (command == "HSET" || command == "HGET" ||
        command == "HDEL" || command == "HPRINT" ||
        command == "HSTATS") {
        std::size_t count = command == "HSET"
            ? 4
            : (command == "HGET" || command == "HDEL" ? 3 : 2);

        need(parts.size() == count, "неверное число аргументов");
        need(database.hashes.count(name), "таблица не создана");

        HashTable& table = database.hashes.at(name);

        if (command == "HPRINT") {
            for (int i = 0; i < table.states.size; ++i) {
                if (table.states.data[i] == "1") {
                    std::cout << table.keys.data[i] << " -> "
                              << table.values.data[i] << '\n';
                }
            }
            return false;
        }

        if (command == "HSTATS") {
            std::cout << (table.doubleHash ? "DOUBLE" : "LINEAR")
                      << " size=" << table.size
                      << " capacity=" << table.states.size
                      << " load="
                      << static_cast<double>(table.size) / table.states.size
                      << " folding=" << (table.folding ? "YES" : "NO")
                      << '\n';
            return false;
        }

        int key = number(parts[2]);

        if (command == "HGET") {
            int value;
            std::cout << (hashGet(table, key, value) ? value : -1) << '\n';
            return false;
        }

        if (command == "HDEL") {
            bool changed = hashErase(table, key);
            std::cout << (changed ? "Удалено\n" : "Не найдено\n");
            return changed;
        }

        hashSet(table, key, number(parts[3]));
        std::cout << "Выполнено\n";
        return true;
    }

    if (command == "CCREATE") {
        need(parts.size() == 3, "формат: CCREATE имя ёмкость");
        need(!database.caches.count(name), "кэш уже существует");

        int capacity = number(parts[2]);

        need(
            capacity >= 0 && capacity <= 1000000,
            "ёмкость кэша: от 0 до 1000000"
        );

        cacheInit(database.caches[name], capacity);
        std::cout << "Создано: C " << name << '\n';
        return true;
    }

    need(database.caches.count(name), "кэш не создан");
    LRUCache& cache = database.caches.at(name);

    if (command == "CPRINT") {
        need(parts.size() == 2, "формат: CPRINT имя");

        for (DoubleNode* current = cache.order.head;
             current; current = current->next) {
            int slot = number(current->value);
            std::cout << cache.keys.data[slot] << " -> "
                      << cache.values.data[slot] << '\n';
        }

        return false;
    }

    need(
        parts.size() == (command == "SET" ? 4u : 3u),
        "неверное число аргументов"
    );

    int key = number(parts[2]);

    if (command == "GET") {
        int value;
        bool found = cacheGet(cache, key, value);

        std::cout << (found ? value : -1) << '\n';

        // Успешное чтение меняет порядок LRU.
        return found;
    }

    cacheSet(cache, key, number(parts[3]));
    std::cout << "Выполнено\n";
    return cache.capacity > 0;
}

// E — множество, H — таблица, C — кэш, B — дерево поиска.
bool labLoadLine(Database& database, const std::string& line) {
    std::istringstream input(line);
    std::string type, name;

    if (!(input >> type) ||
        (type != "E" && type != "H" && type != "C" && type != "B")) {
        return false;
    }

    need(
        static_cast<bool>(input >> name),
        "в файле не указано имя структуры"
    );
    checkName(name);

    std::vector<std::string> tokens;
    std::string token;

    while (readToken(input, token)) tokens.push_back(token);

    if (type == "E") {
        need(!database.sets.count(name), "повтор имени множества в файле");

        for (const std::string& value : tokens) number(value);

        HashTable& set = database.sets[name];
        hashInit(set, 17, true, false);

        for (const std::string& value : tokens) {
            hashSet(set, number(value), 0);
        }
    } else if (type == "B") {
        need(
            !database.searchTrees.count(name),
            "повтор имени дерева в файле"
        );

        for (const std::string& value : tokens) number(value);

        SearchTree& tree = database.searchTrees[name];

        for (const std::string& value : tokens) {
            treeSearchInsert(tree, number(value));
        }
    } else if (type == "H") {
        need(!database.hashes.count(name), "повтор имени таблицы в файле");
        need(
            tokens.size() >= 3 && (tokens.size() - 3) % 2 == 0,
            "неверная строка хеш-таблицы"
        );
        need(
            tokens[0] == "LINEAR" || tokens[0] == "DOUBLE",
            "неверный метод хеширования"
        );
        need(
            tokens[1] == "FOLD" || tokens[1] == "DIV",
            "неверная хеш-функция"
        );

        int capacity = number(tokens[2]);

        need(
            capacity >= 1 && capacity <= 10000000,
            "неверный размер таблицы в файле"
        );

        for (std::size_t i = 3; i < tokens.size(); ++i) {
            number(tokens[i]);
        }

        HashTable& table = database.hashes[name];

        hashInit(
            table, capacity,
            tokens[0] == "DOUBLE",
            tokens[1] == "FOLD"
        );

        for (std::size_t i = 3; i < tokens.size(); i += 2) {
            int key = number(tokens[i]);
            need(hashFind(table, key) < 0, "повтор ключа в таблице");
            hashSet(table, key, number(tokens[i + 1]));
        }
    } else {
        need(!database.caches.count(name), "повтор имени кэша в файле");
        need(
            tokens.size() >= 1 && (tokens.size() - 1) % 2 == 0,
            "неверная строка кэша"
        );

        int capacity = number(tokens[0]);

        need(
            capacity >= 0 && capacity <= 1000000 &&
            (tokens.size() - 1) / 2 <= static_cast<std::size_t>(capacity),
            "неверная ёмкость кэша в файле"
        );

        for (std::size_t i = 1; i < tokens.size(); ++i) {
            number(tokens[i]);
        }

        LRUCache& cache = database.caches[name];
        cacheInit(cache, capacity);

        for (std::size_t i = 1; i < tokens.size(); i += 2) {
            int key = number(tokens[i]);
            need(hashFind(cache.index, key) < 0, "повтор ключа кэша");
            cacheSet(cache, key, number(tokens[i + 1]));
        }
    }

    return true;
}

void labSave(Database& database, std::ostream& output) {
    for (auto& item : database.sets) {
        output << "E " << item.first;
        HashTable& set = item.second;

        for (int i = 0; i < set.states.size; ++i) {
            if (set.states.data[i] == "1") {
                output << ' ' << set.keys.data[i];
            }
        }

        output << '\n';
    }

    for (auto& item : database.searchTrees) {
        output << "B " << item.first;

        for (const std::string& value : treeValues(item.second)) {
            output << ' ' << value;
        }

        output << '\n';
    }

    for (auto& item : database.hashes) {
        HashTable& table = item.second;

        output << "H " << item.first << ' '
               << (table.doubleHash ? "DOUBLE" : "LINEAR") << ' '
               << (table.folding ? "FOLD" : "DIV") << ' '
               << table.states.size;

        for (int i = 0; i < table.states.size; ++i) {
            if (table.states.data[i] == "1") {
                output << ' ' << table.keys.data[i]
                       << ' ' << table.values.data[i];
            }
        }

        output << '\n';
    }

    for (auto& item : database.caches) {
        LRUCache& cache = item.second;
        output << "C " << item.first << ' ' << cache.capacity;

        for (DoubleNode* current = cache.order.head;
             current; current = current->next) {
            int slot = number(current->value);

            output << ' ' << cache.keys.data[slot]
                   << ' ' << cache.values.data[slot];
        }

        output << '\n';
    }
}

void clearLabDatabase(Database& database) {
    for (auto& item : database.sets) clearHash(item.second);
    for (auto& item : database.hashes) clearHash(item.second);
    for (auto& item : database.caches) clearCache(item.second);
    for (auto& item : database.searchTrees) clearTree(item.second);

    database.sets.clear();
    database.hashes.clear();
    database.caches.clear();
    database.searchTrees.clear();
}