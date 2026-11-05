#include "array.h"
#include <iostream>
#include <stdexcept>

void arrayCheckIndex(const Array& array, int index, bool insertion) {
    if (!(index >= 0 && (insertion ? index <= array.size : index < array.size))) throw std::runtime_error("индекс вне диапазона");
}

// Добавление в конец массива.
void arrayPush(Array& array, const std::string& value) {
    if (array.size == array.capacity) {
        int capacity = array.capacity ? array.capacity * 2 : 4;
        std::string* data = new std::string[capacity];

        for (int i = 0; i < array.size; ++i)
            data[i] = array.data[i];

        delete[] array.data;
        array.data = data;
        array.capacity = capacity;
    }

    array.data[array.size] = value;
    ++array.size;
}

void arrayInsert(Array& array, int index, const std::string& value) {
    arrayCheckIndex(array, index, true);
    arrayPush(array, value);

    for (int i = array.size - 1; i > index; --i)
        array.data[i] = array.data[i - 1];

    array.data[index] = value;
}

void arrayErase(Array& array, int index) {
    arrayCheckIndex(array, index);

    for (int i = index; i < array.size - 1; ++i)
        array.data[i] = array.data[i + 1];

    --array.size;
}

std::string arrayGet(const Array& array, int index) {
    arrayCheckIndex(array, index);
    return array.data[index];
}

void arraySet(Array& array, int index, const std::string& value) {
    arrayCheckIndex(array, index);
    array.data[index] = value;
}

int arrayFind(const Array& array, const std::string& value) {
    for (int i = 0; i < array.size; ++i)
        if (array.data[i] == value) return i;

    return -1;
}

int arrayValueIndex(const Array& array, const std::string& value) {
    int index = arrayFind(array, value);
    if (!(index >= 0)) throw std::runtime_error("значение не найдено");
    return index;
}

std::vector<std::string> arrayValues(const Array& array) {
    std::vector<std::string> result;

    for (int i = 0; i < array.size; ++i)
        result.push_back(array.data[i]);

    return result;
}

void clearArray(Array& array) {
    delete[] array.data;
    array.data = nullptr;
    array.size = array.capacity = 0;
}
