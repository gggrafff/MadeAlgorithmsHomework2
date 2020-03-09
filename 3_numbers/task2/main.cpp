/*
 * B. Просеивай!
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 512 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод

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