/*
 * D. Разреженные таблицы
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дан массив из n чисел. Требуется написать программу, которая будет отвечать на запросы следующего вида:
 * найти минимум на отрезке между u и v включительно.
 *
 * Входные данные
 * В первой строке зданы три натуральных числа n, m (1⩽n⩽10^5, 1⩽m⩽10^7) и a1 (0⩽a1<16714589) —
 * количество элементов в массиве, количество запросов и первый элемент массива соответственно.
 * Вторая строка содержит два натуральных числа u1 и v1 (1⩽u1,v1⩽n) — первый запрос.
 * Для того, размер ввода был небольшой, массив и запросы генерируются.
 * Элементы a2,a3,…,an задаются следующей формулой: a[i+1]=(23⋅a[i]+21563) mod 16714589.
 * Например, при n=10, a1=12345 получается следующий массив:
 * a = (12345, 305498, 7048017, 11694653, 1565158, 2591019, 9471233, 570265, 13137658, 1325095).
 * Запросы генерируются следующим образом:
 * u[i+1]=((17⋅u[i]+751+r[i]+2i) mod n)+1, v[i]+1=((13⋅v[i]+593+r[i]+5i) mod n)+1, где ri — ответ на запрос номер i.
 * Обратите внимание, что u[i] может быть больше, чем v[i].
 * Выходные данные
 * В выходной файл выведите um, vm и rm(последний запрос и ответ на него).
 * Примеры
 * Входные данные
 * 10 8 12345
 * 3 9
 * Выходные данные
 * 5 3 1565158
 *
 * Примечание
 * Можно заметить, что массивы u, v и r можно не сохранять в памяти полностью.
 * Запросы и ответы на них выглядят следующим образом:
 *  i   ui  vi  ri
 *  1   3   9   570265
 *  2   10  1   12345
 *  3   1   2   12345
 *  4   10  10  1325095
 *  5   5   9   570265
 *  6   2   1   12345
 *  7   3   2   305498
 *  8   5   3   1565158
 *
 * Эта задача скорее всего не решается стандартными интерпретаторами Python 2 и Python 3.
 * Используйте соответствующие компиляторы PyPy.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <tuple>
#include <numeric>

/**
 * @brief Генератор последовательностей вида a[i] = (x * a[i-1] + y) mod n.
 * @tparam CoefficientsType Тип коэффициентов x, y.
 * @tparam SequenceType Тип элементов a[i].
 */
template<typename CoefficientsType, typename SequenceType>
class SequenceGenerator {
public:
    /**
     * @brief Конструируем генератор.
     * @param x Коэффициент х из выражения a[i] = (x * a[i-1] + y) mod n.
     * @param y Коэффициент y из выражения a[i] = (x * a[i-1] + y) mod n.
     * @param a0 Первый элемент последовательности.
     * @param module Модуль n из выражения a[i] = (x * a[i-1] + y) mod n.
     */
    SequenceGenerator(CoefficientsType x, CoefficientsType y, SequenceType a0, uint64_t module) :
            x_(x),
            y_(y),
            a_current_(a0),
            module_(module) {}

    /**
     * @brief Генерирует следующий элемент последовательности.
     * @return Следующий элемент последовательности.
     */
    SequenceType next() {
        a_current_ = (x_ * a_current_ + y_) % module_;
        return a_current_;
    }

    /**
     * @brief Генерирует следующий элемент последовательности c разовой корректировкой коэффициентов и текущего состояния.
     * @details a[i] = ((x + dx) * (a[i-1] + da) + (y + dy)) mod n
     * @param dx Поправка для коэффициента x.
     * @param dy Поправка для коэффициента y.
     * @param da Поправка для текущего элемента.
     * @return Следующий элемент последовательности.
     */
    SequenceType next(CoefficientsType dx, CoefficientsType dy, SequenceType da) {
        a_current_ = ((x_ + dx) * (a_current_ + da) + (y_ + dy)) % module_;
        return a_current_;
    }

    /**
     * @brief Возвращает текущий элемент последовательности.
     * @return Текущий элемент последовательности.
     */
    SequenceType get_current() {
        return a_current_;
    }

    /**
     * @brief Генерирует следующий элемент последовательности и берет его по заданному модулю.
     * @param module Модуль, по которому нужно взять следующий элемент.
     * @return Остаток от деления следующего элемента последовательности на module.
     */
    SequenceType next_by_module(uint64_t module) {
        return next() % module;
    }

    /**
     * @brief Генерирует несколько следующих элементов последовательности.
     * @param count Количество генерируемых элементов.
     * @return Массив сгенерированных элементов.
     */
    std::vector<SequenceType> next(size_t count) {
        std::vector<SequenceType> result;
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(next());
        }
        return result;
    }

    /**
     * @brief Генерирует несколько следующих элементов последовательности и возвращает вместе с текущим элементом.
     * @param length Общая длина последовательности.
     * @return Текущий элемент и несколько сгенерированных.
     */
    std::vector<SequenceType> get_sequence(size_t length) {
        std::vector<SequenceType> result;
        result.reserve(length);
        result.push_back(a_current_);
        for (size_t i = 0; i < length - 1; ++i) {
            result.push_back(next());
        }
        return result;
    }

    /**
     * @brief Генерирует несколько следующих элементов последовательности и возвращает вместе с текущим элементом.
     * @brief Элементы берутся по заданному модулю.
     * @param length Общая длина последовательности.
     * @param module Модуль, по которому берём элементы.
     * @return Текущий элемент и несколько сгенерированных.
     */
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

/**
 * @brief Вычисляет для каждого целого x от 1 до n значение y = 2^k такое, что 2^(k - 1) < x <= 2^k
 * @param max_number Максимальное число n, до которого вычисляем.
 * @return Массив результатов для всех x от 1 до max_number.
 */
std::vector<uint8_t> calculate_ceil_lb(uint64_t max_number) {
    std::vector<uint8_t> result{0};
    for (uint64_t i = 1; i < max_number; ++i) {
        result.push_back(result.back());
        if (i > (1 << result.back())) {
            result.back() += 1;
        }
    }
    return result;
}

/**
 * @brief Реализация разреженной таблицы.
 * @tparam SequenceType Тип элементов в таблице.
 */
template<typename SequenceType>
class SparseTable {
public:
    /**
     * @brief Конструирует разреженную таблицу методом ДП.
     * @param numbers Массив элементов, для которых строим таблицу.
     */
    SparseTable(const std::vector<SequenceType> &numbers) {
        ceil_lb_ = calculate_ceil_lb(numbers.size() + 1);
        sparse_table_.resize(numbers.size(), std::vector<SequenceType>(
                ceil_lb_.back(),
                std::numeric_limits<SequenceType>::max()));
        for (auto l = 0; l < sparse_table_.size(); ++l) {
            sparse_table_[l].front() = numbers[l];
        }
        dynamics_initialize();
    }

    /**
     * @brief Конструирует разреженную таблицу методом ДП.
     * @tparam GeneratorType Тип генератора элементов.
     * @param generator Генератор элементов.
     * @param length Длина последовательности.
     */
    template<typename GeneratorType>
    SparseTable(GeneratorType &generator, size_t length) {
        ceil_lb_ = calculate_ceil_lb(length + 1);
        sparse_table_.resize(length, std::vector<SequenceType>(
                ceil_lb_.back(),
                std::numeric_limits<SequenceType>::max()));
        sparse_table_[0][0] = generator.get_current();
        for (auto l = 1; l < sparse_table_.size(); ++l) {
            sparse_table_[l].front() = generator.next();
        }
        dynamics_initialize();
    }

    /**
     * @brief Запрос минимума на отрезке.
     * @param l Левая граница отрезка. Индекс.
     * @param r Правая граница отрезка. Индекс.
     * @return Результат запроса.
     */
    SequenceType get_min(size_t l, size_t r) {
        if (l == r) {
            return sparse_table_[l][0];
        }
        if (l > r) {
            std::swap(l, r);
        }
        auto k = ceil_lb_[r - l + 1] - 1;
        return std::min(sparse_table_[l][k], sparse_table_[r - (1 << k) + 1][k]);
    }

private:
    /**
     * @brief Совершает переходы в методе ДП для заполнения таблицы.
     */
    void dynamics_initialize() {
        for (auto k = 1; k < sparse_table_[0].size(); ++k) {
            for (auto l = 0; l < sparse_table_.size(); ++l) {
                auto right = l + (1 << (k - 1));
                if (right >= sparse_table_.size()) {
                    break;
                }
                sparse_table_[l][k] = std::min(sparse_table_[l][k - 1], sparse_table_[right][k - 1]);
            }
        }
    }

    std::vector<std::vector<SequenceType>> sparse_table_;
    std::vector<uint8_t> ceil_lb_;
};

std::tuple<size_t, size_t, uint32_t> solve_task(
        size_t n, size_t m, uint32_t a1,
        size_t u1, size_t v1) {
    if (n == 1) {
        return {1, 1, a1};
    }
    SequenceGenerator<uint16_t, uint32_t> array_generator(23, 21563, a1, 16714589);
    SparseTable<uint32_t> min_table(array_generator, n);
    SequenceGenerator<size_t, size_t> u_generator(17, 751, u1 - 1, n);
    SequenceGenerator<size_t, size_t> v_generator(13, 593, v1 - 1, n);
    auto r_current = min_table.get_min(u1 - 1, v1 - 1);
    for (size_t i = 1; i < m; ++i) {
        r_current = min_table.get_min(u_generator.next(0, r_current + 2 * i, 1),
                                      v_generator.next(0, r_current + 5 * i, 1));
    }
    return {u_generator.get_current() + 1, v_generator.get_current() + 1, r_current};
}

// Начало тестов

void test_one_element() {
    auto[u, v, r] = solve_task(1, 8, 12345, 1, 1);
    assert(u == 1);
    assert(v == 1);
    assert(r == 12345);
}

void test_one_request() {
    auto[u, v, r] = solve_task(10, 1, 12345, 3, 9);
    assert(u == 3);
    assert(v == 9);
    assert(r == 570265);
}

void test_from_task() {
    auto[u, v, r] = solve_task(10, 8, 12345, 3, 9);
    assert(u == 5);
    assert(v == 3);
    assert(r == 1565158);
}

void test_sequence_generator() {
    SequenceGenerator<uint16_t, uint32_t> array_generator(23, 21563, 12345, 16714589);
    auto result = array_generator.get_sequence(10);
    std::vector<uint32_t> true_result{12345, 305498, 7048017, 11694653, 1565158, 2591019, 9471233, 570265, 13137658,
                                      1325095};
    assert(result == true_result);
}

void test_sparse_table() {
    SequenceGenerator<uint16_t, uint32_t> array_generator(23, 21563, 12345, 16714589);
    size_t n = 1000;
    auto sequence = array_generator.get_sequence(n);
    SparseTable min_table(sequence);
    SequenceGenerator<size_t, size_t> u_generator(17, 751, 3 - 1, n);
    SequenceGenerator<size_t, size_t> v_generator(13, 593, 9 - 1, n);
    auto r_current = min_table.get_min(3 - 1, 9 - 1);
    auto r_current_true = *std::min_element(std::next(sequence.begin(), 3 - 1), std::next(sequence.begin(), 9 - 1 + 1));
    assert(r_current == r_current_true);
    for (size_t i = 0; i < 1000; ++i) {
        auto u = u_generator.next(0, r_current + 2 * i, 1);
        auto v = v_generator.next(0, r_current + 5 * i, 1);
        if (u > v) {
            std::swap(u, v);
        }
        r_current = min_table.get_min(u, v);
        r_current_true = *std::min_element(std::next(sequence.begin(), u), std::next(sequence.begin(), v + 1));
        assert(r_current == r_current_true);
    }
}

void run_all_tests() {
    test_from_task();
    test_sequence_generator();
    test_one_element();
    test_one_request();
    test_sparse_table();
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
    size_t n{0}, m{0};
    uint32_t a1{0};
    std::cin >> n >> m >> a1;
    size_t u1{0}, v1{0};
    std::cin >> u1 >> v1;

    // Решение задачи
    auto[u, v, r] = solve_task(n, m, a1, u1, v1);

    // Запись результата
    std::cout << u << " " << v << " " << r;

    return 0;
}