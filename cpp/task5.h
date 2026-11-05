#pragma once
#include "array.h"

struct BSTNode {
    int value;
    int id;
    BSTNode* left = nullptr;
    BSTNode* right = nullptr;
};

void bstRightView(const Array& values, Array& result);
