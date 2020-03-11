/*
 * B. Просеивай!
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 512 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 * 
 * Для положительного целого n определим функции:
 * d(n) — минимальный делитель n, больший 1, по определению положим d(1) = 0.
 * s0(n) — количество различных делителей n.
 * s1(n) — сумма всех делителей n.
 * φ(n) — функция Эйлера, количество целых чисел k, таких что 1 ≤ k ≤ n и GCD(n, k) = 1.
 *
 * По данному числу n вычислите sum(d[k], k=1..n), sum(s0[k], k=1..n), sum(s1[k], k=1..n) и sum(φ[k], k=1..n).
 * Входные данные
 * В единственной строке записано число n (1 ≤ n ≤ 107).
 * Выходные данные
 * Выведите четыре числа: sum(d[k], k=1..n), sum(s0[k], k=1..n), sum(s1[k], k=1..n) и sum(φ[k], k=1..n).
 * Примеры
 * Входные данные
 * 10
 * Выходные данные
 * 28 27 87 32
 */

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * Весь код можно не смотреть, т.к. используется только одна функция calculate_summary_statistics_fast.
 * Остальной код пока не удалил, потому что использую его в других задачах, а так же для проверки быстрого варианта решения, да и нравится он мне больше, чем calculate_summary_statistics_fast.
 * Если есть желание посмотреть всё, то лучше начать с конца: с calculate_summary_statistics.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>

/**
 * @brief Структура для сохранения ответа
 */
struct NumberStatistics {
    uint64_t number{0};  // Число
    uint64_t d{0};  // минимальный делитель n, больший 1, d(1) = 0
    uint64_t s0{0};  // количество различных делителей
    uint64_t s1{0};  // сумма всех делителей
    uint64_t euler{0};  // функция Эйлера, количество целых чисел k, таких что 1 ≤ k ≤ n и GCD(n, k) = 1
};

/**
 * @brief Для хранения чисел в канонической форме.
 * @details Элемент вектора - множитель в канонической форме.
 * @details Первое поле множителя - простое число. Второе - показатель степени.
 */
using CanonicalFormNumber = std::vector<std::pair<uint64_t, uint64_t>>;

/**
 * @brief Раскладывает каждое число в массиве на простые множители. Линейное решето Эратосфена.
 * @param numbers Числа, которые нужно факторизовать.
 * @return Массив разложений исходных чисел на простые множители.
 */
std::vector<std::vector<uint64_t>> factorize(const std::vector<uint64_t> &numbers) {
    const uint64_t max_number = *std::max_element(numbers.begin(), numbers.end());
    std::vector<uint64_t> primes;
    std::vector<uint64_t> least_prime(max_number + 1, 0);

    for (uint64_t i = 2; i <= max_number; ++i) {
        if (least_prime[i] == 0) {
            least_prime[i] = i;
            primes.push_back(i);
        }
        for (const auto prime : primes) {
            if ((prime > least_prime[i]) || (prime * i > max_number)) {
                break;
            }
            least_prime[prime * i] = prime;
        }
    }

    std::vector<std::vector<uint64_t>> result;
    for (auto number : numbers) {
        result.emplace_back();
        while (number != 1) {
            result.back().push_back(least_prime[number]);
            number /= least_prime[number];
        }
    }
    return result;
}

/**
 * @brief Представляет набор чисел в канонической форме.
 * @param numbers Числа, которые нужно факторизовать.
 * @return Массив канонических форм чисел.
 */
std::vector<CanonicalFormNumber> numbers_to_canonical_form(const std::vector<uint64_t> &numbers) {
    const auto factors = factorize(numbers);
    std::vector<CanonicalFormNumber> canonical(numbers.size());
    for (size_t i = 0; i < canonical.size(); ++i) {
        if (numbers[i] == 1) {
            canonical[i].emplace_back(1, 1);
            continue;
        }
        auto current_factor = factors[i][0];
        auto current_power = 1;
        for (size_t j = 1; j < factors[i].size(); ++j) {
            if (factors[i][j] == current_factor) {
                current_power += 1;
            } else {
                canonical[i].emplace_back(current_factor, current_power);
                current_factor = factors[i][j];
                current_power = 1;
            }
        }
        canonical[i].emplace_back(current_factor, current_power);
    }
    return canonical;
}

/**
 * @brief Функция Эйлера для набора чисел.
 * @param numbers Числа в канонической форме.
 * @return Значения функции Эйлера для входных чисел.
 */
std::vector<uint64_t> euler_function(const std::vector<CanonicalFormNumber> &numbers) {
    std::vector<uint64_t> eulers(numbers.size(), 1);
    for (size_t i = 0; i < eulers.size(); ++i) {
        if (numbers[i][0].first == 1 && numbers[i][0].second == 1) {
            continue;
        }
        for (size_t j = 0; j < numbers[i].size(); ++j) {
            eulers[i] *= (numbers[i][j].first - 1);
            for (uint64_t p = 1; p < numbers[i][j].second; ++p) {
                eulers[i] *= numbers[i][j].first;
            }
        }
    }
    return eulers;
}

/**
 * @brief Подсчёт количества делителей для набора чисел.
 * @details Формулу можно найти на странцие 367 книги Куликов Л.Я. Алгебра и теория чисел
 * @details Учеб. пособие для педагогических институтов. — М.: Высш. школа, 1979. — 559 с.
 * @param numbers Числа в канонической форме.
 * @return Количества делителей для входных чисел.
 */
std::vector<uint64_t> divisors_count(const std::vector<CanonicalFormNumber> &numbers) {
    std::vector<uint64_t> divisors_count(numbers.size(), 1);
    for (size_t i = 0; i < divisors_count.size(); ++i) {
        if (numbers[i][0].first == 1 && numbers[i][0].second == 1) {
            continue;
        }
        for (size_t j = 0; j < numbers[i].size(); ++j) {
            divisors_count[i] *= (numbers[i][j].second + 1);
        }
    }
    return divisors_count;
}

/**
 * @brief Подсчёт суммы делителей для набора чисел.
 * @details Формулу можно найти на странцие 367 книги Куликов Л.Я. Алгебра и теория чисел
 * @details Учеб. пособие для педагогических институтов. — М.: Высш. школа, 1979. — 559 с.
 * @param numbers Числа в канонической форме.
 * @return Суммы делителей для входных чисел.
 */
std::vector<uint64_t> divisors_sum(const std::vector<CanonicalFormNumber> &numbers) {
    std::vector<uint64_t> divisors_sum(numbers.size(), 1);
    for (size_t i = 0; i < divisors_sum.size(); ++i) {
        if (numbers[i][0].first == 1 && numbers[i][0].second == 1) {
            continue;
        }
        for (size_t j = 0; j < numbers[i].size(); ++j) {
            divisors_sum[i] *=
                    (std::pow(numbers[i][j].first, (numbers[i][j].second + 1)) - 1) / (numbers[i][j].first - 1);
        }
    }
    return divisors_sum;
}

/**
 * @brief Подсчёт d(n), s0(n), s1(n), euler(n) для набора чисел.
 * @param numbers Числа, для которых считаем характеристики.
 * @return Массив характеристик в структурах NumberStatistics.
 */
std::vector<NumberStatistics> calculate_statistics(const std::vector<uint64_t> &numbers) {
    std::vector<NumberStatistics> result(numbers.size());
    const auto canonicals = numbers_to_canonical_form(numbers);
    const auto eulers = euler_function(canonicals);
    const auto div_counts = divisors_count(canonicals);
    const auto div_sums = divisors_sum(canonicals);
    for (size_t i = 0; i < result.size(); ++i) {
        if (numbers[i] <= 1) {
            result[i].d = 0;
        } else {
            result[i].d = canonicals[i].front().first;
        }
        result[i].euler = eulers[i];
        result[i].s0 = div_counts[i];
        result[i].s1 = div_sums[i];
    }
    return result;
}

/**
 * @brief Поиск суммы характеристик согласно условию задачи.
 * @details Работает медленно, но является reusable.
 * @param n Верхняя граница диапазона входных чисел.
 * @return Суммы характеристик по всем числам из диапазона.
 */
NumberStatistics calculate_summary_statistics(const size_t n) {
    std::vector<uint64_t> numbers;
    numbers.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        numbers.push_back(i + 1);
    }
    auto statistics = calculate_statistics(numbers);
    NumberStatistics summary;
    for (const auto &number: statistics) {
        summary.d += number.d;
        summary.s0 += number.s0;
        summary.s1 += number.s1;
        summary.euler += number.euler;
    }
    return summary;
}

/**
 * @brief Поиск суммы характеристик согласно условию задачи.
 * @details Работает быстро, но не пригодно для использования в других задачах.
 * @param n Верхняя граница диапазона входных чисел.
 * @return Суммы характеристик по всем числам из диапазона.
 */
NumberStatistics calculate_summary_statistics_fast(const size_t n) {
    // Линейное решето Эратосфена
    std::vector<uint64_t> primes;
    std::vector<uint64_t> least_prime(n + 1, 0);
    // Отдельно обработаем чётные и кратные трём для ускорения.
    if (n >= 2) {
        primes.push_back(2);
    }
    if (n >= 3) {
        primes.push_back(3);
    }
    if (n >= 3) {
        //кратные трём
        for (uint64_t i = 3; i <= n; i += 3) {
            least_prime[i] = 3;
        }
    }
    if (n >= 2) {
        // чётные
        for (uint64_t i = 2; i <= n; i += 2) {
            least_prime[i] = 2;
        }
    }
    // Решето
    for (uint64_t i = 5; i <= n; i += 2) {
        if (least_prime[i] == 0) {
            least_prime[i] = i;
            primes.push_back(i);
        }
        for (const auto prime : primes) {
            if ((prime > least_prime[i]) || (prime * i > n)) {
                break;
            }
            least_prime[prime * i] = prime;
        }
    }

    NumberStatistics summary;

    summary.d = std::accumulate(least_prime.begin(), least_prime.end(), 0ULL);

    // Все числа делятся на 1 (n делителей) и на себя (n делителей).
    summary.s0 = n + n - 1;
    // Все числа делятся на 1 (сумма n*1) и на себя (сумма (n+1)/2*n).
    summary.s1 = n + static_cast<uint64_t>(static_cast<double>(n + 1) / 2.0 * static_cast<double>(n)) - 1;
    for (uint64_t number = 2; number <= n / 2; ++number) {
        // (n / number) чисел из диапзаона делятся на number
        summary.s0 += (n / number) - 1;
        summary.s1 += number * (n / number - 1);
    }

    // Считаем сумму значений функции Эйлера
    summary.euler += 1;  // Для number == 1
    for (uint64_t i = 2; i <= n; ++i) {  // Для остальных чисел
        auto number = i;

        uint64_t euler = 1;

        uint64_t current_factor = least_prime[number];
        uint64_t current_power = 1;
        number /= least_prime[number];

        while (number != 1) {
            const auto current_prime = least_prime[number];
            if (current_prime == current_factor) {
                current_power += 1;
            } else {
                euler *= (current_factor - 1) * static_cast<uint64_t>(std::pow(current_factor, current_power - 1));

                current_factor = current_prime;
                current_power = 1;
            }
            number /= least_prime[number];
        }
        euler *= (current_factor - 1) * static_cast<uint64_t>(std::pow(current_factor, current_power - 1));

        summary.euler += euler;
    }
    return summary;
}

// Начало тестов

void test_from_task_1() {
    const auto result = calculate_summary_statistics(10);
    assert(result.d == 28);
    assert(result.s0 == 27);
    assert(result.s1 == 87);
    assert(result.euler == 32);
}

void test_normal_and_fast() {
    for (auto i = 1; i < 100; ++i) {
        const auto result1 = calculate_summary_statistics(i);
        const auto result2 = calculate_summary_statistics_fast(i);
        assert(result1.d == result2.d);
        assert(result1.s0 == result2.s0);
        assert(result1.s1 == result2.s1);
        assert(result1.euler == result2.euler);
    }
}


void run_all_tests() {
    test_from_task_1();
    test_normal_and_fast();
}

// Конец тестов

int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);

    if (argc > 1) {
        if (std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        }
    }

    // Чтение входных данных
    uint64_t n{0};
    std::cin >> n;

    const auto result = calculate_summary_statistics_fast(n);

    // Запись результата
    std::cout << result.d << " " << result.s0 << " " << result.s1 << " " << result.euler;

    return 0;
}