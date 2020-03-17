/*
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
    if (a < 1000000000ULL) {
        return binpow_small(a, b, module);
    } else {
        return binpow_large(a, b, module);
    }
}

/**
 * @brief Факторизация числа перебором делителей.
 * @param number Число, которое нужно факторизовать.
 * @return Массив простых делителей.
 */
std::vector<uint64_t> factorize(uint64_t number) {
    std::vector<uint64_t> result;

    for (uint64_t i = 2; i <= sqrt(number); i++) {
        while (number % i == 0) {
            result.push_back(i);
            number /= i;
        }
    }

    if (number != 1) {
        result.push_back(number);
    }

    return result;
}


/**
 * @brief Поиск минимального первообразного корня по модулю p.
 * @param p Модуль поля.
 * @return Минимальный генератор.
 */
uint64_t find_min_generator(const uint64_t p) {
  if (p==2) {
    return 1;
  }
  const auto phi = p - 1;
    auto factors = factorize(phi);  // факторизуем p-1

    for (uint64_t g = 2; g < p; ++g) {
        bool success = true;
        uint64_t current_factor = 0;
        for (auto factor : factors) {
            if (current_factor != factor)  // не проверяем дважды один и тот же делитель
            {
                const auto gpd = binpow(g, (p - 1) / factor, p);  // проверяем минимально необходимый набор делителей
                if (gpd == 1) {
                    success = false;  // g - не генератор
                }
                current_factor = factor;
            }
        }
        if (success) {
            return g;  // нашли генератор
        }
    }
    assert(false);
    return 0;
}

/**
 * @brief Поиск дискретного логарифма в конечном поле по модулю.
 * @param a Основание логарифма.
 * @param b Число под логарифмом.
 * @param n Модуль поля.
 * @return Значение логарифма, если оно существует.
 */
std::optional<uint64_t> discrete_logarithm(const uint64_t a, const uint64_t b, const uint64_t n) {
    if (a == b) {
      return 1;
    }
    if (b == 1) {
      return 0;
    }
    auto k = static_cast<uint64_t>(std::sqrt(n) + 1);
    auto ak = binpow(a, k, n);
    while (ak == 1) {
        ++k;
        ak = binpow(a, k, n);
        if (k>=n) {
          return std::nullopt;
        }
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

/**
 * @brief Поиск дискретного корня в конечном поле по модулю. x^a=b (mod n)
 * @param a Показатель степени.
 * @param b Значение возведения в степень.
 * @param n Модуль поля.
 * @return Значение логарифма, если оно существует.
 */
std::optional<uint64_t> discrete_sqrt(const uint64_t a, const uint64_t b, const uint64_t n) {
    assert(a != 0 || (a == 0 && b == 1));
    if (b == 0) {
        return 0;
    }
    auto g = find_min_generator(n);
    const auto gpa = binpow(g, a, n);
    auto y = discrete_logarithm(gpa, b, n);
    if (y.has_value()) {
      const auto x = binpow(g, y.value(), n);
        return x;
    }
    return std::nullopt;
}

// Начало тестов

void test_from_task() {
    uint64_t a = 3;
    uint64_t b = 4;
    uint64_t n = 5;
    auto result = discrete_sqrt(a, b, n);
    assert(result.has_value());
    assert(binpow(result.value(), a, n) == b);
    assert(result.value() < n);
    a = 3;
    b = 1;
    n = 7;
    result = discrete_sqrt(a, b, n);
    assert(result.has_value());
    assert(binpow(result.value(), a, n) == b);
    assert(result.value() < n);
}

void test_module7() {
  uint64_t a = 0;
  uint64_t b = 1;
  uint64_t n = 7;
  auto result = discrete_sqrt(a, b, n);
  assert(result.has_value());
  assert(binpow(result.value(), a, n) == b);
  assert(result.value() < n);
  a = 1; b = 3; n = 7;
  result = discrete_sqrt(a, b, n);
  assert(result.has_value());
  assert(binpow(result.value(), a, n) == b);
  assert(result.value() < n);
  a = 2; b = 2; n = 7;
  result = discrete_sqrt(a, b, n);
  assert(result.has_value());
  assert(binpow(result.value(), a, n) == b);
  assert(result.value() < n);
  a = 3; b = 6; n = 7;
  result = discrete_sqrt(a, b, n);
  assert(result.has_value());
  assert(binpow(result.value(), a, n) == b);
  assert(result.value() < n);
  a = 4; b = 4; n = 7;
  result = discrete_sqrt(a, b, n);
  assert(result.has_value());
  assert(binpow(result.value(), a, n) == b);
  assert(result.value() < n);
  a = 5; b = 5; n = 7;
  result = discrete_sqrt(a, b, n);
  assert(result.has_value());
  assert(binpow(result.value(), a, n) == b);
  assert(result.value() < n);
  a = 6; b = 1; n = 7;
  result = discrete_sqrt(a, b, n);
  assert(result.has_value());
  assert(binpow(result.value(), a, n) == b);
  assert(result.value() < n);
}

void test_module2() {
  uint64_t a = 1;
  uint64_t b = 1;
  uint64_t n = 2;
  auto result = discrete_sqrt(a, b, n);
  assert(result.has_value());
  assert(binpow(result.value(), a, n) == b);
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

void test_find_min_generator() {
  std::vector<uint64_t> modules { 2, 3, 5, 7, 11, 13 };
  std::vector<uint64_t> generators { 1, 2, 2, 3, 2, 2 };
  assert(modules.size() == generators.size());
  for (size_t i = 0; i< modules.size(); ++i) {
    auto g = find_min_generator(modules[i]);
    assert(g == generators[i]);
  }
}

void run_all_tests() {
    test_find_min_generator();
    test_from_task();
    test_module7();
    test_module2();
    test_binprod();
    test_binpow();
}

// Конец тестов

int main(int argc, char *argv[]) {
  //run_all_tests();
  //return 0;
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
    size_t T{0};
    std::cin >> T;
    std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> tests;
    tests.reserve(T);
    for (size_t i = 0; i < T; ++i) {
        uint64_t a, b, n{0};
        std::cin >> b >> a >> n;
        tests.emplace_back(a, b, n);
    }

    std::vector<std::optional<uint64_t>> results;
    results.reserve(T);
    for (const auto &test : tests) {
      const auto& [a, b, n] = test;
        results.push_back(discrete_sqrt(a, b, n));
    }

    // Запись результата
    for (const auto &result : results) {
        if (result.has_value()) {
            std::cout << result.value();
        } else {
            std::cout << -1;
        }
        std::cout << "\n";
    }

    return 0;
}