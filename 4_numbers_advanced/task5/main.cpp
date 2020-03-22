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
    if ((a < 1000 * 1000) && (b < 1000 * 1000) && (module < 1000 * 1000)) {
        return binpow_small(a, b, module);
    } else {
        return binpow_large(a, b, module);
    }
}

/**
 * @brief Расширенный алгоритм Евклида.
 * @tparam NumberType Числовой тип данных.
 * @param a Первое из чисел, для которых нужно найти НОД.
 * @param b Второе из чисел, для которых нужно найти НОД.
 * @return (НОД(a, b), x, y) такие, что a * x + b * y = НОД(a, b).
 */
template<typename NumberType>
std::tuple<NumberType, int64_t, int64_t> continued_division(NumberType a, NumberType b) {
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
 * @brief Тест простоты Ферма.
 * @details Быстрый вероятностный тест. Работает не для всех чисел.
 * @param n Проверяемое на простоту число.
 * @param iterations_count Количество проверок. Чем больше, тем точнее результат.
 * @return Если ответ false - то число точно составное, иначе - может быть как простым, так и составным.
 */
bool is_prime_ferma(uint64_t n, uint16_t iterations_count = 10) {
    // Проверяем тривиальные случаи
    if (n == 2 || n == 3)
        return true;
    if (n < 2 || ((n & 1) == 0))
        return false;

    for (auto i = 0; i < iterations_count; ++i) {
        uint64_t a = rand() % (n - 1) + 1;
        auto apn = binpow(a, n - 1, n);
        if (apn != 1) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Тест простоты Миллера-Рабина.
 * @details Быстрый вероятностный тест. Работает не для всех чисел.
 * @details Описание: https://ru.wikipedia.org/wiki/Тест_Миллера_—_Рабина
 * @param n Проверяемое на простоту число.
 * @param iterations_count Количество раундов проверки. Чем больше, тем точнее результат.
 * @return Если ответ false - то число точно составное, иначе - может быть как простым, так и составным.
 */
bool is_prime_miller_rabin(uint64_t n, uint16_t iterations_count = 10) {
    // Проверяем тривиальные случаи
    if (n == 2 || n == 3)
        return true;
    if (n < 2 || ((n & 1) == 0))
        return false;

    // Раскладываем n-1 на произведение q * 2^p
    uint64_t n_1 = n - 1;
    uint64_t p = 0;
    while ((n_1 & 1) == 0) {
        ++p;
        n_1 /= 2;
    }
    uint64_t q = n_1;
    assert((q & 1) == 1);

    for (auto i = 0; i < iterations_count; ++i) {
        uint64_t a = rand() % (n - 3) + 2;  // Выбрать случайное целое число a в отрезке [2, n − 2]
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

/**
 * @brief Вычисление символа Якоби
 * @details Символ Якоби — теоретико-числовая функция двух аргументов, введённая К. Якоби в 1837 году. Является квадратичным характером в кольце вычетов.
 * @param a Целое число.
 * @param b Натуральное, нечётное, больше единицы.
 * @return Символ Якоби.
 */
int64_t symbol_jacobi(int64_t a, int64_t b) {
    auto gcd = std::get<0>(continued_division(a, b));
    if (gcd != 1) {
        return 0;
    }
    int64_t r = 1;

    if (a < 0) {
        a = -a;
        if ((b % 4) == 3) {
            r = -r;
        }
    }
    while (a != 0) {
        uint64_t t = 0;
        while ((a % 2) == 0) {
            t += 1;
            a /= 2;
        }
        if ((t % 2) == 1) {
            if (((b % 8) == 3) || ((b % 8) == 5)) {
                r = -r;
            }
        }

        if (((a % 4) == (b % 4)) && ((b % 4) == 3)) {
            r = -r;
        }

        auto c = a;
        a = b % c;
        b = c;
    }

    return r;
}

/**
 * @brief Тест простоты Соловея-Штрассена.
 * @details Быстрый вероятностный тест. Работает не для всех чисел.
 * @details Описание: https://ru.wikipedia.org/wiki/Тест_Соловея_—_Штрассена
 * @param n Проверяемое на простоту число.
 * @param iterations_count Количество проверок. Чем больше, тем точнее результат.
 * @return Если ответ false - то число точно составное, иначе - может быть как простым, так и составным.
 */
bool is_prime_solovey_shtrassen(uint64_t n, uint16_t iterations_count = 10) {
    // Проверяем тривиальные случаи
    if (n == 2 || n == 3)
        return true;
    if (n < 2 || ((n & 1) == 0))
        return false;

    for (auto i = 0; i < iterations_count; ++i) {
        const uint64_t a = rand() % (n - 2) + 2;  // Выбрать случайное целое число a в отрезке [2, n − 1]
        assert(a >= 2);
        assert(a <= n - 1);
        const auto gcd = std::get<0>(continued_division(a, n));
        if (gcd > 1) {
            return false;
        }
        const auto apn = binpow(a, (n - 1) / 2, n);
        const auto j = (n + symbol_jacobi(a, n)) % n;
        if (apn != j) {
            return false;
        }
    }
    return true;  // простое с вероятностью 1 - 2^(-iterations_count)
}


/**
 * @brief Проверка числа на простоту.
 * @details Набор вероятностных тестов.
 * @param n Проверяемое число.
 * @return False - если составное, иначе - псевдопростое.
 */
bool is_prime(uint64_t n) {
    // Проверяем тривиальные случаи
    if (n == 2 || n == 3)
        return true;
    if (n < 2 || ((n & 1) == 0))
        return false;

    bool ferma = is_prime_ferma(n, 2);
    if (!ferma) {
        return false;
    }

    bool miller_rabin = is_prime_miller_rabin(n, 3);
    if (!miller_rabin) {
        return false;
    }

    bool solovey_shtrassen = is_prime_solovey_shtrassen(n, 2);
    if (!solovey_shtrassen) {
        return false;
    }

    return true;
}

/**
 * @brief Проверка на простоту набора чисел.
 * @param numbers Проверяемые числа.
 * @return Массив результатов тестов. True - псевдопростое, false - составное.
 */
std::vector<bool> is_prime(std::vector<uint64_t> numbers) {
    std::vector<bool> result;
    result.reserve(numbers.size());
    for (auto number :numbers) {
        result.push_back(is_prime(number));
    }
    return result;
}

/**
 * @brief Получить массив простых чисел, меньших заданного.
 * @param number Верхняя граница, до которой нужно искать простые.
 * @return Массив простых чисел, которые меньше number.
 */
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

std::tuple<std::vector<uint64_t>, std::vector<uint64_t>> compute_sieve() {
    const uint64_t max_number = 1000 * 1000;
    std::vector<uint64_t> primes;
    std::vector<uint64_t> least_prime(max_number + 1, 0);

    for (uint64_t i = 2; i <= max_number; ++i) {
        if (least_prime[i] == 0) {
            least_prime[i] = i;
            primes.push_back(i);
        }
        for (const auto prime: primes) {
            if ((prime > least_prime[i]) || (prime * i > max_number)) {
                break;
            }
            least_prime[prime * i] = prime;
        }
    }
    return {primes, least_prime};
}

std::tuple<std::vector<uint64_t>, std::vector<uint64_t>> get_sieve() {
    static std::tuple<std::vector<uint64_t>, std::vector<uint64_t>> sieve = compute_sieve();
    return sieve;
}


std::vector<uint64_t> factorize_eratosthenes(uint64_t number) {
    if (number < 2) {
        return {};
    }
    assert(number <= 1000 * 1000);
    static auto sieve = get_sieve();
    static const auto& primes = std::get<0>(sieve);
    static const auto& least_prime = std::get<1>(sieve);

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

uint64_t find_divisor_pollard_p_1(uint64_t n, uint64_t iterations_count = 100) {
    // параметры алгоритма, существенно влияют на производительность и качество поиска
    const uint64_t b = 13;
    const uint64_t q[] = {2, 3, 5, 7, 11, 13};

    // несколько попыток алгоритма
    uint64_t a = 5 % n;
    for (int j = 0; j < iterations_count; j++) {

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

uint64_t find_divisor_pollard_rho(uint64_t n, uint64_t iterations_count = 100000, uint64_t increase = 1) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<uint64_t> numbers_generator(0, n);
    uint64_t a0 = numbers_generator(gen);
    uint64_t a1 = a0;
    a1 = binpow(a1, 2, n);
    a1 += increase;
    a1 %= n;
    auto g = std::get<0>(continued_division((a1 >= a0 ? a1 - a0 : a0 - a1), n));
    for (unsigned count = 0; count < iterations_count && (g == 1 || g == n); count++) {
        a0 = binpow(a0, 2, n);
        a0 += increase;
        a0 %= n;
        a1 = binpow(a1, 2, n);
        a1 += increase;
        a1 %= n;
        a1 = binpow(a1, 2, n);
        a1 += increase;
        a1 %= n;
        g = std::get<0>(continued_division((a1 >= a0 ? a1 - a0 : a0 - a1), n));
    }
    return g;
}

uint64_t find_divisor_ferma(const uint64_t n) {
    auto x = static_cast<uint64_t>(sqrt(n));
    uint64_t y = 0;
    uint64_t r = x * x - y * y - n;
    for (;;)
        if (r == 0)
            return x != y ? x - y : x + y;
        else if (r > 0) {
            r -= y + y + 1;
            ++y;
        } else {
            r += x + x + 1;
            ++x;
        }
}

uint64_t find_divisor_pollard_brent(uint64_t n, uint64_t iterations_count = 19) {
    uint64_t b0 = rand() % n;
    uint64_t b1 = (b0 * b0 + 2) % n;
    uint64_t a = b1;
    for (uint64_t iteration = 0, series_len = 1; iteration < iterations_count; iteration++, series_len *= 2) {
        uint64_t g = std::get<0>(continued_division(b1 - b0, n));
        for (uint64_t len = 0; len < series_len && (g == 1 && g == n); len++) {
            b1 = (b1 * b1 + 2) % n;
            g = std::get<0>(continued_division((b1 >= b0 ? b1 - b0 : b0 - b1), n));
        }
        b0 = a;
        a = b1;
        if (g != 1 && g != n)
            return g;
    }
    return 1;
}

uint64_t find_divisor_pollard_monte_carlo (uint64_t n, uint64_t m = 100)
{
    uint64_t b = rand() % (m-2) + 2;

    static std::vector<uint64_t> primes;
    static uint64_t m_max;
    if (primes.empty())
        primes.push_back (3);
    if (m_max < m)
    {
        m_max = m;
        for (uint64_t prime=5; prime<=m; ++++prime)
        {
            bool is_prime = true;
            for (uint64_t div : primes) {
                if (div*div > prime)
                    break;
                if (prime % div == 0)
                {
                    is_prime = false;
                    break;
                }
            }
            if (is_prime)
                primes.push_back (prime);
        }
    }

    uint64_t g = 1;
    for (size_t i=0; i<primes.size() && g==1; i++)
    {
        uint64_t cur = primes[i];
        while (cur <= n)
            cur *= primes[i];
        cur /= primes[i];
        b = binpow (b, cur, n);
        g = std::get<0>(continued_division((b-1>=1?b-1:1-b), n));
        if (g == n)
            g = 1;
    }

    return g;
}


void factorize(const uint64_t &n, std::vector<uint64_t> &result) {
    if (n == 1)
        return;

    // проверяем, не простое ли число
    if (is_prime(n)) {
        result.push_back(n);
        return;
    }

    // если число достаточно маленькое, то его разлагаем простым способом
    if (n < 1000 * 1000) {
        auto factors = factorize_eratosthenes(n);
        result.insert(result.end(), factors.begin(), factors.end());
        return;
    } else {
        // число большое, запускаем на нем алгоритмы факторизации
        uint64_t divisor{1};
        if (divisor == 1) {
            divisor = find_divisor_pollard_brent(n, 1000);
        }
        if (divisor == 1) {
            divisor = find_divisor_pollard_monte_carlo(n, 100);
        }
        if (divisor == 1) {
            find_divisor_pollard_p_1(n, 1000);
        }
        if (divisor == 1) {
            static std::array<uint64_t, 5> primes_small {1, 3, 5, 7, 11};
            for(auto& prime: primes_small)
            {
                divisor = find_divisor_pollard_rho(n, 1000000, prime);
                if (divisor != 1)
                {
                    break;
                }
            }
        }
        /*if (divisor == 1) {
            divisor = find_divisor_ferma(n);
        }*/
        /*if (divisor == 1) {
            result.push_back(n);
            return;
        }*/
        assert(divisor != 1);
        // рекурсивно обрабатываем найденные множители
        factorize(divisor, result);
        assert((n % divisor) == 0);
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

void test_factorize_big() {
    uint64_t n = 50000000;
    std::vector<uint64_t> factors;
    factorize(n, factors);
    std::vector<uint64_t> true_factors{2, 2, 2, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5};
    std::sort(factors.begin(), factors.end());
    assert(factors == true_factors);
}

void test_factorize_very_big() {
    uint64_t n = 40724068285812;
    std::vector<uint64_t> factors;
    factorize(n, factors);
    std::vector<uint64_t> true_factors{2, 2, 3, 43, 47, 1679204531};
    std::sort(factors.begin(), factors.end());
    assert(factors == true_factors);
}

void test_factorize_very_big_2() {
    uint64_t n = 9000000000000000000;
    std::vector<uint64_t> factors;
    factorize(n, factors);
    std::vector<uint64_t> true_factors{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 5, 5, 5, 5, 5, 5, 5,
                                       5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
    std::sort(factors.begin(), factors.end());
    assert(factors == true_factors);
}

void test_factorize_very_big_3() {
    uint64_t n = binpow(1073676287, 2, 9000000000000000000);
    std::vector<uint64_t> factors;
    factorize(n, factors);
    std::vector<uint64_t> true_factors{1073676287, 1073676287};
    std::sort(factors.begin(), factors.end());
    assert(factors == true_factors);
}

void test_factorize_middle() {
    uint64_t n = 8051;
    std::vector<uint64_t> factors;
    factorize(n, factors);
    std::vector<uint64_t> true_factors{83, 97};
    std::sort(factors.begin(), factors.end());
    assert(factors == true_factors);
}

void test_factorize_small() {
    uint64_t n = 2;
    std::vector<uint64_t> factors;
    factorize(n, factors);
    assert(factors.size() == 1);
    assert(factors[0] == 2);
}

void test_factorize_ferma_number() {
    uint64_t n = 4294967297;  // F5 = 2^(2^5) + 1
    std::vector<uint64_t> factors;
    factorize(n, factors);
    std::vector<uint64_t> true_factors{641, 6700417};
    std::sort(factors.begin(), factors.end());
    assert(factors == true_factors);
}


void test_is_prime_primes() {
    std::vector<uint64_t> primes = get_primes(100000);
    auto result = is_prime(primes);
    std::vector<bool> true_result(result.size(), true);
    assert(result == true_result);
}

void test_factorize_primes() {
    std::vector<uint64_t> primes = get_primes(100000);
    for (auto prime: primes) {
        std::vector<uint64_t> factors;
        factorize(prime, factors);
        assert(factors.size() == 1);
        assert(factors[0] == prime);
    }
}

void test_is_prime_non_primes() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<uint64_t> numbers_generator_small(2, 100000000);
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

void test_is_prime_carmichael() {
    std::vector<uint64_t> numbers{561, 1105, 1729, 2465, 2821, 6601, 8911, 41041, 62745, 63973, 75361, 101101, 126217,
                                  172081, 188461, 278545, 340561, 825265, 321197185, 5394826801, 232250619601,
                                  9746347772161};
    auto results = is_prime(numbers);
    std::vector<bool> true_results(results.size(), false);
    assert(results == true_results);
}

void test_factorize_carmichael() {
    std::vector<std::pair<uint64_t, std::vector<uint64_t>>> tests{
            {561,           {3,  11, 17}},
            {1105,          {5,  13, 17}},
            {1729,          {7,  13, 19}},
            {2465,          {5,  17, 29}},
            {2821,          {7,  13, 31}},
            {6601,          {7,  23, 41}},
            {8911,          {7,  19, 67}},
            {41041,         {7,  11, 13, 41}},
            {62745,         {3,  5,  47, 89}},
            {63973,         {7,  13, 19, 37}},
            {75361,         {11, 13, 17, 31}},
            {101101,        {7,  11, 13, 101}},
            {126217,        {7,  13, 19, 73}},
            {172081,        {7,  13, 31, 61}},
            {188461,        {7,  13, 19, 109}},
            {278545,        {5,  17, 29, 113}},
            {340561,        {13, 17, 23, 67}},
            {825265,        {5,  7,  17, 19, 73}},
            {321197185,     {5,  19, 23, 29, 37, 137}},
            {5394826801,    {7,  13, 17, 23, 31, 67, 73}},
            {232250619601,  {7,  11, 13, 17, 31, 37, 73, 163}},
            {9746347772161, {7,  11, 13, 17, 19, 31, 37, 41, 641}},
    };
    for (const auto &test: tests) {
        std::vector<uint64_t> factors;
        factorize(test.first, factors);
        std::sort(factors.begin(), factors.end());
        assert(factors == test.second);
    }
}

void test_is_prime_carol() {
    std::vector<uint64_t> numbers{7, 47, 223, 3967, 16127, 1046527, 16769023, 1073676287, 68718952447, 274876858367,
                                  4398042316799, 1125899839733759, 18014398241046527};
    auto results = is_prime(numbers);
    std::vector<bool> true_results(results.size(), true);
    assert(results == true_results);
}

void test_factorize_carol() {
    std::vector<uint64_t> numbers{7, 47, 223, 3967, 16127, 1046527, 16769023, 1073676287, 68718952447, 274876858367,
                                  4398042316799, 1125899839733759, 18014398241046527};
    for (auto number: numbers) {
        std::vector<uint64_t> factors;
        factorize(number, factors);
        assert(factors.size() == 1);
        assert(factors[0] == number);
    }
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
    result = binpow(701221682, 68718952446, 68718952447);
    assert(result == 1);
    result = binpow(73401560, 68718952446, 68718952447);
    assert(result == 1);
}

void test_jacobi() {
    auto result = symbol_jacobi(2, 15);
    assert(result == 1);
    result = symbol_jacobi(7, 15);
    assert(result == -1);
    result = symbol_jacobi(7, 17);
    assert(result == -1);
    result = symbol_jacobi(100, 111);
    assert(result == 1);
}


void run_all_tests() {
    test_from_task_1();
    test_from_task_2();
    test_factorize_big();
    test_binprod();
    test_binpow();
    test_factorize_very_big();
    test_factorize_small();
    test_is_prime_primes();
    test_is_prime_non_primes();
    test_is_prime_carmichael();
    test_is_prime_carol();
    test_jacobi();
    test_factorize_ferma_number();
    test_factorize_middle();
    test_factorize_primes();
    test_factorize_carmichael();
    test_factorize_carol();
    test_factorize_very_big_2();
    test_factorize_very_big_3();
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