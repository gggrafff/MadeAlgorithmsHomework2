/*
 * A. Разбиение на пары
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 64 мегабайта
 * ввод стандартный ввод
 * вывод стандартный вывод

 * Однажды n ребят решили поучаствовать в VK Cup. Для этого им нужно разбиться на команды по два человека.
 * Однако выяснилось, что не все ребята хорошо ладят друг с другом. Вы выяснили для каждой пары ребят,
 * могут ли они участвовать в одной команде. Помогите им сформировать максимальное число команд.
 *
 * Входные данные
 * Первая строка содержит число n (1 ≤ n ≤ 18), далее n строк по n символов описывают отношения ребят.
 * Символ j строки i равен «Y», если ребята i и j могут участвовать в команде.
 * Выходные данные
 * Выведите одно число — максимальное число участников в командах, которые можно сформировать.
 *
 * Примеры
 * Входные данные
 * 5
 * NYYYY
 * YNNNN
 * YNNNN
 * YNNNN
 * YNNNN
 * Выходные данные
 * 2
 *
 * Входные данные
 * 5
 * NYYYY
 * YNNNN
 * YNNNY
 * YNNNY
 * YNYYN
 * Выходные данные
 * 4
 */

/*
 * Что храним? dp[mask] - максимальный ответ для подмножества mask
 * База dp[0] = 0
 * Переход dp[mask] = max(dp[mask'], dp[mask''] + 1), где mask' = mask - 2^i, mask'' = mask - 2^i - 2^j (есть ребро между i и j)
 * Порядок по возрастанию mask
 * Ответ dp[2^n - 1]
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

/**
 * @brief Матрица смежности графа.
 */
using Graph = std::vector<std::vector<bool>>;

/**
 * @brief Извлечение значения бита из битовой маски.
 * @tparam NumberType Числовой тип данных, которым представлена битовая маска.
 * @param mask Битовая маска.
 * @param index Номер бита.
 * @return Значение бита.
 */
template<typename NumberType>
NumberType getBit(NumberType mask, uint8_t index) {
    return (mask >> index) & 1;
}

/**
 * @brief Поиск максимального паросочетания в произвольном графе методом ДП.
 * @param graph Матрица смежности графа. Количество вершин графа не более 30.
 * @return Максимальное паросочетание.
 */
uint64_t find_highest_matching(const Graph &graph) {
    assert(graph.size() <= 30);
    size_t N = (1U << graph.size());  // количество всех подмножеств
    std::vector<uint64_t> dp(N, 0);
    for (size_t mask = 1; mask < N; ++mask) {
        for (size_t i = 0; i < graph.size(); ++i) {
            if (getBit(mask, static_cast<uint8_t>(i)) == 1) {
                dp[mask] = dp[mask - (1 << i)];  // mask' = mask - 2^i
                for (size_t j = 0; j < graph.size(); ++j) {
                    if (getBit(mask, static_cast<uint8_t>(j)) == 1 && graph[i][j]) {
                        dp[mask] = std::max(dp[mask], dp[mask - (1 << i) - (1 << j)] + 1);  // mask'' = mask - 2^i - 2^j
                    }
                }
            }
        }
    }
    return dp.back();
}

// Начало тестов

void test_from_task_1() {
    Graph graph = {
            {false, true,  true,  true,  true},
            {true,  false, false, false, false},
            {true,  false, false, false, false},
            {true,  false, false, false, false},
            {true,  false, false, false, false},
    };
    auto result = find_highest_matching(graph);
    assert(result == 1);
}

void test_from_task_2() {
    Graph graph = {
            {false, true,  true,  true,  true},
            {true,  false, false, false, false},
            {true,  false, false, false, true},
            {true,  false, false, false, true},
            {true,  false, true,  true,  false},
    };
    auto result = find_highest_matching(graph);
    assert(result == 2);
}

void run_all_tests() {
    test_from_task_1();
    test_from_task_2();
}

// Конец тестов

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        }
    }

    // Чтение входных данных
    size_t N{0};
    std::cin >> N;
    Graph graph(N);
    for (size_t i = 0; i < N; ++i) {
        std::string compatibility;
        std::cin >> compatibility;
        graph[i].reserve(N);
        for (size_t j = 0; j < N; ++j) {
            graph[i].push_back(compatibility[j] == 'Y');
        }
    }

    auto pairs_number = find_highest_matching(graph);

    // Запись результата
    std::cout << pairs_number * 2;  // Вывести нужно количество участников (вдвое больше количества пар)

    return 0;
}