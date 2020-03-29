/*
 * F. RMQ наоборот
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод rmq.in
 * вывод rmq.out
 *
 * Рассмотрим массив a[1..n]. Пусть Q(i, j) — ответ на запрос о нахождении минимума среди чисел a[i], ..., a[j].
 * Вам даны несколько запросов и ответы на них. Восстановите исходный массив.
 *
 * Входные данные
 * Первая строка входного файла содержит число n — размер массива, и m — число запросов (1 ≤ n, m ≤ 100 000). Следующие m строк содержат по три целых числа i, j и q, означающих, что Q(i, j) = q (1 ≤ i ≤ j ≤ n,  - 231 ≤ q ≤ 231 - 1).
 * Выходные данные
 * Если искомого массива не существует, выведите строку «inconsistent».
 * В противном случае в первую строку выходного файла выведите «consistent».
 * Во вторую строку выходного файла выведите элементы массива.
 * Элементами массива должны быть целые числа в интервале от  - 2^31 до 2^31 - 1 включительно.
 * Если решений несколько, выведите любое.
 *
 * Примеры
 * Входные данные
 * 3 2
 * 1 2 1
 * 2 3 2
 * Выходные данные
 * consistent
 * 1 2 2
 *
 * Входные данные
 * 3 3
 * 1 2 1
 * 1 1 2
 * 2 3 2
 * Выходные данные
 * inconsistent
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>



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