/*
 * C. Корень по модулю
 * ограничение по времени на тест 4 секунды
 * ограничение по памяти на тест 256 мегабайт ввод
 * стандартный ввод
 * вывод стандартный вывод

 * Даны числа a, b и m, (1≤a,b<m≤109), m — простое. Требуется найти корень степени b из a по модулю m.
 *
 * Входные данные
 * Входной файл состоит из одного или нескольких тестов. Количество тестов T≤300 задано в первой строке.
 * В T последующих строках заданы по три числа в каждой — a, b и m.
 * Выходные данные
 * Для каждого из тестов выведите какой-либо из корней степени b из a по модулю m, либо -1,
 * если искомого корня не существует. Выводимое число должно быть в пределах от -1 до m−1.
 *
 * Пример
 * Входные данные
 * 2
 * 4 3 5
 * 1 3 7
 * Выходные данные
 * 4
 * 1
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
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<uint64_t> numbers_generator(0, n);
    // Проверяем тривиальные случаи
    if (n == 2 || n == 3)
        return true;
    if (n < 2 || ((n & 1) == 0))
        return false;

    for (auto i = 0; i < iterations_count; ++i) {
        uint64_t a = numbers_generator(gen) % (n - 1) + 1;
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
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<uint64_t> numbers_generator(0, n);
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
        uint64_t a = numbers_generator(gen) % (n - 3) + 2;  // Выбрать случайное целое число a в отрезке [2, n − 2]
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
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<uint64_t> numbers_generator(0, n);
    // Проверяем тривиальные случаи
    if (n == 2 || n == 3)
        return true;
    if (n < 2 || ((n & 1) == 0))
        return false;

    for (auto i = 0; i < iterations_count; ++i) {
        const uint64_t a = numbers_generator(gen) % (n - 2) + 2;  // Выбрать случайное целое число a в отрезке [2, n − 1]
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

/**
 * @brief Расчёт решета Эратосфена до 1_000_000
 * @return Кортеж. Первый элемент - массив простых чисел до миллиона. Второй элемент - массив минимальных простых делителей для факторизации.
 */
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

/**
 * @brief Получить статически инициализированное решето.
 * @return Кортеж. Первый элемент - массив простых чисел до миллиона. Второй элемент - массив минимальных простых делителей для факторизации.
 */
std::tuple<std::vector<uint64_t>, std::vector<uint64_t>> get_sieve() {
    static std::tuple<std::vector<uint64_t>, std::vector<uint64_t>> sieve = compute_sieve();
    return sieve;
}

/**
 * @brief Факторизация с использованием решета Эратосфена
 * @param number Число, которое факторизуем.
 * @return Массив простых делителей по возрастанию.
 */
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


/**
 * @brief Поиск делителя числа Ро алгоритмом Полларда.
 * @param n Число, для которого ищем делитель.
 * @param iterations_count Количество итераций алгоритма.
 * @return Делитель. Если не найден, то 1.
 */
uint64_t find_divisor_pollard_rho(uint64_t n, uint64_t iterations_count = 100000, uint64_t increase = 1) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<uint64_t> numbers_generator(0, n);
    uint64_t a0 = numbers_generator(gen);
    uint64_t a1 = a0;
    a1 = binprod(a1, a1, n);
    a1 += increase;
    a1 %= n;
    uint64_t g = std::get<0>(continued_division((a1 >= a0 ? a1 - a0 : a0 - a1), n));
    for (uint64_t count = 0; count < iterations_count && g == 1; count++) {
        a0 = binprod(a0, a0, n);
        a0 += increase;
        a0 %= n;
        a1 = binprod(a1, a1, n);
        a1 += increase;
        a1 %= n;
        a1 = binprod(a1, a1, n);
        a1 += increase;
        a1 %= n;
        g = std::get<0>(continued_division((a1 >= a0 ? a1 - a0 : a0 - a1), n));
    }
    return g;
}


void factorize(const uint64_t n, std::vector<uint64_t> &result) {
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
            static std::array<uint64_t, 5> primes_small {1, 3, 5};
            for(auto& prime: primes_small)
            {
                divisor = find_divisor_pollard_rho(n, 1000, prime);
                if (divisor != 1)
                {
                    break;
                }
            }
        }
        if (divisor == 1) {
            result.push_back(n);
            return;
        }
        assert(divisor != 1);
        // рекурсивно обрабатываем найденные множители
        factorize(divisor, result);
        assert((n % divisor) == 0);
        factorize(n / divisor, result);
    }
}

/**
 * @brief Поиск минимального первообразного корня по модулю p.
 * @param p Модуль поля.
 * @return Минимальный генератор.
 */
uint64_t find_min_generator(const uint64_t p) {
    if (p == 2) {
        return 1;
    }
    const auto phi = p - 1;
    std::vector<uint64_t> factors;
    factorize(phi, factors);  // факторизуем p-1

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
        if (k >= n) {
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
 * @param g Генератор поля.
 * @return Значение логарифма, если оно существует.
 */
std::optional<uint64_t> discrete_root(const uint64_t a, const uint64_t b, const uint64_t n) {
    assert(a != 0 || (a == 0 && b == 1));
    if (b == 0) {
        return 0;
    }
    auto g = find_min_generator(n);
    auto gpa = binpow(g, a, n);
    auto y = discrete_logarithm(gpa, b, n);
    if (y.has_value()) {
        auto x = binpow(g, y.value(), n);
        return x;
    }
    return std::nullopt;
}

// Начало тестов

void test_from_task() {
    uint64_t a = 3;
    uint64_t b = 4;
    uint64_t n = 5;
    auto result = discrete_root(a, b, n);
    assert(result.has_value());
    assert(binpow(result.value(), a, n) == b);
    assert(result.value() < n);
    a = 3;
    b = 1;
    n = 7;
    result = discrete_root(a, b, n);
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

void test_module7() {
    uint64_t a = 0;
    uint64_t b = 1;
    uint64_t n = 7;
    auto result = discrete_root(a, b, n);
    assert(result.has_value());
    assert(binpow(result.value(), a, n) == b);
    assert(result.value() < n);
    a = 1;
    b = 3;
    n = 7;
    result = discrete_root(a, b, n);
    assert(result.has_value());
    assert(binpow(result.value(), a, n) == b);
    assert(result.value() < n);
    a = 2;
    b = 2;
    n = 7;
    result = discrete_root(a, b, n);
    assert(result.has_value());
    assert(binpow(result.value(), a, n) == b);
    assert(result.value() < n);
    a = 3;
    b = 6;
    n = 7;
    result = discrete_root(a, b, n);
    assert(result.has_value());
    assert(binpow(result.value(), a, n) == b);
    assert(result.value() < n);
    a = 4;
    b = 4;
    n = 7;
    result = discrete_root(a, b, n);
    assert(result.has_value());
    assert(binpow(result.value(), a, n) == b);
    assert(result.value() < n);
    a = 5;
    b = 5;
    n = 7;
    result = discrete_root(a, b, n);
    assert(result.has_value());
    assert(binpow(result.value(), a, n) == b);
    assert(result.value() < n);
    a = 6;
    b = 1;
    n = 7;
    result = discrete_root(a, b, n);
    assert(result.has_value());
    assert(binpow(result.value(), a, n) == b);
    assert(result.value() < n);
}

void test_module2() {
  const uint64_t a = 1;
  const uint64_t b = 1;
  const uint64_t n = 2;
    auto result = discrete_root(a, b, n);
    assert(result.has_value());
    assert(binpow(result.value(), a, n) == b);
    assert(result.value() < n);
}

/*void test_find_min_generator() {
  const std::vector<uint64_t> modules{2, 3, 5, 7, 11, 13};
  const std::vector<uint64_t> generators{1, 2, 2, 3, 2, 2};
    assert(modules.size() == generators.size());
  const auto result = find_min_generators(modules);
    assert(result == generators);
}*/

/*void test_factorize() {
    std::vector<uint64_t> modules{2, 3, 5, 7, 11, 13};
    auto all_factors_1 = factorize_eratosthenes(modules, -1);
    auto all_factors_2 = factorize_exhaustion(modules, -1);
    assert(all_factors_1 == all_factors_2);
}*/


void test_random_root() {
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> numbers_generator_big(1, 1000);
  for (auto i = 0; i < 1000; ++i) {
    uint64_t n = numbers_generator_big(gen) + 1;
    while(!is_prime(n)) {
      n = numbers_generator_big(gen) + 1;
    }
    const uint64_t a = numbers_generator_big(gen) % n + 1;
    uint64_t b = numbers_generator_big(gen) % n + 1;
    while (a == 0 && b == 0) {
      b = numbers_generator_big(gen) % n + 1;
    }
    auto result = discrete_root(a, b, n);
    if (result.has_value()) {
      assert(binpow(result.value(), a, n) == b);
      assert(result.value() < n);
    }
  }
}

void test_root() {
  auto result = discrete_root(1, 0, 7);
  assert(result == 0);
  result = discrete_root(4, 0, 7);
  assert(result == 0);
  result = discrete_root(0, 1, 7);
  assert(result.has_value());
  result = discrete_root(0, 1, 7);
  assert(result.has_value());
  result = discrete_root(37, 61, 113);
  assert(result == 91);
  result = discrete_root(100000000001, 407240682858, 999999999999);
}


void run_all_tests() {
    test_from_task();
    test_binprod();
    test_binpow();
    test_module7();
    test_module2();
    /*test_factorize();
    test_find_min_generator();*/
    test_random_root();
    test_root();
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
    std::vector<std::tuple<uint64_t, uint64_t>> equations;
    std::vector<uint64_t> modules;
    equations.reserve(T);
    for (size_t i = 0; i < T; ++i) {
        uint64_t a, b, n{0};
        std::cin >> b >> a >> n;
        equations.emplace_back(a, b);
        modules.push_back(n);
    }

    //auto generators = find_min_generators(modules);
    std::vector<std::optional<uint64_t>> results;
    results.reserve(T);
    for (size_t i = 0; i < equations.size(); ++i) {
        auto[a, b] = equations[i];
        results.push_back(discrete_root(a, b, modules[i]));
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