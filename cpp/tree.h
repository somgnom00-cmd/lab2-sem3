#pragma once
#include <vector>
#include <string>

// Оставил только бинарное дерево поиска.
struct TreeNode {
    std::string value;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};

struct SearchTree {
    TreeNode* root = nullptr;
};

void treeSearchInsert(SearchTree& tree, int value);
bool treeSearchFind(const SearchTree& tree, int value);
std::vector<std::string> treeValues(const SearchTree& tree);
std::vector<std::string> treeRightView(const SearchTree& tree);
void clearTree(SearchTree& tree);