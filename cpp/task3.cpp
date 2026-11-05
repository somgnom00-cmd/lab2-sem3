#include "task3.h"
#include <sstream>
#include <stdexcept>

static bool distribute(const Array& values, int index, int target, Array& sums, Array& groups) {
    if (index == values.size) return true;
    int value = std::stoi(arrayGet(values, index));
    for (int group = 0; group < sums.size; ++group) {
        int sum = std::stoi(arrayGet(sums, group));
        if (value > target - sum) continue;
        bool repeated = false;
        for (int j = 0; j < group; ++j)
            if (arrayGet(sums, j) == arrayGet(sums, group)) repeated = true;
        if (repeated) continue;
        arraySet(sums, group, std::to_string(sum + value));
        arraySet(groups, index, std::to_string(group));
        if (distribute(values, index + 1, target, sums, groups)) return true;
        arraySet(sums, group, std::to_string(sum));
        // Все пустые группы равнозначны: достаточно проверить одну.
        if (sum == 0) break;
    }
    return false;
}

std::string partitionSet(const Set& set, int target) {
    if (target <= 0 || set.size == 0) throw std::runtime_error("нужны непустое множество и положительная сумма");
    Array values, groups, sums;
    try {
        setValues(set, values);
        long long total = 0;
        for (int i = 0; i < values.size; ++i) {
            int value = std::stoi(arrayGet(values, i));
            if (value <= 0) throw std::runtime_error("нужны натуральные числа");
            total += value;
            arrayPush(groups, "0");
            // Сортировка вставками по убыванию сокращает перебор.
            int j = i;
            while (j > 0 && std::stoi(arrayGet(values, j - 1)) < value) {
                arraySet(values, j, arrayGet(values, j - 1));
                --j;
            }
            arraySet(values, j, std::to_string(value));
        }
        long long count = total / target;
        if (total % target != 0 || count < 1 || count > values.size ||
            std::stoi(arrayGet(values, 0)) > target)
            throw std::runtime_error("множество нельзя разбить с указанной суммой");
        for (int i = 0; i < count; ++i) arrayPush(sums, "0");
        if (!distribute(values, 0, target, sums, groups))
            throw std::runtime_error("множество нельзя разбить с указанной суммой");
        std::ostringstream output;
        for (int group = 0; group < sums.size; ++group) {
            output << '{';
            bool first = true;
            for (int i = 0; i < values.size; ++i)
                if (std::stoi(arrayGet(groups, i)) == group) {
                    if (!first) output << ", ";
                    output << arrayGet(values, i);
                    first = false;
                }
            output << "}\n";
        }
        clearArray(values); clearArray(groups); clearArray(sums);
        return output.str();
    } catch (...) {
        clearArray(values); clearArray(groups); clearArray(sums);
        throw;
    }
}
