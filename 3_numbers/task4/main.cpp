/*
 * D. Прямая
 * ограничение по времени на тест 1 second
 * ограничение по памяти на тест 256 megabytes
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Своим уравнением Ax + By + C = 0 задана прямая на плоскости.
 * Требуется найти любую принадлежащую этой прямой точку,
 * координаты которой — целые числа от  - 5·10^18 до 5·10^18 включительно, или выяснить что таких точек нет.
 *
 * Входные данные
 * В первой строке содержатся три целых числа
 * A, B и C ( - 2·10^9 ≤ A, B, C ≤ 2·10^9) — соответствующие коэффициенты уравнения прямой.
 * Гарантируется, что A^2 + B^2 > 0.
 * Выходные данные
 * Если искомая точка существует, выведите ее координаты, иначе выведите единственное число -1.
 *
 * Примеры
 * Входные данные
 * 2 5 3
 * Выходные данные
 * 6 -3
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <optional>
#include <tuple>

/**
 * @brief Точка прямой.
 */
using Point = std::pair<int64_t, int64_t>;

/**
 * @brief Расширенный алгоритм Евклида.
 * @param a Первое из чисел, для которых нужно найти НОД
 * @param b Второе из чисел, для которых нужно найти НОД
 * @return (НОД(a, b), x, y) такие, что a * x + b * y = НОД(a, b)
 */
std::tuple<int64_t, int64_t, int64_t> continued_division(int64_t a, int64_t b) {
    if (b == 0) {
        return {a, 1, 0};
    }
    int64_t x2 = 1;
    int64_t x1 = 0;
    int64_t y2 = 0;
    int64_t y1 = 1;
    while (b > 0) {
        const auto q = a / b;
        const auto r = a - q * b;
        const auto x = x2 - q * x1;
        const auto y = y2 - q * y1;
        a = b;
        b = r;
        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;
    }
    return {a, x2, y2};
}

/**
 * @brief Решение уравнения Ax + By + C = 0 в целых числах.
 * @param A Коэффициент прямой.
 * @param B Коэффициент прямой.
 * @param C Коэффициент прямой.
 * @return Если ерешние есть, то точка Point, иначе - nullopt.
 */
std::optional<Point> solve_diophantine_equation(int64_t A, int64_t B, int64_t C) {
    auto [g, x, y] = continued_division(std::abs(A), std::abs(B));
    if (C % g != 0) {
        return std::nullopt;  // Уравнение имеет либо бесконечно много произвольных решений, либо не имеет решений
    }
    // Находим решение
    x *= -C / g;
    y *= -C / g;
    // Восстанавливаем знаки
    if (A < 0) {
        x = -x;
    }
    if (B < 0) {
        y = -y;
    }
    return Point({x, y});
}

// Начало тестов

void test_from_task_1() {
    auto result = solve_diophantine_equation(2, 5, 3);
    assert(result.value().first == 6 && result.value().second == -3);
}

void run_all_tests() {
    test_from_task_1();
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

    // Чтение входных данных
    int64_t A{0}, B{0}, C{0};
    std::cin >> A >> B >> C;

    auto result = solve_diophantine_equation(A, B, C);

    // Запись результата
    if (result.has_value()) {
        std::cout << result.value().first << " " << result.value().second;
    } else {
        std::cout << -1;
    }

    return 0;
}