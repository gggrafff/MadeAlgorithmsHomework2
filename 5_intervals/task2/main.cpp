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
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

/**
 * https://neerc.ifmo.ru/wiki/index.php?title=%D0%94%D0%B5%D1%80%D0%B5%D0%B2%D0%BE_%D0%A4%D0%B5%D0%BD%D0%B2%D0%B8%D0%BA%D0%B0
 */
class FenwickTree {
public:
    FenwickTree(const std::vector<int64_t> &numbers) {
        /*
         * function build():
               for i = 0 to N - 1
                   modify(i, a[i])
         */
        /*fenwick_sums.reserve(numbers.size());
        fenwick_sums.push_back(numbers.front());
        for (size_t i = 1; i < numbers.size(); ++i) {
            fenwick_sums[i] = prefix_sums[i - 1];
        }*/
    }

    uint64_t get_sum(size_t l, size_t r) {
        if (l > r) {
            std::swap(l, r);
        }
        /*
         * int sum(i):
               result = 0
               while i >= 0
                   result += t[i]
                   i = f(i) - 1
         */
    }

    void add_to_element(size_t index, int64_t value){
        /*
         * function modify(i, d):
               while i < N
                   t[i] += d
                   i = i | (i + 1)
         */
    }

    void set_element(size_t index, int64_t value){
        /*
         * function set(i, x):
           d = x - a[i]
           a[i] = x
           modify(i, d)
         */
    }

private:
    size_t fenwick(const size_t i)
    {
        return i & (i + 1);
    }
    std::vector<int64_t> fenwick_sums;
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

    // Чтение входных данных


    // Запись результата


    return 0;
}