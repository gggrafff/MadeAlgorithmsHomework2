/*
 * A. Задача для второклассника
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Вам даны два числа. Необходимо найти их произведение.
 *
 * Входные данные
 * Входные данные состоят из двух строк, на каждой из которых находится целое одно целое число,
 * длина которого не превосходит двухсот пятидесяти тысяч символов.
 *
 * Выходные данные
 * Выведите произведение данных чисел.
 *
 * Примеры
 * Входные данные
 * 2
 * 2
 * Выходные данные
 * 4
 *
 * Входные данные
 * 1
 * -1
 * Выходные данные
 * -1
 */

/*
 * Подробнее о:
 *
 */


#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>
#include <utility>
#include <iomanip>
#include <optional>


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
    

    return 0;
}
