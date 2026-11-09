#include "hash_table.h"
#include "structures.h"

#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>

template <typename Action> double milliseconds(Action action) {
    const auto start = std::chrono::steady_clock::now();
    action();
    const auto finish = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(finish - start).count();
}

int main() try { // NOLINT(bugprone-exception-escape): ниже есть catch для всех исключений.
    constexpr int count = 5'000;
    volatile int guard = 0;
    std::cout << "LAB3 benchmark, variant 1\n";
    std::cout << "Operations per test: " << count << "\n\n";
    std::cout << std::fixed << std::setprecision(3);

    Array array;
    std::cout << "Array pushBack:     " << milliseconds([&] {
        for (int i = 0; i < count; ++i)
            array.pushBack(std::to_string(i));
    }) << " ms\n";
    std::cout << "Array indexed read: " << milliseconds([&] {
        for (int i = 0; i < count; ++i)
            guard += static_cast<int>(array.at(i).size());
    }) << " ms\n";

    SinglyList list;
    std::cout << "List pushBack:      " << milliseconds([&] {
        for (int i = 0; i < count; ++i)
            list.pushBack(std::to_string(i));
    }) << " ms\n";
    std::cout << "List sequential:    " << milliseconds([&] {
        for (const auto &value : list.values())
            guard += static_cast<int>(value.size());
    }) << " ms\n";

    LinearHashTable linear;
    std::cout << "Linear hash insert: " << milliseconds([&] {
        for (int i = 0; i < count; ++i)
            linear.set(i * 7919, i);
    }) << " ms\n";
    std::cout << "Linear hash search: " << milliseconds([&] {
        for (int i = 0; i < count; ++i) {
            int value = 0;
            if (linear.get(i * 7919, value))
                guard += value & 1;
        }
    }) << " ms\n";

    DoubleHashTable doubled;
    std::cout << "Double hash insert: " << milliseconds([&] {
        for (int i = 0; i < count; ++i)
            doubled.set(i * 7919, i);
    }) << " ms\n";
    std::cout << "Double hash search: " << milliseconds([&] {
        for (int i = 0; i < count; ++i) {
            int value = 0;
            if (doubled.get(i * 7919, value))
                guard += value & 1;
        }
    }) << " ms\n";

    std::cout << "\nGuard value: " << guard << '\n';
    return 0;
} catch (const std::exception &error) {
    std::cerr << "Benchmark error: " << error.what() << '\n';
    return 1;
} catch (...) {
    std::cerr << "Benchmark error: unknown exception\n";
    return 2;
}
