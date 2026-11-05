#pragma once
#include <string>
#include <vector>

struct Array {
    std::string* data = nullptr;
    int capacity = 0;
    int size = 0;
};

void arrayPush(Array& array, const std::string& value);
void arrayInsert(Array& array, int index, const std::string& value);
void arrayErase(Array& array, int index);
std::string arrayGet(const Array& array, int index);
void arraySet(Array& array, int index, const std::string& value);
void arrayCheckIndex(const Array& array, int index, bool insertion = false);
int arrayFind(const Array& array, const std::string& value);
int arrayValueIndex(const Array& array, const std::string& value);
std::vector<std::string> arrayValues(const Array& array);
void clearArray(Array& array);
