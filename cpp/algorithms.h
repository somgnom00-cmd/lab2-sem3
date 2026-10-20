#pragma once
#include "database.h"

// Добавил вычисление выражений, разбиение множества и вывод подмножеств.
int calculate(const std::string& expression);
void printPartition(HashTable& set, int target);
void printSubsets(const Array& array);
void compareHashing(int count);
