#include <gtest/gtest.h>

#include "structures.h"
#include <stdexcept>

TEST(Stack, PopsInReverseOrderAndRejectsEmpty) {
    Stack stack;
    stack.push("one");
    stack.push("two");
    EXPECT_EQ(stack.pop(), "two");
    EXPECT_EQ(stack.pop(), "one");
    EXPECT_THROW(stack.pop(), std::out_of_range);
}

TEST(Queue, PreservesOrderAndAcceptsPushAfterDrain) {
    Queue queue;
    queue.push("one");
    queue.push("two");
    EXPECT_EQ(queue.pop(), "one");
    EXPECT_EQ(queue.pop(), "two");
    queue.push("three");
    EXPECT_EQ(queue.front(), "three");
    EXPECT_EQ(queue.size(), 1u);
}

TEST(CompleteBinaryTree, FillsLastLevelFromTheLeft) {
    CompleteBinaryTree tree;
    const std::vector<std::string> expected{"root", "left", "right", "left-left"};
    for (const auto &value : expected)
        tree.insert(value);
    EXPECT_TRUE(tree.isComplete());
    EXPECT_EQ(tree.values(), expected);
    EXPECT_TRUE(tree.contains("left-left"));
    EXPECT_FALSE(tree.contains("missing"));
}
