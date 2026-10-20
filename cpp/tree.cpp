#include "tree.h"
#include <queue>

// Удаление узлов снизу вверх.
void destroyTreeNodes(TreeNode* node) {
    if (!node) return;

    destroyTreeNodes(node->left);
    destroyTreeNodes(node->right);
    delete node;
}

// Обход по уровням для вывода и сохранения дерева.
std::vector<std::string> treeValues(const SearchTree& tree) {
    std::vector<std::string> result;
    std::queue<TreeNode*> pending;

    if (tree.root) pending.push(tree.root);

    while (!pending.empty()) {
        TreeNode* current = pending.front();
        pending.pop();

        result.push_back(current->value);

        if (current->left) pending.push(current->left);
        if (current->right) pending.push(current->right);
    }

    return result;
}

void clearTree(SearchTree& tree) {
    destroyTreeNodes(tree.root);
    tree.root = nullptr;
}

// Меньшие значения помещаю слева, большие справа.
// Повторное значение не добавляю.
void treeSearchInsert(SearchTree& tree, int value) {
    TreeNode** place = &tree.root;

    while (*place) {
        int stored = std::stoi((*place)->value);

        if (stored == value) return;

        place = value < stored
            ? &(*place)->left
            : &(*place)->right;
    }

    *place = new TreeNode{std::to_string(value)};
}

bool treeSearchFind(const SearchTree& tree, int value) {
    const TreeNode* current = tree.root;

    while (current) {
        int stored = std::stoi(current->value);

        if (stored == value) return true;

        current = value < stored
            ? current->left
            : current->right;
    }

    return false;
}

static void collectRightView(
    const TreeNode* node,
    int depth,
    std::vector<std::string>& result
) {
    if (!node) return;

    // Первый посещённый справа узел каждого уровня.
    if (depth == static_cast<int>(result.size())) {
        result.push_back(node->value);
    }

    collectRightView(node->right, depth + 1, result);
    collectRightView(node->left, depth + 1, result);
}

std::vector<std::string> treeRightView(const SearchTree& tree) {
    std::vector<std::string> result;
    collectRightView(tree.root, 0, result);
    return result;
}