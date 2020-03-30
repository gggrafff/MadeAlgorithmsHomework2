/*
 * C. RMQ2
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Входные данные
 * В первой строке находится число n — размер массива. (1 ≤ n ≤ 105)
 * Во второй строке находится n чисел ai — элементы массива.
 * Далее содержится описание операций, их количество не превышает 2·10^5.
 * В каждой строке находится одна из следующих операций:
 *     set i j x — установить все a[k], i ≤ k ≤ j в x.
 *     add i j x — увеличить все a[k], i ≤ k ≤ j на x.
 *     min i j — вывести значение минимального элемента в массиве на отрезке с i по j, гарантируется, что (1 ≤ i ≤ j ≤ n).
 * Все числа во входном файле и результаты выполнения всех операций не превышают по модулю 10^18.
 * Выходные данные
 * Выведите последовательно результат выполнения всех операций min. Следуйте формату выходного файла из примера.
 *
 * Пример
 * Входные данные
 * 5
 * 1 2 3 4 5
 * min 2 5
 * min 1 5
 * min 1 4
 * min 2 4
 * set 1 3 10
 * add 2 4 4
 * min 2 5
 * min 1 5
 * min 1 4
 * min 2 4
 * Выходные данные
 * 2
 * 1
 * 1
 * 2
 * 5
 * 5
 * 8
 * 8
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>


/*
 * https://neerc.ifmo.ru/wiki/index.php?title=%D0%94%D0%B5%D1%80%D0%B5%D0%B2%D0%BE_%D0%BE%D1%82%D1%80%D0%B5%D0%B7%D0%BA%D0%BE%D0%B2._%D0%9F%D0%BE%D1%81%D1%82%D1%80%D0%BE%D0%B5%D0%BD%D0%B8%D0%B5
 * https://e-maxx.ru/algo/segment_tree
 */
class SegmentTree {
public:
    explicit SegmentTree(const std::vector<int64_t> &numbers) {

    }

    uint64_t get_min(size_t l, size_t r) {
        if (l > r) {
            std::swap(l, r);
        }
        if (l == r) {

        }

    }

    void add_to_element(size_t index, const int64_t value) {

    }

    void set_element(const size_t index, const int64_t value) {
        const auto diff = value - numbers_[index];
        numbers_[index] = value;
        add_to_element(index, diff);
    }

private:

    std::vector<int64_t> tree_;
    std::vector<int64_t> numbers_;
};

// Начало тестов

void test_from_task() {

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

    // Решение задачи
    size_t n{0};
    std::cin >> n;
    std::vector<int64_t> numbers(n, 0);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> numbers[i];
    }
    SegmentTree tree(numbers);
    std::string operation;
    while (!std::cin.eof()) {
        std::cin >> operation;
        if (operation == "min") {
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