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
#include <tuple>

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
        const auto half_prod = binprod(a, b / 2, module);
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
 * @brief Расширенный алгоритм Евклида.
 * @param a Первое из чисел, для которых нужно найти НОД
 * @param b Второе из чисел, для которых нужно найти НОД
 * @return (НОД(a, b), x, y) такие, что a * x + b * y = НОД(a, b)
 */
std::tuple<uint64_t, int64_t, int64_t> continued_division(uint64_t a, uint64_t b) {
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

std::vector<uint64_t> factorize_eratosthenes(uint64_t number) {
    if (number < 2) {
        return {};
    }
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
    while (number > 1) {
        result.push_back(least_prime[number]);
        number /= least_prime[number];
    }
    return result;
}

std::vector<uint64_t> factorize_exhaustion(uint64_t number) {
    std::vector<uint64_t> result;

    if (number < 2) {
        return {};
    }

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

uint64_t find_divisor_pollard_p_1(uint64_t n) {
    // параметры алгоритма, существенно влияют на производительность и качество поиска
    const uint64_t b = 13;
    const uint64_t q[] = {2, 3, 5, 7, 11, 13};

    // несколько попыток алгоритма
    uint64_t a = 5 % n;
    for (int j = 0; j < 10; j++) {

        // ищем такое a, которое взаимно просто с n
        while (std::get<0>(continued_division(a, n)) != 1) {
            a = binpow(a, 2, n);
            a += 3;
            a %= n;
        }

        // вычисляем a^M
        for (auto qq : q) {
            auto e = static_cast<uint64_t>(floor(log((double) b) / log((double) qq)));
            uint64_t aa = binpow(a, binpow(qq, e, n), n);
            if (aa == 0)
                continue;

            // проверяем, не найден ли ответ
            auto g = std::get<0>(continued_division(aa - 1, n));
            if (1 < g && g < n)
                return g;
        }

    }

    // если ничего не нашли
    return 1;

}

uint64_t find_divisor_pollard_rho(uint64_t n, uint64_t iterations_count = 100000) {
    uint64_t a0 = rand() % n;
    uint64_t a1 = a0;
    a1 = binpow(a1, 2, n);
    if (++a1 == n)
        a1 = 0;
    auto g = std::get<0>(continued_division((a1 >= a0 ? a1 - a0 : a0 - a1), n));
    for (unsigned count = 0; count < iterations_count && (g == 1 || g == n); count++) {
        a0 = binpow(a0, 2, n);
        if (++a0 == n)
            a0 = 0;
        a1 = binpow(a1, 2, n);
        if (++a1 == n)
            a1 = 0;
        a1 = binpow(a1, 2, n);
        if (++a1 == n)
            a1 = 0;
        g = std::get<0>(continued_division((a1 >= a0 ? a1 - a0 : a0 - a1), n));
    }
    return g;
}

uint64_t find_divisor_ferma (const uint64_t n)
{
    auto x = static_cast<uint64_t>(sqrt(n));
    uint64_t y = 0;
    uint64_t r = x*x - y*y - n;
    for (;;)
        if (r == 0)
            return x!=y ? x-y : x+y;
        else
        if (r > 0)
        {
            r -= y+y+1;
            ++y;
        }
        else
        {
            r += x+x+1;
            ++x;
        }
}


void factorize(const uint64_t &n, std::vector<uint64_t> &result) {
    if (n == 1)
        return;

    // проверяем, не простое ли число
    if (is_prime (n)) {
        result.push_back(n);
        return;
    }

    // если число достаточно маленькое, то его разлагаем простым перебором
    if (n < 1000 * 1000) {
        auto factors = factorize_eratosthenes(n);
        result.insert(result.end(), factors.begin(), factors.end());
        return;
    } else {
        // число большое, запускаем на нем алгоритмы факторизации
        uint64_t divisor;
        divisor = find_divisor_pollard_p_1(n);
        if (divisor == 1) {
            divisor = find_divisor_pollard_rho(n);
        }
        /*if (divisor == 1) {
            divisor = find_divisor_ferma(n);
        }*/
        if (divisor == 1) {
            result.push_back(n);
            return;
        }
        /*if (divisor==1)
        {
            auto factors = factorize_eratosthenes(n);
            result.insert(result.end(), factors.begin(), factors.end());
            return;
        }*/
        // сначала идут быстрые алгоритмы Полларда
        /*div = pollard_monte_carlo (n);
        if (div == 1)
            div = pollard_bent (n);
        // придётся запускать 100%-ый алгоритм Ферма
        if (div == 1)
            div = ferma (n, unused);*/
        // рекурсивно обрабатываем найденные множители
        factorize(divisor, result);
        factorize(n / divisor, result);
    }
}


// Начало тестов

void test_from_task_1() {
    uint64_t n = 6;
    std::vector<uint64_t> factors;
    factorize(n, factors);
    assert(factors.size() == 2);
    uint64_t multiplication = 1;
    for (auto factor: factors) {
        multiplication *= factor;
    }
    assert(multiplication == n);
}

void test_from_task_2() {
    uint64_t n = 7;
    std::vector<uint64_t> factors;
    factorize(n, factors);
    assert(factors.size() == 1);
    uint64_t multiplication = 1;
    for (auto factor: factors) {
        multiplication *= factor;
    }
    assert(multiplication == n);
}

void test_big() {
    uint64_t n = 50000000;
    std::vector<uint64_t> factors;
    factorize(n, factors);
    assert(factors.size() == factorize_eratosthenes(n).size());
    uint64_t multiplication = 1;
    for (auto factor: factors) {
        multiplication *= factor;
    }
    assert(multiplication == n);
}

void test_very_big() {
    uint64_t n = 40724068285812;
    std::vector<uint64_t> factors;
    factorize(n, factors);
    uint64_t multiplication = 1;
    for (auto factor: factors) {
        multiplication *= factor;
    }
    assert(multiplication == n);
}

void test_small() {
    uint64_t n = 2;
    std::vector<uint64_t> factors;
    factorize(n, factors);
    assert(factors.size() == 1);
    assert(factors[0] == 2);
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
    test_big();
    test_binprod();
    test_binpow();
    test_very_big();
    test_small();
}

// Конец тестов

int main(int argc, char *argv[]) {
    run_all_tests();
    return 0;
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
    size_t n{0};
    std::cin >> n;
    std::vector<uint64_t> factors;

    factorize(n, factors);
    std::sort(factors.begin(), factors.end());

    // Запись результата
    for (const auto factor: factors) {
        std::cout << factor << " ";
    }

    return 0;
}