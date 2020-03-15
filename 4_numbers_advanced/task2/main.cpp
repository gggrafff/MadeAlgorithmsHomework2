/*
 * B. Дискретное логарифмирование
 * ограничение по времени на тест 3 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Даны натуральные числа a, b, и простое число n.
 * Требуется найти дискретный логарифм b по основанию a по модулю n, то есть такое число x (0⩽x<n), что a^x≡b(modn).
 *
 * Входные данные
 * В первой строке заданы через пробел три целых числа a, b и n (0⩽a,b<n⩽10^12), Число n простое.
 * Выходные данные
 * В первой строке выведите −1, если дискретного логарифма не существует. Иначе следует вывести его значение.
 * Если ответ неоднозначен, разрешается выводить любой.
 *
 * Примеры
 * Входные данные
 * 2 4 7
 * Выходные данные
 * 2
 *
 * Входные данные
 * 41 145 239
 * Выходные данные
 * 116
 */

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <optional>
#include <random>

/**
 * @brief Бинарное умножение по модулю.
 * @details Рекурсивный вариант функции.
 * @param a Первый множитель.
 * @param b Второй множитель.
 * @param module Модуль поля.
 * @return Произведение по модулю.
 */
uint64_t binprod(uint64_t a, uint64_t b, uint64_t module) {
    if (a == 0 || b == 0)
        return 0;
    if (b % 2 == 1)
        return (binprod(a, b - 1, module) + a) % module;
    else {
        auto half_prod = binprod(a, b / 2, module);
        return (half_prod + half_prod) % module;
    }
}

/**
 * @brief Бинарное возведение в степень по модулю.
 * @details Для не очень больших чисел (квадрат которых вмещается в uint64_t).
 * @param a Основание степени.
 * @param b Показатель степени.
 * @param module Модуль поля.
 * @return Результат возведения в степень.
 */
uint64_t binpow_small(uint64_t a, uint64_t b, uint64_t module) {
    uint64_t res = 1;
    while (b > 0) {
        if (b & 1) {
            res *= a;
            res %= module;
            --b;
        }
        a *= a;
        a %= module;
        b >>= 1;
    }
    return res;
}

/**
 * @brief Бинарное возведение в степень по модулю.
 * @details Для больших чисел (квадрат которых не вмещается в uint64_t).
 * @details Работает медленнее, чем аналог для маленьких чисел.
 * @param a Основание степени.
 * @param b Показатель степени.
 * @param module Модуль поля.
 * @return Результат возведения в степень.
 */
uint64_t binpow_large(uint64_t a, uint64_t b, uint64_t module) {
    uint64_t res = 1;
    while (b > 0) {
        if (b & 1) {
            res = binprod(res, a, module);
            --b;
        }
        a = binprod(a, a, module);
        b >>= 1;
    }
    return res;
}

/**
 * @brief Бинарное возведение в степень по модулю.
 * @details Выбирает нужную функцию для возведения.
 * @param a Основание степени.
 * @param b Показатель степени.
 * @param module Модуль поля.
 * @return Результат возведения в степень.
 */
uint64_t binpow(uint64_t a, uint64_t b, uint64_t module) {
    if (a<1000000000ULL)
    {
        return binpow_small(a, b, module);
    }else
    {
        return binpow_large(a, b, module);
    }
}

/**
 * @brief Поиск дискретного логарифма в конечном поле по модулю.
 * @param a Основание логарифма.
 * @param b Число под логарифмом.
 * @param n Модуль поля.
 * @return Значение логарифма, если оно существует.
 */
std::optional<uint64_t> discrete_logarithm(const uint64_t a, const uint64_t b, const uint64_t n) {
    auto k = static_cast<uint64_t>(std::sqrt(n) + 1);
    auto ak = binpow(a, k, n);
    while (ak == 1) {
        ++k;
        ak = binpow(a, k, n);
    }
    uint64_t current_aik = 1;
    std::map<uint64_t, size_t> apk;
    for (size_t r = 1; r <= k; ++r) {
        current_aik = binprod(current_aik, ak, n);
        apk[current_aik] = r;
    }
    uint64_t current_bai = b;
    for (size_t s = 1; s <= k; ++s) {
        current_bai = binprod(current_bai, a, n);
        auto r_iter = apk.find(current_bai);
        if (r_iter != apk.end()) {
            const auto r = r_iter->second;
            const auto x = r * k - s;
            if (x < n) {
                return x;
            }
        }
    }
    return std::nullopt;
}

// Начало тестов

void test_from_task_1() {
    uint64_t a = 2;
    uint64_t b = 4;
    uint64_t n = 7;
    auto result = discrete_logarithm(a, b, n);
    assert(result.has_value());
    assert(binpow(a, result.value(), n) == b);
    assert(result.value() < n);
}

void test_from_task_2() {
    uint64_t a = 41;
    uint64_t b = 145;
    uint64_t n = 239;
    auto result = discrete_logarithm(a, b, n);
    assert(result.has_value());
    assert(binpow(a, result.value(), n) == b);
    assert(result.value() < n);
}

void test_binprod() {
    auto result = binprod(0, 1, 7);
    assert(result == 0);
    result = binprod(0, 4, 7);
    assert(result == 0);
    result = binprod(4, 0, 7);
    assert(result == 0);
    result = binprod(1, 0, 7);
    assert(result == 0);
    result = binprod(15, 15, 21);
    assert(result == 15);
    result = binprod(583, 49, 935);
    assert(result == 517);
    result = binprod(91, 37, 113);
    assert(result == 90);
    result = binprod(9999999999, 100000000001, 99999999999);
    assert(result == 19999999998);
}

void test_binpow() {
    auto result = binpow(0, 1, 7);
    assert(result == 0);
    result = binpow(0, 4, 7);
    assert(result == 0);
    result = binpow(4, 0, 7);
    assert(result == 1);
    result = binpow(1, 0, 7);
    assert(result == 1);
    result = binpow(15, 15, 21);
    assert(result == 15);
    result = binpow(583, 49, 935);
    assert(result == 583);
    result = binpow(91, 37, 113);
    assert(result == 61);
    result = binpow(99999999999, 100000000001, 999999999999);
    assert(result == 407240682858);
}

void run_all_tests() {
    test_from_task_1();
    test_from_task_2();
    test_binprod();
    test_binpow();
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
    uint64_t a, b, n{0};
    std::cin >> a >> b >> n;

    auto result = discrete_logarithm(a, b, n);

    // Запись результата
    if (result.has_value()) {
        std::cout << result.value();
    } else {
        std::cout << -1;
    }

    return 0;
}