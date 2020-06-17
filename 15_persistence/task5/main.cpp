/*
 *
 */

/*
 * Подробнее о :
 *
 */

#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <math.h>
#include <array>
#include <utility>
#include <iomanip>
#include <optional>
#include <complex>
#include <string>
#include <numeric>
#include <cctype>
#include <utility>

// Начало тестов

void test_from_task() {
}

void run_all_tests() {
    test_from_task();
}

// Конец тестов

int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    if (argc > 1) {
        if (std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        }
    }

    // Решение задачи

    return 0;
}
