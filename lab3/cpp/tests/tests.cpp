#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <fstream>
#include <sstream>
#include <utility>

#include "database.h"
#include "hash_table.h"
#include "structures.h"

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

TEST_CASE("Array: insertion, deletion and deep copy") {
    Array array;
    array.pushBack("a");
    array.pushBack("c");
    array.insert(1, "b");
    CHECK(array.values() == std::vector<std::string>{"a", "b", "c"});
    CHECK(array.find("b") == 1);
    array.set(1, "B");
    array.erase(0);
    CHECK(array.values() == std::vector<std::string>{"B", "c"});

    Array copy = array;
    copy.set(0, "changed");
    CHECK(array.at(0) == "B");
    CHECK(copy.at(0) == "changed");
    CHECK_THROWS_AS(array.at(10), std::out_of_range);
}

TEST_CASE("SinglyList: head, tail and middle links") {
    SinglyList list;
    list.pushBack("b");
    list.pushFront("a");
    list.pushBack("d");
    list.insert(2, "c");
    CHECK(list.values() == std::vector<std::string>{"a", "b", "c", "d"});
    list.erase(0);
    list.erase(list.size() - 1);
    CHECK(list.values() == std::vector<std::string>{"b", "c"});
    CHECK(list.find("missing") == -1);
}

TEST_CASE("DoublyList: links in both directions") {
    DoublyList list;
    list.pushBack("a");
    list.pushBack("c");
    list.insert(1, "b");
    CHECK(list.values() == std::vector<std::string>{"a", "b", "c"});
    CHECK(list.reverseValues() == std::vector<std::string>{"c", "b", "a"});
    list.erase(1);
    CHECK(list.values() == std::vector<std::string>{"a", "c"});
}

TEST_CASE("Stack follows LIFO and reports an empty state") {
    Stack stack;
    CHECK_THROWS_AS(stack.top(), std::out_of_range);
    stack.push("first");
    stack.push("second");
    CHECK(stack.top() == "second");
    CHECK(stack.pop() == "second");
    CHECK(stack.pop() == "first");
    CHECK_THROWS_AS(stack.pop(), std::out_of_range);
}

TEST_CASE("Queue follows FIFO and resets both ends") {
    Queue queue;
    CHECK_THROWS_AS(queue.front(), std::out_of_range);
    queue.push("first");
    queue.push("second");
    CHECK(queue.front() == "first");
    CHECK(queue.pop() == "first");
    CHECK(queue.pop() == "second");
    CHECK(queue.size() == 0);
    queue.push("again");
    CHECK(queue.front() == "again");
}

TEST_CASE("CompleteBinaryTree preserves level order") {
    CompleteBinaryTree tree;
    for (int i = 1; i <= 10; ++i)
        tree.insert(std::to_string(i));
    CHECK(tree.size() == 10);
    CHECK(tree.isComplete());
    CHECK(tree.contains("7"));
    CHECK_FALSE(tree.contains("77"));
    CHECK(tree.values() ==
          std::vector<std::string>{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"});
}

TEST_CASE_TEMPLATE("Hash tables handle collisions, updates and deleted cells", Table,
                   LinearHashTable, DoubleHashTable) {
    Table table(5);
    table.set(1, 10);
    table.set(1001, 20);
    table.set(2001, 30);
    int value = 0;
    REQUIRE(table.get(1001, value));
    CHECK(value == 20);
    table.set(1001, 25);
    CHECK(table.size() == 3);
    REQUIRE(table.get(1001, value));
    CHECK(value == 25);
    CHECK(table.erase(1));
    CHECK_FALSE(table.erase(1));
    CHECK(table.get(2001, value));
    CHECK(value == 30);
    table.set(-1, -10);
    CHECK(table.get(-1, value));
    CHECK(value == -10);
}

TEST_CASE_TEMPLATE("Text and binary serialization restore every container", T, Array, SinglyList,
                   DoublyList, Queue, CompleteBinaryTree) {
    T original;
    if constexpr (std::is_same_v<T, Array>) {
        original.pushBack("one");
        original.pushBack("two words");
    } else if constexpr (std::is_same_v<T, SinglyList> || std::is_same_v<T, DoublyList>) {
        original.pushBack("one");
        original.pushBack("two words");
    } else if constexpr (std::is_same_v<T, Queue>) {
        original.push("one");
        original.push("two words");
    } else {
        original.insert("one");
        original.insert("two words");
    }

    const auto base = std::filesystem::temp_directory_path() / (original.type() + "_lab3");
    const auto text = base.string() + ".txt";
    const auto binary = base.string() + ".bin";
    original.saveText(text);
    original.saveBinary(binary);
    T fromText;
    T fromBinary;
    fromText.loadText(text);
    fromBinary.loadBinary(binary);
    CHECK(fromText.values() == original.values());
    CHECK(fromBinary.values() == original.values());
    std::filesystem::remove(text);
    std::filesystem::remove(binary);
}

TEST_CASE("Stack serialization keeps top-to-bottom order") {
    Stack original;
    original.push("bottom");
    original.push("top");
    const auto file = std::filesystem::temp_directory_path() / "stack_lab3.bin";
    original.saveBinary(file);
    Stack restored;
    restored.loadBinary(file);
    CHECK(restored.values() == original.values());
    CHECK(restored.pop() == "top");
    std::filesystem::remove(file);
}

TEST_CASE_TEMPLATE("Hash serialization restores keys and values", Table, LinearHashTable,
                   DoubleHashTable) {
    Table original;
    original.set(7, 70);
    original.set(-8, 80);
    const auto file = std::filesystem::temp_directory_path() / (original.type() + "_lab3.txt");
    original.saveText(file);
    Table restored;
    restored.loadText(file);
    int value = 0;
    CHECK(restored.get(7, value));
    CHECK(value == 70);
    CHECK(restored.get(-8, value));
    CHECK(value == 80);
    std::filesystem::remove(file);
}

TEST_CASE("Serialization rejects a different container type") {
    Array array;
    array.pushBack("value");
    const auto file = std::filesystem::temp_directory_path() / "wrong_type_lab3.txt";
    array.saveText(file);
    Queue queue;
    CHECK_THROWS_AS(queue.loadText(file), std::runtime_error);
    std::filesystem::remove(file);
}

TEST_CASE("Database owns containers through virtual base pointers") {
    Database database;
    Container &container = database.create("data", "Array");
    CHECK(container.type() == "Array");
    CHECK(database.size() == 1);
    CHECK_THROWS_AS(database.create("data", "Queue"), std::invalid_argument);
    CHECK(database.erase("data"));
    CHECK_THROWS_AS(database.get("data"), std::out_of_range);
}

TEST_CASE("Array insertion supports a reference to one of its own elements") {
    Array a;
    for (int i = 0; i < 4; ++i)
        a.pushBack(std::string(128, static_cast<char>('a' + i)));
    a.pushBack(a.at(0));
    CHECK(a.at(4) == std::string(128, 'a'));
    a.insert(0, a.at(2));
    CHECK(a[0] == std::string(128, 'c'));
    CHECK(a.size() == 6);
}

TEST_CASE("Invalid record leaves the previous hash table untouched") {
    LinearHashTable table;
    table.set(7, 70);
    const auto file = std::filesystem::temp_directory_path() / "lab3_invalid_record.txt";
    {
        std::ofstream output(file);
        output << "LAB3TXT1\n\"LinearHashTable\"\n2\n\"1=10\"\n\"broken\"\n";
    }
    CHECK_THROWS_AS(table.loadText(file), std::runtime_error);
    int value = 0;
    CHECK(table.get(7, value));
    CHECK(value == 70);
    CHECK_FALSE(table.contains(1));
    std::filesystem::remove(file);
}

TEST_CASE("Database factory creates all container types and checks names") {
    Database db;
    for (const auto *type : {"Array", "SinglyList", "DoublyList", "Stack", "Queue",
                             "CompleteBinaryTree", "LinearHashTable", "DoubleHashTable"}) {
        CHECK(db.create(type, type).type() == type);
        CHECK(std::as_const(db).get(type).size() == 0);
    }
    CHECK(db.size() == 8);
    CHECK_THROWS_AS(db.create("", "Array"), std::invalid_argument);
    CHECK_THROWS_AS(db.create("x", "Unknown"), std::invalid_argument);
    CHECK_THROWS_AS(std::as_const(db).get("x"), std::out_of_range);
    CHECK_FALSE(db.erase("x"));
}

TEST_CASE("Corrupt file and I/O errors are reported") {
    Array array;
    array.pushBack("old");
    const auto file = std::filesystem::temp_directory_path() / "lab3_truncated.bin";
    const auto missing = file / "not_a_file";
    CHECK_THROWS_AS(array.loadBinary(missing), std::runtime_error);
    CHECK_THROWS_AS(array.loadText(missing), std::runtime_error);
    CHECK_THROWS_AS(array.saveBinary(missing), std::runtime_error);
    CHECK_THROWS_AS(array.saveText(missing), std::runtime_error);
    {
        std::ofstream output(file);
        output << "invalid";
    }
    CHECK_THROWS_AS(array.loadBinary(file), std::runtime_error);
    CHECK_THROWS_AS(array.loadText(file), std::runtime_error);
    array.saveBinary(file);
    std::filesystem::resize_file(file, std::filesystem::file_size(file) - 1);
    CHECK_THROWS_AS(array.loadBinary(file), std::runtime_error);
    CHECK(array[0] == "old");
    {
        std::ofstream output(file);
        output << "LAB3TXT1\n\"Array\"\n2\n\"only one\"\n";
    }
    CHECK_THROWS_AS(array.loadText(file), std::runtime_error);
    std::filesystem::remove(file);
}

TEST_CASE_TEMPLATE("Copy and move preserve container values and ownership", T, Array, SinglyList,
                   DoublyList, Stack, Queue, CompleteBinaryTree) {
    T empty;
    T a;
    const auto path = std::filesystem::temp_directory_path() / "lab3_copy_move.txt";
    {
        std::ofstream output(path);
        output << "LAB3TXT1\n\"" << a.type() << "\"\n2\n\"left\"\n\"right\"\n";
    }
    a.loadText(path);
    const auto values = a.values();
    T b(std::move(a));
    CHECK(a.size() == 0);
    CHECK(b.values() == values);
    empty = b;
    b.clear();
    CHECK(empty.values() == values);
    b = std::move(empty);
    CHECK(b.values() == values);
    CHECK(empty.size() == 0);
    std::filesystem::remove(path);
}

TEST_CASE("Indexed operations handle edges without damaging links") {
    Array a;
    a.insert(0, "a");
    a.set(0, "b");
    CHECK(a.find("b") == 0);
    CHECK(a.find("a") == -1);
    CHECK_THROWS_AS(a.set(1, ""), std::out_of_range);
    CHECK_THROWS_AS(a.erase(1), std::out_of_range);
    CHECK_THROWS_AS(a.insert(2, ""), std::out_of_range);
    SinglyList s;
    s.insert(0, "first");
    s.insert(1, "last");
    CHECK(s.at(1) == "last");
    CHECK(s.find("last") == 1);
    CHECK(s.find("missing") == -1);
    s.erase(0);
    s.erase(0);
    CHECK(s.values().empty());
    CHECK_THROWS_AS(s.at(0), std::out_of_range);
    CHECK_THROWS_AS(s.erase(0), std::out_of_range);
    CHECK_THROWS_AS(s.insert(1, ""), std::out_of_range);
    DoublyList d;
    d.insert(0, "a");
    d.insert(1, "c");
    d.insert(1, "b");
    CHECK(d.at(0) == "a");
    CHECK(d.at(2) == "c");
    CHECK(d.find("c") == 2);
    CHECK(d.find("missing") == -1);
    d.erase(0);
    d.erase(1);
    CHECK(d.reverseValues() == std::vector<std::string>{"b"});
    CHECK_THROWS_AS(d.erase(2), std::out_of_range);
    CHECK_THROWS_AS(d.insert(2, ""), std::out_of_range);
    Queue q;
    CHECK_THROWS_AS(q.front(), std::out_of_range);
    q.push("first");
    CHECK(q.front() == "first");
    Stack stack;
    CHECK_THROWS_AS(stack.top(), std::out_of_range);
    stack.push("top");
    CHECK(stack.top() == "top");
    CompleteBinaryTree t;
    CHECK(t.isComplete());
    CHECK_FALSE(t.contains("absent"));
}

TEST_CASE("Array stream output keeps insertion order") {
    Array a;
    a.pushBack("first");
    a.pushBack("second");
    std::ostringstream output;
    output << a;
    CHECK(output.str() == "[first, second]");
}

TEST_CASE("Hash table grows without losing earlier entries") {
    DoubleHashTable h(2);
    const auto originalCapacity = h.capacity();
    for (int i = -40; i <= 40; ++i)
        h.set(i, i * 3);
    CHECK(h.capacity() > originalCapacity);
    for (int i = -40; i <= 40; ++i) {
        int value = 0;
        REQUIRE(h.get(i, value));
        CHECK(value == i * 3);
    }
    for (int i = -40; i <= 40; i += 2)
        CHECK(h.erase(i));
    CHECK_FALSE(h.contains(-40));
    CHECK(h.contains(-39));
}
