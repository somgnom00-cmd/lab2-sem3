#pragma once
#include "singly.h"
#include "array.h"
#include <vector>
#include <string>

struct Set {
    std::vector<SinglyList> buckets = std::vector<SinglyList>(17);
    int size = 0;
};

bool setAdd(Set& set, int value);
bool setDel(Set& set, int value);
bool setAt(const Set& set, int value);
void setValues(const Set& set, Array& values);
void clearSet(Set& set);
std::string setFileQuery(const std::string& filename, const std::string& query);
