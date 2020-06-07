/*
 * C. ДНК Роботов
 *
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Последние достижения в технологии синтеза ДНК позволили провести эксперимент по созданию биороботов.
 *
 * Для облегчения задачи создания ПО для управления роботами было принято решение, что их ДНК будет состоять
 * из M=2n символов для некоторого n≥2. Кроме этого, по техническим причинам это будет не обычная строка,
 * а циклическая, то есть её можно начинать читать с любой позиции.
 *
 * Одной из целей эксперимента является изучение мутаций биороботов.
 * В результате продолжительных наблюдений было найдено много различных видов роботов.
 * Для понимания процесса мутации учёным необходимо решить следующую задачу.
 *
 * Для ДНК двух роботов требуется определить коэффициент их похожести.
 * Он вычисляется, как максимальное количество совпадающих символов при наилучшем совмещении этих ДНК.
 * Чем больше символов совпадает, тем лучше совмещение.
 *
 * Требуется написать программу, которая найдёт наилучшее совмещение двух ДНК.
 *
 * Входные данные
 * В первой строке входного файла записано одно число M(4≤M≤131072).
 * В следующих двух строках записаны ДНК двух роботов.
 * Обе ДНК — строки, состоящие ровно из M символов из множества {'A', 'C', 'G', 'T'}.
 *
 * Выходные данные
 * В выходной файл выведите два числа — максимальное количество совпадающих символов и значение оптимального
 * сдвига — неотрицательное количество символов второй ДНК, которые необходимо перенести из конца строки в
 * её начало для достижения наилучшего совмещения.
 *
 * Пример
 * Входные данные
 * 16
 * ACGTACGTACGTACGT
 * CGTACGTACGTACGTC
 *
 * Выходные данные
 * 15 1
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
