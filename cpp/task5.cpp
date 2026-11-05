#include "task5.h"
#include "queue.h"

static void insert(BSTNode*& root, int value, BSTNode** nodes, int& count) {
    BSTNode** place = &root;
    while (*place) {
        if (value == (*place)->value) return;
        place = value < (*place)->value ? &(*place)->left : &(*place)->right;
    }
    *place = new BSTNode{value, count};
    nodes[count++] = *place;
}

void bstRightView(const Array& values, Array& result) {
    clearArray(result);
    BSTNode* root = nullptr;
    BSTNode** nodes = new BSTNode*[values.size];
    int count = 0;
    Queue pending;
    try {
        for (int i = 0; i < values.size; ++i)
            insert(root, std::stoi(arrayGet(values, i)), nodes, count);
        // Очередь хранит строки, поэтому передаём номера, а не адреса узлов.
        if (root) queuePush(pending, std::to_string(root->id));
        while (pending.size > 0) {
            int levelSize = pending.size;
            for (int i = 0; i < levelSize; ++i) {
                BSTNode* node = nodes[std::stoi(queueGet(pending))];
                queuePop(pending);
                if (i == levelSize - 1) arrayPush(result, std::to_string(node->value));
                if (node->left) queuePush(pending, std::to_string(node->left->id));
                if (node->right) queuePush(pending, std::to_string(node->right->id));
            }
        }
        for (int i = 0; i < count; ++i) delete nodes[i];
        delete[] nodes;
    } catch (...) {
        clearQueue(pending);
        for (int i = 0; i < count; ++i) delete nodes[i];
        delete[] nodes;
        clearArray(result);
        throw;
    }
}
