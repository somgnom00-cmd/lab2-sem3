#pragma once
#include <vector>
#include <string>

struct TreeNode {
    std::string value;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};

struct CompleteTree {
    TreeNode* root = nullptr;
};

void treeInsert(CompleteTree& tree, const std::string& value);
bool treeFind(const CompleteTree& tree, const std::string& value);
bool treeComplete(const CompleteTree& tree);
std::vector<std::string> treeValues(const CompleteTree& tree);
void clearTree(CompleteTree& tree);
