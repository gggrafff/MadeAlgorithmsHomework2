/*
 * A. Сумма простая
 * ограничение по времени на тест 1 секунда
 * ограничение по памяти на тест 512 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Вам нужно научиться отвечать на запрос «сумма чисел на отрезке».
 * Массив не меняется. Запросов много. Отвечать на каждый запрос следует за О(1).
 *
 * Входные данные
 * Размер массива — n и числа x, y, a0, порождающие массив a: a[i] = (x * a[i-1] + y) mod 2^16
 * Далее следует количество запросов m и числа z, t, b0, порождающие массив b: b[i] = (z*b[i-1]+t) mod 2^30.
 * Массив c строится следующим образом: c[i] = b[i] mod n.
 * Запросы: i-й из них — найти сумму на отрезке от min(c2i, c2i + 1) до max(c2i, c2i + 1) в массиве a.
 * Ограничения: 1 ≤ n ≤ 10^7, 0 ≤ m ≤ 10^7. Все числа целые от 0 до 2^16. t может быть равно  -1.
 *
 * Выходные данные
 * Выведите сумму всех сумм.
 *
 * Пример
 * Входные данные
 * 3 1 2 3
 * 3 1 -1 4
 * Выходные данные
 * 23
 * Примечание
 * a = {3, 5, 7}, b = {4, 3, 2, 1, 0, 230 - 1}, c = {1, 0, 2, 1, 0, 0},
 * запросы = {[0, 1], [1, 2], [0, 0]}, суммы = {8, 12, 3}.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

template<typename CoefficientsType, typename SequenceType>
class SequenceGenerator {
public:
    SequenceGenerator(CoefficientsType x, CoefficientsType y, SequenceType a0, uint64_t module) :
            x_(x),
            y_(y),
            a_current_(a0),
            module_(module) {}

    SequenceType next() {
        a_current_ = (x_ * a_current_ + y_) % module_;
        return a_current_;
    }

    SequenceType get_current()
    {
        return a_current_;
    }

    SequenceType next_by_module(uint64_t module) {
        return next() % module;
    }

    std::vector<SequenceType> next(size_t count) {
        std::vector<SequenceType> result;
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(next());
        }
        return result;
    }

    std::vector<SequenceType> get_sequence(size_t length) {
        std::vector<SequenceType> result;
        result.reserve(length);
        result.push_back(a_current_);
        for (size_t i = 0; i < length - 1; ++i) {
            result.push_back(next());
        }
        return result;
    }

    std::vector<SequenceType> get_sequence_by_module(size_t length, uint64_t module) {
        std::vector<SequenceType> result;
        result.reserve(length);
        result.push_back(a_current_ % module);
        for (size_t i = 0; i < length - 1; ++i) {
            result.push_back(next_by_module(module));
        }
        return result;
    }

private:
    const uint64_t module_{1 << 16};
    CoefficientsType x_{0};
    CoefficientsType y_{0};
    SequenceType a_current_{0};
};

template<typename SequenceType>
class PrefixAdder {
public:
    PrefixAdder(const std::vector<SequenceType> &numbers) {
        prefix_sums = numbers;
        for (size_t i = 1; i < prefix_sums.size(); ++i) {
            prefix_sums[i] += prefix_sums[i - 1];
        }
    }

    template<typename GeneratorType>
    PrefixAdder(GeneratorType &generator, size_t length) {
        prefix_sums.reserve(length);
        prefix_sums.push_back(generator.get_current());
        for (size_t i = 1; i < length; ++i) {
            prefix_sums.push_back(generator.next() + prefix_sums.back());
        }
    }

    uint64_t get_sum(size_t l, size_t r) {
        if (l > r) {
            std::swap(l, r);
        }
        if (l == 0) {
            return prefix_sums[r];
        }
        return prefix_sums[r] - prefix_sums[l - 1];
    }

private:
    std::vector<uint64_t> prefix_sums;
};


uint64_t solve_task(size_t n,
                    int32_t x, int32_t y,
                    uint16_t a0,
                    size_t m,
                    int32_t z, int32_t t,
                    size_t b0)
{
    if (m == 0)
    {
        return 0;
    }
    SequenceGenerator<int32_t, uint16_t> array_generator(x, y, a0, 1 << 16);
    SequenceGenerator<int32_t, size_t> requests_generator(z, t, b0, 1 << 30);
    PrefixAdder<uint32_t > adder(array_generator, n);
    uint64_t total_sum = adder.get_sum(b0 % n, requests_generator.next_by_module(n));
    for (size_t i = 1; i < m; i += 1) {
        total_sum += adder.get_sum(requests_generator.next_by_module(n), requests_generator.next_by_module(n));
    }
    return total_sum;
}


// Начало тестов

void test_from_task() {
    auto result = solve_task(3, 1, 2, 3, 3, 1, -1, 4);
    assert(result == 23);
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
    size_t n{0};
    int32_t x{0}, y{0};
    uint16_t a0{0};
    std::cin >> n >> x >> y >> a0;
    size_t m{0};
    int32_t z{0}, t{0};
    size_t b0{0};
    std::cin >> m >> z >> t >> b0;

    // Решение задачи
    auto result = solve_task(n, x, y, a0, m, z, t, b0);

    // Запись результата
    std::cout << result;

    return 0;
}