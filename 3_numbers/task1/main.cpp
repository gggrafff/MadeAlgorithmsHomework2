/*
 * A. Массовое разложение на множители
 * ограничение по времени на тест 1.0 с
 * ограничение по памяти на тест 64 мегабайта
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дано много чисел. Требуется разложить их все на простые множители.
 *
 * Входные данные
 * В первой строке задано число n (2 ≤ n ≤ 300000). В следующих n строках заданы числа ai (2 ≤ ai ≤ 10^6), которые нужно разложить на множители.
 * Выходные данные
 * Для каждого числа выведите в отдельной строке разложение на простые множители в порядке возрастания множителей.
 *
 * Пример
 * Входные данные
 * 4
 * 60
 * 14
 * 3
 * 55
 * Выходные данные
 * 2 2 3 5
 * 2 7
 * 3
 * 5 11
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

/**
 * @brief Раскладывает каждое число в массиве на простые множители. Линейное решето Эратосфена.
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

// Начало тестов

void test_from_task_1() {
    std::vector<uint64_t> numbers{60, 14, 3, 55};
    auto result = factorize(numbers);
    assert(result[0] == std::vector<uint64_t>({2, 2, 3, 5}));
    assert(result[1] == std::vector<uint64_t>({2, 7}));
    assert(result[2] == std::vector<uint64_t>({3}));
    assert(result[3] == std::vector<uint64_t>({5, 11}));
}

void run_all_tests() {
    test_from_task_1();
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
    size_t N{0};
    std::cin >> N;
    std::vector<uint64_t> numbers(N);
    for (size_t i = 0; i < N; ++i) {
        std::cin >> numbers[i];
    }

    auto factors = factorize(numbers);

    // Запись результата
    for (const auto &number_factors: factors) {
        for (const auto factor: number_factors) {
            std::cout << factor << " ";
        }
        std::cout << "\n"; //std::endl;
    }

    return 0;
}