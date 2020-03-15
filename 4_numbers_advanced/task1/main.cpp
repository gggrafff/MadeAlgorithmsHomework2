/*
 * A. Первообразный корень по простому модулю
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дано простое нечётное число p.
 * Требуется найти минимальный первообразный корень по модулю p.
 *
 * Входные данные
 * Первая строка входного файла содержит простое число p(3≤p≤10^9).
 * Выходные данные
 * Выведите ответ на поставленную задачу.
 *
 * Примеры
 * Входные данные
 * 3
 * Выходные данные
 * 2
 *
 * Входные данные
 * 239
 * Выходные данные
 * 7
 *
 * Входные данные
 * 127
 * Выходные данные
 * 3
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

/**
 * @brief Проверка числа на простоту перебором делителей.
 * @param x Проверяем число.
 * @return true, если простое, иначе - false.
 */
bool is_prime(uint64_t x) {
    for (int i = 2; i <= sqrt(x); i++) {
        if (x % i == 0) {
            return false;
        }
    }
    return true;
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
 * @brief Факторизация числа построением линейного решета Эратосфена. Требует много памяти.
 * @param number Число, которое нужно факторизовать.
 * @return Массив простых делителей.
 */
std::vector<uint64_t> factorize_eratosthenes(uint64_t number) {
    std::vector<uint64_t> primes;
    std::vector<uint64_t> least_prime(number + 1, 0);

    for (uint64_t i = 2; i <= number; ++i) {
        if (least_prime[i] == 0) {
            least_prime[i] = i;
            primes.push_back(i);
        }
        for (const auto prime: primes) {
            if ((prime > least_prime[i]) || (prime * i > number)) {
                break;
            }
            least_prime[prime * i] = prime;
        }
    }

    std::vector<uint64_t> result;
    while (number != 1) {
        result.push_back(least_prime[number]);
        number /= least_prime[number];
    }
    return result;
}

/**
 * @brief Бинарное возведение в степень по модулю.
 * @param a Основание степени.
 * @param n Показатель степени.
 * @param module Модуль поля.
 * @return Результат возведения в степень.
 */
uint64_t binpow(uint64_t a, uint64_t n, uint64_t module) {
    uint64_t res = 1;
    while (n) {
        if (n & 1) {
            res *= a;
            res %= module;
        }
        a *= a;
        a %= module;
        n >>= 1;
    }
    return res;
}

/**
 * @brief Поиск минимального первообразного корня по модулю p.
 * @param p Модуль поля.
 * @return Минимальный генератор.
 */
uint64_t find_min_generator(const uint64_t p) {
    auto factors = factorize(p - 1);  // факторизуем p-1

    for (uint64_t g = 2; g < p; ++g) {
        bool success = true;
        uint64_t current_factor = 0;
        for (auto factor: factors) {
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

// Начало тестов

void test_from_task() {
    auto result = find_min_generator(3);
    assert(result == 2);
    result = find_min_generator(239);
    assert(result == 7);
    result = find_min_generator(127);
    assert(result == 3);
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

    // Чтение входных данных
    uint64_t p{0};
    std::cin >> p;

    const auto g = find_min_generator(p);

    // Запись результата
    std::cout << g;

    return 0;
}