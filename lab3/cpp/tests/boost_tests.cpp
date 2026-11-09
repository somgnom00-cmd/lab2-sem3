#define BOOST_TEST_MODULE Lab3ArrayAndLists
#include <boost/test/included/unit_test.hpp>

#include "structures.h"
#include <stdexcept>

BOOST_AUTO_TEST_CASE(array_growth_and_copy) {
    Array original;
    for (int i = 0; i < 12; ++i)
        original.pushBack(std::to_string(i));
    Array copied = original;
    copied.set(0, "changed");
    BOOST_TEST(original[0] == "0");
    BOOST_TEST(copied[11] == "11");
    BOOST_CHECK_THROW(copied.at(12), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(singly_list_reuses_empty_tail) {
    SinglyList list;
    list.pushBack("old");
    list.erase(0);
    list.pushBack("new");
    BOOST_TEST(list.size() == 1u);
    BOOST_TEST(list.at(0) == "new");
}

BOOST_AUTO_TEST_CASE(doubly_list_links_after_middle_removal) {
    DoublyList list;
    list.pushBack("a");
    list.pushBack("b");
    list.pushBack("c");
    list.erase(1);
    const auto reversed = list.reverseValues();
    BOOST_REQUIRE_EQUAL(reversed.size(), 2u);
    BOOST_TEST(reversed[0] == "c");
    BOOST_TEST(reversed[1] == "a");
}
