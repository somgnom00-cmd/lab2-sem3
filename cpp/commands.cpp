#include "commands.h"
#include "lab.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

// Убрал команды полного дерева из справки.
void help() {
    std::cout << "Команды (значения — текст или числа, индексы с нуля):\n"
        "MCREATE имя | MPUSH имя значение | MINSERT имя индекс значение\n"
        "MGET имя индекс | MSET имя индекс значение | MDEL имя индекс\n"
        "MLEN имя | FCREATE имя | LCREATE имя\n"
        "FPUSH/LPUSH имя HEAD|TAIL значение\n"
        "FPUSH/LPUSH имя BEFORE|AFTER опорное_значение новое_значение\n"
        "FDEL/LDEL имя HEAD|TAIL|VALUE значение_для_VALUE\n"
        "FDEL/LDEL имя BEFORE|AFTER опорное_значение\n"
        "FGET/LGET имя индекс | FFIND/LFIND имя значение\n"
        "SCREATE имя | SPUSH имя значение | SPOP имя | SGET имя\n"
        "QCREATE имя | QPUSH имя значение | QPOP имя | QGET имя\n"
        "PRINT тип имя [REVERSE] | HELP | SAVE | EXIT\n"
        "Типы: M массив, F односвязный, L двусвязный, S стек,\n"
        "Q очередь. REVERSE разрешён для F и L.\n"
        "HEAD/TAIL для удаления вводятся без числа.\n"
        "Текст с пробелами вводится в двойных кавычках.\n";

    labHelp();
}

static void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

bool execute(Database& database, const std::string& query) {
    bool handled = false;
    bool changed = executeLab(database, query, handled);
    if (handled) return changed;

    std::istringstream input(query);
    std::vector<std::string> parts;
    std::string token;

    while (readToken(input, token)) parts.push_back(token);

    require(
        !parts.empty() && !parts[0].empty(),
        "пустая команда; введите HELP"
    );

    std::string command = parts[0];

    auto arguments = [&](int count) {
        require(
            static_cast<int>(parts.size()) == count,
            "неверное число аргументов; введите HELP"
        );
    };

    if (command == "HELP") {
        arguments(1);
        help();
        return false;
    }

    bool print = command == "PRINT";
    char type = command[0];
    std::string operation = command.substr(1);
    std::string name;

    if (print) {
        require(
            parts.size() == 3 || parts.size() == 4,
            "формат: PRINT тип имя [REVERSE]"
        );
        require(parts[1].size() == 1, "тип: M, F, L, S или Q");

        type = parts[1][0];
        name = parts[2];
    } else {
        require(
            parts.size() >= 2,
            "такого варианта нет или нет аргументов; HELP"
        );
        name = parts[1];
    }

    // Полное дерево и команды типа T больше не используются.
    require(
        std::string("MFLSQ").find(type) != std::string::npos,
        "такого варианта не существует; выберите команду из HELP"
    );
    checkName(name);

    if (operation == "CREATE" && !print) {
        arguments(2);
        require(!dbExists(database, type, name), "структура уже существует");

        dbCreate(database, type, name);
        std::cout << "Создано: " << type << ' ' << name << '\n';
        return true;
    }

    require(dbExists(database, type, name), "структура не создана");

    if (print) {
        bool reverse = parts.size() == 4;

        if (reverse) {
            require(
                parts[3] == "REVERSE" && (type == 'F' || type == 'L'),
                "REVERSE разрешён только для списков"
            );
        }

        std::vector<std::string> items =
            dbValues(database, type, name, reverse);

        for (const std::string& value : items) {
            writeValue(std::cout, value);
            std::cout << ' ';
        }

        std::cout << '\n';
        return false;
    }

    int length = dbSize(database, type, name);
    int index = 0;

    if (operation == "LEN" && type == 'M') {
        arguments(2);
        std::cout << length << '\n';
        return false;
    }

    if (operation == "FIND" && (type == 'F' || type == 'L')) {
        arguments(3);
        std::cout << dbFind(database, type, name, parts[2]) << '\n';
        return false;
    }

    if (operation == "GET" ||
        (operation == "POP" && (type == 'S' || type == 'Q'))) {
        if (type == 'M' || type == 'F' || type == 'L') {
            arguments(3);
            index = number(parts[2]);
        } else {
            arguments(2);
        }

        require(
            index >= 0 && index < length,
            "структура пуста или индекс вне диапазона"
        );

        std::cout << dbGet(database, type, name, index) << '\n';

        if (operation == "GET") return false;

        dbErase(database, type, name, index);
    } else if (type == 'M') {
        require(
            operation == "PUSH" || operation == "INSERT" ||
            operation == "SET" || operation == "DEL",
            "неизвестная команда; HELP"
        );

        arguments(operation == "PUSH" || operation == "DEL" ? 3 : 4);

        index = operation == "PUSH" ? length : number(parts[2]);
        bool adding = operation == "PUSH" || operation == "INSERT";

        require(
            index >= 0 && (adding ? index <= length : index < length),
            "индекс вне диапазона"
        );

        if (operation == "DEL") {
            dbErase(database, type, name, index);
        } else {
            std::string value = parts.back();

            if (adding) dbInsert(database, type, name, index, value);
            else dbSet(database, name, index, value);
        }
    } else if (type == 'S' || type == 'Q') {
        require(operation == "PUSH", "неизвестная команда; HELP");
        arguments(3);

        dbInsert(database, type, name, type == 'S' ? 0 : length, parts[2]);
    } else {
        require(
            operation == "PUSH" || operation == "DEL",
            "неизвестная команда; HELP"
        );
        require(
            parts.size() >= 3,
            "нужен способ: HEAD, TAIL, BEFORE, AFTER, VALUE"
        );

        std::string position = parts[2];
        bool adding = operation == "PUSH";

        if (position == "HEAD" || position == "TAIL") {
            arguments(adding ? 4 : 3);
            index = position == "HEAD" ? 0 : (adding ? length : length - 1);
        } else {
            require(
                position == "BEFORE" || position == "AFTER" ||
                (!adding && position == "VALUE"),
                "неизвестный способ; HELP"
            );

            arguments(adding ? 5 : 4);
            index = dbFind(database, type, name, parts[3]);

            require(index >= 0, "опорное значение не найдено");

            if (position == "AFTER") ++index;
            if (position == "BEFORE" && !adding) --index;
        }

        require(
            index >= 0 && (adding ? index <= length : index < length),
            "нет элемента в выбранной позиции"
        );

        if (adding) dbInsert(database, type, name, index, parts.back());
        else dbErase(database, type, name, index);
    }

    std::cout << "Выполнено\n";
    return true;
}