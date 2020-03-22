/*
 * D. Проверка на простоту
 * ограничение по времени на тест 1 секунда
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 * Дано n натуральных чисел ai. Определите для каждого числа, является ли оно простым.
 *
 * Входные данные
 * Программа получает на вход число n, 1 ≤ n ≤ 1000 и далее n чисел ai, 1 ≤ ai ≤ 10^18.
 * Выходные данные
 * Если число ai простое, программа должна вывести YES, для составного числа программа должна вывести NO.
 *
 * Пример
 * Входные данные
 * 4
 * 1
 * 5
 * 10
 * 239
 * Выходные данные
 * NO
 * YES
 * NO
 * YES
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

void test_carmichael() {
    std::vector<uint64_t> numbers{561, 1105, 1729, 2465, 2821, 6601, 8911, 41041, 62745, 63973, 75361, 101101, 126217,
                                  172081, 188461, 278545, 340561, 825265, 321197185, 5394826801, 232250619601,
                                  9746347772161};
    auto results = is_prime(numbers);
    std::vector<bool> true_results(results.size(), false);
    assert(results == true_results);
}

void test_carol() {
    std::vector<uint64_t> numbers{7, 47, 223, 3967, 16127, 1046527, 16769023, 1073676287, 68718952447, 274876858367,
                                  4398042316799, 1125899839733759, 18014398241046527};
    auto results = is_prime(numbers);
    std::vector<bool> true_results(results.size(), true);
    assert(results == true_results);
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
    test_jacobi();
    test_from_task();
    test_binprod();
    test_binpow();
    test_carmichael();
    test_primes();
    test_non_primes();
    test_carol();
}

// Конец тестов

int main(int argc, char *argv[]) {
    srand(time(nullptr)); // рандомизация
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