/*
 * B. RSQ
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Входные данные
 * В первой строке находится число n — размер массива. (1 ≤ n ≤ 500 000)
 * Во второй строке находится n чисел ai — элементы массива.
 * Далее содержится описание операций, их количество не превышает 1 000 000.
 * В каждой строке находится одна из следующих операций:
 *     set i x — установить a[i] в x.
 *     sum i j — вывести значение суммы элементов в массиве на отрезке с i по j, гарантируется, что (1 ≤ i ≤ j ≤ n).
 *
 * Все числа во входном файле и результаты выполнения всех операций не превышают по модулю 10^18.
 * Выходные данные
 * Выведите последовательно результат выполнения всех операций sum. Следуйте формату выходного файла из примера.
 * Пример
 * Входные данные
 * 5
 * 1 2 3 4 5
 * sum 2 5
 * sum 1 5
 * sum 1 4
 * sum 2 4
 * set 1 10
 * set 2 3
 * set 5 2
 * sum 2 5
 * sum 1 5
 * sum 1 4
 * sum 2 4
 *
 * Выходные данные
 * 14
 * 15
 * 10
 * 9
 * 12
 * 22
 * 20
 * 10
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

/**
 * https://neerc.ifmo.ru/wiki/index.php?title=%D0%94%D0%B5%D1%80%D0%B5%D0%B2%D0%BE_%D0%A4%D0%B5%D0%BD%D0%B2%D0%B8%D0%BA%D0%B0
 */
class FenwickTree {
public:
    explicit FenwickTree(const std::vector<int64_t> &numbers) {
        numbers_ = numbers;
        fenwick_sums_.resize(numbers.size(), 0);
        for (size_t i = 0; i < numbers.size(); ++i) {
            add_to_element(i, numbers[i]);
        }
    }

    uint64_t get_prefix_sum(int64_t r) {
        assert(r >= 0);
        assert(r < fenwick_sums_.size());

        uint64_t result = 0;
        for (; r >= 0; r = (r & (r+1)) - 1) {
            result += fenwick_sums_[r];
        }
        /*while (r >= 0) {
            result += fenwick_sums_[r];
            r = fenwick(r) - 1;
        }*/
        return result;
    }

    uint64_t get_sum(size_t l, size_t r) {
        if (l > r) {
            std::swap(l, r);
        }
        if (l == 0) {
            return get_prefix_sum(r);
        }
        assert(r >= l);
        return get_prefix_sum(r) - get_prefix_sum(l - 1);
    }

    void add_to_element(size_t index, const int64_t value) {
        while (index < fenwick_sums_.size()) {
            fenwick_sums_[index] += value;
            index = index | (index + 1);
        }
    }

    void set_element(const size_t index, const int64_t value) {
        const auto diff = value - numbers_[index];
        numbers_[index] = value;
        add_to_element(index, diff);
    }

private:
    template<typename NumberType>
    static NumberType fenwick(const NumberType i) {
        return i & (i + 1);
    }

    std::vector<int64_t> fenwick_sums_;
    std::vector<int64_t> numbers_;
};

// Начало тестов

void test_from_task() {
    FenwickTree tree({1, 2, 3, 4, 5});
    assert(tree.get_sum(2 - 1, 5 - 1) == 14);
    assert(tree.get_sum(1 - 1, 5 - 1) == 15);
    assert(tree.get_sum(1 - 1, 4 - 1) == 10);
    assert(tree.get_sum(2 - 1, 4 - 1) == 9);
    tree.set_element(1 - 1, 10);
    tree.set_element(2 - 1, 3);
    tree.set_element(5 - 1, 2);
    assert(tree.get_sum(2 - 1, 5 - 1) == 12);
    assert(tree.get_sum(1 - 1, 5 - 1) == 22);
    assert(tree.get_sum(1 - 1, 4 - 1) == 20);
    assert(tree.get_sum(2 - 1, 4 - 1) == 10);
}

void run_all_tests() {
    test_from_task();
}

// Конец тестов

int main(int argc, char *argv[]) {
    /*run_all_tests();
    return 0;*/

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

    // Решение задачи
    size_t n{0};
    std::cin >> n;
    std::vector<int64_t> numbers(n, 0);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> numbers[i];
    }
    FenwickTree tree(numbers);
    std::string operation;
    while (!std::cin.eof()) {
        std::cin >> operation;
        if (operation == "sum") {
            size_t l{0}, r{0};
            std::cin >> l >> r;
            std::cout << tree.get_sum(l - 1, r - 1) << std::endl;
        } else if (operation == "set") {
            size_t index{0};
            uint64_t value{0};
            std::cin >> index >> value;
            tree.set_element(index - 1, value);
        } else {
            break;
        }
    }

    return 0;
}