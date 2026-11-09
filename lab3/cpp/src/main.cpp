#include "database.h"
#include "hash_table.h"
#include "structures.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace {
void print(const Container &container) {
    std::cout << container.type() << " [";
    const auto stored = container.values();
    for (std::size_t i = 0; i < stored.size(); ++i) {
        if (i != 0)
            std::cout << ", ";
        std::cout << stored[i];
    }
    std::cout << "]\n";
}
} // namespace

int main() { // NOLINT(bugprone-exception-escape): ниже есть catch для всех исключений.
    try {
        std::cout << "LAB3, variant 1. C++ demo\n\n";

        Array array;
        array.pushBack("alpha");
        array.pushBack("gamma");
        array.insert(1, "beta");
        array.erase(0);
        print(array);

        SinglyList singly;
        singly.pushBack("one");
        singly.pushBack("three");
        singly.insert(1, "two");
        print(singly);

        DoublyList doubly;
        doubly.pushFront("center");
        doubly.pushFront("left");
        doubly.pushBack("right");
        print(doubly);

        Stack stack;
        stack.push("first");
        stack.push("second");
        std::cout << "Stack pop: " << stack.pop() << '\n';
        print(stack);

        Queue queue;
        queue.push("first");
        queue.push("second");
        std::cout << "Queue pop: " << queue.pop() << '\n';
        print(queue);

        CompleteBinaryTree tree;
        for (int value = 1; value <= 7; ++value)
            tree.insert(std::to_string(value));
        print(tree);
        std::cout << "Tree is complete: " << std::boolalpha << tree.isComplete() << '\n';

        LinearHashTable linear;
        DoubleHashTable doubled;
        for (int key : {1, 1001, 2001}) {
            linear.set(key, key * 10);
            doubled.set(key, key * 10);
        }
        print(linear);
        print(doubled);

        const auto textFile = std::filesystem::temp_directory_path() / "lab3_array.txt";
        const auto binaryFile = std::filesystem::temp_directory_path() / "lab3_array.bin";
        array.saveText(textFile);
        array.saveBinary(binaryFile);
        Array restored;
        restored.loadBinary(binaryFile);
        std::cout << "Restored from binary: ";
        print(restored);
        std::filesystem::remove(textFile);
        std::filesystem::remove(binaryFile);

        Database database;
        database.create("numbers", "Array");
        database.create("index", "DoubleHashTable");
        std::cout << "Polymorphic database objects: " << database.size() << '\n';
        std::cout << "Demo completed successfully.\n";
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Error: unknown exception\n";
        return 2;
    }
}
