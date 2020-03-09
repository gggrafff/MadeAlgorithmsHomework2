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
 * Следующие N строк содержат по четыре целых числа ai, bi, ni и mi (1≤ni,mi≤109, 0≤ai<ni, 0≤bi<mi).
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
#include <algorithm>
#include <cassert>



// Начало тестов

void test_from_task_1() {

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



    // Запись результата

    return 0;
}