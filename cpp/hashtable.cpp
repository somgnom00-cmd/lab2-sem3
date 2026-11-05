#include "hashtable.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <limits>
#include <stdexcept>

long long foldKey(int key) {
    long long number = key;
    if (number < 0) number = -number;
    long long sum = 0;
    do { sum += number % 1000; number /= 1000; } while (number);
    return sum;
}

static int positiveMod(long long value, int divisor) {
    long long result = value % divisor;
    return static_cast<int>(result < 0 ? result + divisor : result);
}

static bool prime(int value) {
    for (int i = 2; i <= value / i; ++i) if (value % i == 0) return false;
    return value >= 2;
}

void hashInit(HashTable& table, int capacity, bool doubleHash, bool folding) {
    if (capacity < 3) capacity = 3;
    while (!prime(capacity)) {
        if (capacity == std::numeric_limits<int>::max()) throw std::runtime_error("слишком большая таблица");
        ++capacity;
    }
    table.cells.assign(capacity, HashCell{});
    table.size = table.deleted = 0;
    table.probes = 0;
    table.doubleHash = doubleHash; table.folding = folding; table.autoResize = true;
}

static int position(const HashTable& table, int key, int attempt) {
    int capacity = static_cast<int>(table.cells.size());
    int start = positiveMod(table.folding ? foldKey(key) : key, capacity);
    int step = table.doubleHash ? 1 + positiveMod(key, capacity - 1) : 1;
    return static_cast<int>((start + static_cast<long long>(attempt) * step) % capacity);
}

static int find(HashTable& table, int key) {
    for (int i = 0; i < static_cast<int>(table.cells.size()); ++i) {
        int index = position(table, key, i);
        ++table.probes;
        if (table.cells[index].state == 0) break;
        if (table.cells[index].state == 1 && table.cells[index].key == key) return index;
    }
    return -1;
}

static void rebuild(HashTable& table, int capacity) {
    HashTable fresh;
    hashInit(fresh, capacity, table.doubleHash, table.folding);
    fresh.autoResize = false;
    for (std::size_t i = 0; i < table.cells.size(); ++i)
        if (table.cells[i].state == 1) hashSet(fresh, table.cells[i].key, table.cells[i].value);
    table.cells.swap(fresh.cells);
    table.deleted = 0;
    table.probes += fresh.probes;
}

void hashSet(HashTable& table, int key, int value) {
    int found = find(table, key);
    if (found >= 0) { table.cells[found].value = value; return; }
    int capacity = static_cast<int>(table.cells.size());
    if (table.autoResize && (static_cast<long long>(table.size) + table.deleted + 1) * 10 > capacity * 7LL) {
        // При большом числе удалённых ячеек достаточно очистки без роста.
        bool grow = (static_cast<long long>(table.size) + 1) * 10 > capacity * 7LL;
        if (grow && capacity > (std::numeric_limits<int>::max() - 1) / 2)
            throw std::runtime_error("слишком большая таблица");
        rebuild(table, grow ? capacity * 2 + 1 : capacity);
        capacity = static_cast<int>(table.cells.size());
    }
    int free = -1;
    for (int i = 0; i < capacity; ++i) {
        int index = position(table, key, i);
        ++table.probes;
        if (table.cells[index].state != 1 && free < 0) free = index;
        if (table.cells[index].state == 0) break;
    }
    if (free < 0) throw std::runtime_error("хеш-таблица заполнена");
    if (table.cells[free].state == 2) --table.deleted;
    table.cells[free] = HashCell{key, value, 1};
    ++table.size;
}

bool hashGet(HashTable& table, int key, int& value) {
    int index = find(table, key);
    if (index < 0) return false;
    value = table.cells[index].value;
    return true;
}

bool hashDel(HashTable& table, int key) {
    int index = find(table, key);
    if (index < 0) return false;
    table.cells[index].state = 2;
    --table.size; ++table.deleted;
    return true;
}

std::string compareHashing() {
    std::ostringstream output;
    output << "Линейное: шаг 1, просто, но возникают скопления ячеек.\n"
           << "Двойное: шаг зависит от ключа, меньше скоплений, сложнее хеширование.\n"
           << "Таблица: 1009 ячеек, свёртка включена; одинаковые ключи i*7919.\n"
           << "Проба — просмотр ячейки; вставка включает проверку наличия ключа.\n"
           << "load method insert_us find_us erase_us probes_insert probes_find probes_erase\n";
    for (int load = 25; load <= 90; load += (load == 75 ? 15 : 25)) {
        for (int method = 0; method < 2; ++method) {
            HashTable table;
            hashInit(table, 1009, method == 1);
            table.autoResize = false; // Размер одинаков даже при загрузке 90%.
            int count = 1009 * load / 100;
            auto start = std::chrono::steady_clock::now();
            for (int i = 0; i < count; ++i) hashSet(table, i * 7919, i);
            auto inserted = std::chrono::steady_clock::now();
            unsigned long long insertProbes = table.probes;
            table.probes = 0;
            for (int i = 0; i < count; ++i) {
                int value;
                if (!hashGet(table, i * 7919, value) || value != i) throw std::runtime_error("ошибка поиска");
            }
            auto searched = std::chrono::steady_clock::now();
            unsigned long long searchProbes = table.probes;
            table.probes = 0;
            for (int i = 0; i < count; ++i) hashDel(table, i * 7919);
            auto erased = std::chrono::steady_clock::now();
            output << load << "% " << (method ? "DOUBLE " : "LINEAR ") << std::fixed << std::setprecision(2)
                   << std::chrono::duration<double, std::micro>(inserted - start).count() << ' '
                   << std::chrono::duration<double, std::micro>(searched - inserted).count() << ' '
                   << std::chrono::duration<double, std::micro>(erased - searched).count() << ' '
                   << insertProbes << ' ' << searchProbes << ' ' << table.probes << '\n';
        }
    }
    return output.str();
}
