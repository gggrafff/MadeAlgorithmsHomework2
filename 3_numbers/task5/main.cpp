/*
 * E. Китайская теорема
 * ограничение по времени на тест 1 секунда
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Решите в целых числах систему уравнений
 * x≡a(modn)
 * x≡b(modm),
 * где n и m взаимно просты. Среди решений следует выбрать наименьшее неотрицательное число.
 *
 * Входные данные
 * Первая строка входных данных содержит число N, 1≤N≤104, — количество тестов, для которых нужно решить задачу.
 * Следующие N строк содержат по четыре целых числа ai, bi, ni и mi (1≤ni,mi≤10^9, 0≤ai<ni, 0≤bi<mi).
 * Выходные данные
 * Для каждого из тестов выведите искомое наименьшее неотрицательное число xi.
 *
 * Пример
 * Входные данные
 * 2
 * 1 0 2 3
 * 3 2 5 9
 * Выходные данные
 * 3
 * 38
 */

#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <random>
#include <cassert>
#include <numeric>

/**
 * @brief Описывает линейное сравнение x = r (mod m).
 */
struct LinearComparision {
    uint64_t remainder{0};
    uint64_t module{1};
};

using SystemLinearComparision = std::vector<LinearComparision>;

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
 * @brief Вычисляет обратный элемент к a в кольце вычетов по модулю n.
 * @details Для вычисления используется расширенный алгоритм Евклида.
 * @param a Элемент, к которому нужно найти обратный.
 * @param n Количество элементов в кольце вычетов.
 * @return Возвращает x такой, что a * x = 1 (mod n).
 */
uint64_t calculate_inverse_element_euclidean(const uint64_t a, const uint64_t n) {
    auto[gcd, x, y] = continued_division(a, n);
    if (gcd > 1) {
        return 0;
    }
    if (x >= 0) {
        return static_cast<uint64_t>(x);
    }
    return x + n;
}


/**
 * @brief Раскладывает каждое числов массиве на простые множители. Линейное решето Эратосфена.
 * @param numbers Числа, которые нужно факторизовать.
 * @return Массив разложений исходных чисел на простые множители.
 */
std::vector<std::vector<uint64_t>> factorize(const std::vector<uint64_t> &numbers) {
    uint64_t max_number = *std::max_element(numbers.begin(), numbers.end());
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

    std::vector<std::vector<uint64_t>> result;
    for (auto number: numbers) {
        result.emplace_back();
        while (number != 1) {
            result.back().push_back(least_prime[number]);
            number /= least_prime[number];
        }
    }
    return result;
}

std::vector<uint64_t> euler_function(const std::vector<uint64_t> &numbers) {
    const auto factors = factorize(numbers);
    std::vector<uint64_t> eulers(numbers.size(), 1);
    for (size_t i = 0; i < eulers.size(); ++i) {
        if (numbers[i] <= 2) {
            eulers[i] = 1;
            continue;
        }
        auto current_factor = factors[i][0];
        auto current_power = 1;
        for (size_t j = 1; j < factors[i].size(); ++j) {
            if (factors[i][j] == current_factor) {
                current_power += 1;
            } else {
                eulers[i] *= (current_factor - 1);
                for (auto p = 1; p < current_power; ++p) {
                    eulers[i] *= current_factor;
                }
                current_factor = factors[i][j];
                current_power = 1;
            }
        }
        eulers[i] *= (current_factor - 1);
        for (auto p = 1; p < current_power; ++p) {
            eulers[i] *= current_factor;
        }
    }
    return eulers;
}

uint64_t calculate_inverse_element_euler(const uint64_t a, const uint64_t n) {
    uint64_t result = 1;
    auto euler_n = euler_function({n})[0];
    for (auto i = 0; i < euler_n - 1; ++i) {
        result *= a;
        result %= n;
    }
    return result;
}

/**
 * @brief Решение системы двух линейных сравнений x = a (mod n) & x = b (mod m).
 * @details Для поиска решения используется китайская теорема об остатках.
 * @param system Система линейных сравнений.
 * @return Такое число x, которое удовлетворяет всем сравнениям системы.
 */
uint64_t solve_system_linear_comparisons(SystemLinearComparision system) {
    assert(system.size() == 2);
    // Решаем систему:
    // x = r1 (mod a1)
    // x = r2 (mod a2)
    // Или в обозначениях авторов задачи:
    // x = a (mod n)
    // x = b (mod m)
    const auto a = system[0].remainder;
    const auto b = system[1].remainder;
    const auto n = system[0].module;
    const auto m = system[1].module;
    // 1. Вычисляем M = П(ai) = n * m
    const auto M = n * m;
    // 2. Для всех i (в данном случае два уравнения, поэтому i=1..2) находим Mi = M / ai
    const auto M1 = m;
    const auto M2 = n;
    // 3. Находим Mi^-1 = 1 / Mi (mod ai), используя расширенный алгоритм Евклида
    const auto M1_inv = calculate_inverse_element_euler(M1, n);
    const auto M2_inv = calculate_inverse_element_euler(M2, m);
    // 4. Вычисляем искомое значение x = sum(ri * Mi * Mi^-1) (mod M)
    if (M1_inv == 0 && M2_inv == 0) {
        return a;
    }
    if (n!=1) {
        assert((M1 * M1_inv) % n == 1);
    }
    if (m!=1) {
        assert((M2 * M2_inv) % m == 1);
    }

    //const auto old_x = (a * M1 * M1_inv + b * M2 * M2_inv) % M;
    uint64_t x = 0;
    if(a != 0 && b!=0) {
        uint64_t x1 = 0;
        uint64_t am1 = a * M1;
        uint64_t r1 = (1ULL << 60) / am1 + 1;
        for (uint64_t i = 0; i < (M1_inv / r1); ++i) {
            x1 += am1 * r1;
            x1 %= M;
        }
        x1 += am1 * (M1_inv % r1);
        x1 %= M;

        uint64_t x2 = 0;
        uint64_t bm2 = b * M2;
        uint64_t r2 = (1ULL << 60) / bm2 + 1;
        for (uint64_t i = 0; i < (M2_inv / r2); ++i) {
            x2 += bm2 * r2;
            x2 %= M;
        }
        x2 += bm2 * (M2_inv % r2);
        x2 %= M;

        x = (x1 + x2) % M;
        assert(x % n == a);
        assert(x % m == b);
    }
    else
    {
        x = (a * M1 * M1_inv + b * M2 * M2_inv) % M;
        assert(x % n == a);
        assert(x % m == b);
    }
    return x;
}


// Начало тестов

void test_euler_function_1() {
    auto result = euler_function({45, 64, 13, 17, 100, 98, 1, 10});
    auto true_result = std::vector<uint64_t>{24, 32, 12, 16, 40, 42, 1, 4};
    assert(result == true_result);
}

void test_euler_function_2() {
    std::vector<uint64_t> input;
    for (uint64_t i = 1; i <= 99; ++i) {
        input.push_back(i);
    }
    auto result = euler_function(input);
    auto true_result = std::vector<uint64_t>{1, 1, 2, 2, 4, 2, 6, 4, 6, 4, 10, 4, 12, 6, 8, 8, 16, 6, 18, 8, 12, 10, 22,
                                             8, 20, 12, 18, 12, 28, 8, 30, 16, 20, 16, 24, 12, 36, 18, 24, 16, 40, 12,
                                             42, 20, 24, 22, 46, 16, 42, 20, 32, 24, 52, 18, 40, 24, 36, 28, 58, 16, 60,
                                             30, 36, 32, 48, 20, 66, 32, 44, 24, 70, 24, 72, 36, 40, 36, 60, 24, 78, 32,
                                             54, 40, 82, 24, 64, 42, 56, 40, 88, 24, 72, 44, 60, 46, 72, 32, 96, 42,
                                             60};
    assert(result == true_result);
}

void test_gcd_random() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> numbers_generator_big(0, 100);
    for (auto i = 0; i < 1000; ++i) {
        auto first = numbers_generator_big(gen);
        auto second = numbers_generator_big(gen);
        auto[gcd, x, y] = continued_division(first, second);
        assert(std::gcd(first, second) == gcd);
        assert((first * x + second * y) == gcd);
    }
    std::uniform_int_distribution<uint64_t> numbers_generator_small(0, 10);
    for (auto i = 0; i < 100; ++i) {
        auto first = numbers_generator_small(gen);
        auto second = numbers_generator_small(gen);
        auto[gcd, x, y] = continued_division(first, second);
        assert(std::gcd(first, second) == gcd);
        assert((first * x + second * y) == gcd);
    }
}

void test_solving_random() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> numbers_generator_small(0, 1000);
    for(auto i=0; i<100;++i) {
        uint64_t n{2}, m{2};
        while (std::gcd(n, m) != 1) {
            n = numbers_generator_small(gen) + 1;
            m = numbers_generator_small(gen) + 1;
        }
        auto a = numbers_generator_small(gen)%n;
        auto b = numbers_generator_small(gen)%m;

        auto result = solve_system_linear_comparisons({{a, n,},
                                                       {b, m}});

        assert(result % n == a);
        assert(result % m == b);
    }
    std::uniform_int_distribution<> numbers_generator_big(1000000, 1000000000);
    for(auto i=0; i<1;++i) {
        uint64_t n{2}, m{2};
        while (std::gcd(n, m) != 1) {
            n = numbers_generator_big(gen) + 1;
            m = numbers_generator_big(gen) + 1;
        }
        auto a = numbers_generator_big(gen)%n;
        auto b = numbers_generator_big(gen)%m;

        auto result = solve_system_linear_comparisons({{a, n,},
                                                       {b, m}});

        assert(result % n == a);
        assert(result % m == b);
    }
}

void test_inverse_elements() {
    auto result = calculate_inverse_element_euclidean(3, 5);
    assert(result == 2);
    result = calculate_inverse_element_euclidean(3, 11);
    assert(result == 4);
    result = calculate_inverse_element_euclidean(3, 13);
    assert(result == 9);

    result = calculate_inverse_element_euler(3, 5);
    assert(result == 2);
    result = calculate_inverse_element_euler(3, 11);
    assert(result == 4);
    result = calculate_inverse_element_euler(3, 13);
    assert(result == 9);
}

void test_inverse_elements_random() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> numbers_generator_big(1000000, 1000000000);
    for (auto i = 0; i < 5; ++i) {
        const auto m = numbers_generator_big(gen) + 1;
        const auto a = numbers_generator_big(gen) % m;
        const auto a_inv_1 = calculate_inverse_element_euclidean(a, m);
        auto gcd = std::gcd(a, m);
        if (gcd == 1 && a != 0) {
            assert(((a * a_inv_1) % m) == 1);
            const auto a_inv_2 = calculate_inverse_element_euler(a, m);
            assert(((a * a_inv_2) % m) == 1);
            assert(a_inv_1 == a_inv_2);
        } else {
            assert(a_inv_1 == 0);
        }
    }
    std::uniform_int_distribution<> numbers_generator_small(0, 1000);
    for (auto i = 0; i < 1000; ++i) {
        const auto m = numbers_generator_small(gen) + 1;
        const auto a = numbers_generator_small(gen) % m;
        const auto a_inv_1 = calculate_inverse_element_euclidean(a, m);
        auto gcd = std::gcd(a, m);
        if (gcd == 1 && a != 0) {
            assert(((a * a_inv_1) % m) == 1);
            const auto a_inv_2 = calculate_inverse_element_euler(a, m);
            assert(((a * a_inv_2) % m) == 1);
            assert(a_inv_1 == a_inv_2);
        } else {
            assert(a_inv_1 == 0);
        }
    }
}

void test_from_task() {
    auto result = solve_system_linear_comparisons({{1, 2,},
                                                   {0, 3}});
    assert(result == 3);
    result = solve_system_linear_comparisons({{3, 5,},
                                              {2, 9}});
    assert(result == 38);
}

void test_zeros_remainders() {
    const auto result = solve_system_linear_comparisons({{0, 2,},
                                                         {0, 3}});
    assert(result == 0);
}

void test_from_wiki() {
    auto result = solve_system_linear_comparisons({{1, 2,},
                                                   {2, 3}});
    assert(result == 5);
    result = solve_system_linear_comparisons({{1, 2,},
                                              {6, 7}});
    assert(result == 13);
    result = solve_system_linear_comparisons({{2, 3,},
                                              {6, 7}});
    assert(result == 20);
}

void test_from_cyberforum() {
    auto result = solve_system_linear_comparisons({{2, 5,},
                                                   {6, 7}});
    assert(result == 27);
    result = solve_system_linear_comparisons({{5, 11,},
                                              {6, 7}});
    assert(result == 27);
    result = solve_system_linear_comparisons({{2, 5,},
                                              {5, 11}});
    assert(result == 27);
}

void test_degenerate_system() {
    const auto result = solve_system_linear_comparisons({{2, 3,},
                                                         {2, 3}});
    assert(result == 2);
}

void test_zeros_remainders_2() {
    const auto result = solve_system_linear_comparisons({{0, 1,},
                                                         {0, 1}});
    assert(result == 0);
}

void test_non_primes() {
    auto result = solve_system_linear_comparisons({{1, 6,},
                                                   {2, 5}});
    assert(result == 7);
    result = solve_system_linear_comparisons({{1, 6,},
                                              {0, 7}});
    assert(result == 7);
    result = solve_system_linear_comparisons({{2, 10,},
                                              {1, 21}});
    assert(result == 22);
}

void test_by_almalynx() {
    auto result = solve_system_linear_comparisons({{2,  5,},
                                                   {15, 17}});
    assert(result == 32);
    result = solve_system_linear_comparisons({{15, 17,},
                                              {5,  12}});
    assert(result == 185);
    result = solve_system_linear_comparisons({{5, 12,},
                                              {2, 5}});
    assert(result == 17);
}

void test_big_numbers() {
    /*auto result = solve_system_linear_comparisons({{999999993,  1000000000,},
                                                   {999999997, 999999999}});
    assert(result == 32);*/
}

void run_all_tests() {
    test_euler_function_1();
    test_euler_function_2();

    /*test_gcd_random();

    test_inverse_elements();
    test_inverse_elements_random();*/

    test_solving_random();

    test_from_task();
    test_zeros_remainders();
    test_from_wiki();
    test_degenerate_system();
    test_zeros_remainders_2();
    test_non_primes();
    test_from_cyberforum();
    test_by_almalynx();
    test_big_numbers();
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
    size_t N{0};
    std::cin >> N;
    std::vector<SystemLinearComparision> systems;
    for (size_t i = 0; i < N; ++i) {
        uint64_t a, b, n, m;
        std::cin >> a >> b >> n >> m;
        systems.push_back({{a, n},
                           {b, m}});
    }

    // Запись результата
    for (const auto &system : systems) {
        std::cout << solve_system_linear_comparisons(system) << "\n";
    }
    return 0;
}