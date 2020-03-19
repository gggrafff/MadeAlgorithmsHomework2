/*
 */

#include <iostream>
#include <vector>
#include <ctime>
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

bool is_prime_ferma(uint64_t n) {
    for (auto i = 0; i < 10000; ++i) {
        uint64_t a = rand() % (n - 1) + 1;
        auto apn = binpow(a, n - 1, n);
        if (apn != 1) {
            return false;
        }
    }
    return true;
}

bool is_prime_miller_rabin(uint64_t n) {

    // сначала проверяем тривиальные случаи
    if (n == 2 || n == 3)
        return true;
    if (n < 2 || ((n & 1) == 0))
        return false;

    // разлагаем n-1 = q*2^p
    uint64_t n_1 = n - 1;
    uint64_t p = 0;
    while ((n_1 & 1) == 0) {
        ++p;
        n_1 /= 2;
    }
    uint64_t q = n_1;
    assert((q & 1) == 1);

    uint16_t iterations_count = 15;

    for (auto i = 0; i < iterations_count; ++i) {
        uint64_t a = rand() % (n - 3) + 2;
        assert(a >= 2);
        assert(a <= n - 2);
        auto x = binpow(a, q, n);
        if (x == 1 || x == n - 1) {
            continue;
        }
        bool success = true;
        for (auto j = 0; j < p - 1; ++j) {
            x = binpow(x, 2, n);
            if (x == 1) {
                return false;
            }
            if (x == n - 1) {
                success = false;
                break;
            }
        }
        if (!success) {
            continue;
        }
        return false;
    }
    return true;
}

bool is_prime(uint64_t n) {
    if (n == 1) { return false; }

    bool ferma = is_prime_ferma(n);
    if (!ferma) {
        return false;
    }

    bool miller_rabin = is_prime_miller_rabin(n);
    return miller_rabin;
}

std::vector<bool> is_prime(std::vector<uint64_t> numbers) {
    std::vector<bool> result;
    result.reserve(numbers.size());
    for (auto number :numbers) {
        result.push_back(is_prime(number));
    }
    return result;
}

std::vector<uint64_t> get_primes(uint64_t number) {
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

    return primes;
}


// Начало тестов

void test_from_task() {
    std::vector<uint64_t> numbers{1, 5, 10, 239};
    auto result = is_prime(numbers);
    std::vector<bool> true_result{false, true, false, true};
    assert(result == true_result);
}

void test_primes() {
    std::vector<uint64_t> primes = get_primes(100000);
    auto result = is_prime(primes);
    std::vector<bool> true_result(result.size(), true);
    assert(result == true_result);
}

void test_non_primes() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> numbers_generator_small(2, 100000000);
    std::vector<uint64_t> numbers;
    std::vector<uint64_t> m1;
    std::vector<uint64_t> m2;
    for (auto i = 0; i < 10000; ++i) {
        m1.push_back(numbers_generator_small(gen));
        m2.push_back(numbers_generator_small(gen));
        uint64_t n = m1.back() * m2.back();
        numbers.push_back(n);
    }
    auto results = is_prime(numbers);
    std::vector<bool> true_results(results.size(), false);
    assert(results == true_results);
}

void test_bad_numbers()
{
    /*std::vector<uint64_t> numbers {893675219};
    auto results = is_prime(numbers);
    assert(results[0]==false);*/
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
    test_from_task();
    test_binprod();
    test_binpow();
    test_bad_numbers();
    test_primes();
    test_non_primes();
}

// Конец тестов

int main(int argc, char *argv[]) {
    srand(time(nullptr)); // автоматическая рандомизация
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
    size_t n{0};
    std::cin >> n;
    std::vector<uint64_t> numbers;
    numbers.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        uint64_t number{0};
        std::cin >> number;
        numbers.push_back(number);
    }

    auto results = is_prime(numbers);

    // Запись результата
    for (auto result: results) {
        if (result) {
            std::cout << "YES";
        } else {
            std::cout << "NO";
        }
        std::cout << "\n";
    }

    return 0;
}