#include "task4.h"
#include <sstream>

static void combinations(const Array& values, int start, int needed,
                         Array& current, std::ostringstream& output, bool& first) {
    if (needed == 0) {
        if (!first) output << ", ";
        first = false;
        output << '{';
        for (int i = 0; i < current.size; ++i) {
            if (i) output << ", ";
            output << arrayGet(current, i);
        }
        output << '}';
        return;
    }
    for (int i = start; i <= values.size - needed; ++i) {
        // Равные элементы на одной глубине дают одинаковые подмножества.
        if (i > start && arrayGet(values, i) == arrayGet(values, i - 1)) continue;
        arrayPush(current, arrayGet(values, i));
        combinations(values, i + 1, needed - 1, current, output, first);
        arrayErase(current, current.size - 1);
    }
}

std::string allSubsets(const Array& values) {
    Array sorted, current;
    try {
        // Сортируем копию: исходный массив остаётся прежним.
        for (int i = 0; i < values.size; ++i) {
            std::string value = arrayGet(values, i);
            arrayPush(sorted, value);
            int j = i;
            while (j > 0 && arrayGet(sorted, j - 1) > value) {
                arraySet(sorted, j, arrayGet(sorted, j - 1));
                --j;
            }
            arraySet(sorted, j, value);
        }
        std::ostringstream output;
        output << '[';
        bool first = true;
        for (int size = 0; size <= sorted.size; ++size)
            combinations(sorted, 0, size, current, output, first);
        output << ']';
        clearArray(sorted); clearArray(current);
        return output.str();
    } catch (...) {
        clearArray(sorted); clearArray(current);
        throw;
    }
}
