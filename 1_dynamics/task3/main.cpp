/*
 * C. Наибольшая возрастающая подпоследовательность
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Пусть a1, a2, ..., an — числовая последовательность.
 * Длина последовательности — это количество элементов этой последовательности.
 * Последовательность ai1, ai2, ..., aik называется подпоследовательностью последовательности a,
 * если 1 ≤ i1 < i2 < ... < ik ≤ n. Последовательность a называется возрастающей, если a1 < a2 < ... < an.
 *
 * Вам дана последовательность, содержащая n целых чисел. Найдите ее самую длинную возрастающую подпоследовательность.
 * Входные данные
 * В первой строке задано одно число n (1 ≤ n ≤ 2000) — длина подпоследовательности.
 * В следующей строке задано n целых чисел ai ( - 109 ≤ ai ≤ 109) — элементы последовательности.
 * Выходные данные
 * В первой строке выведите число k — длину наибольшей возрастающей подпоследовательности.
 * В следующей строке выведите k чисел — саму подпоследовательность.
 *
 * Примеры
 * Входные данные
 * 5
 * 1 3 5 4 2
 * Выходные данные
 * 3
 * 1 3 5
 *
 * Входные данные
 * 3
 * 1 2 3
 * Выходные данные
 * 3
 * 1 2 3
 */

/*
 * Что храним? dp[i] - максимальная длина возрастающей подпоследовательности, оканчивающейся в i
 * База dp[0][0] = 1
 * Переход dp[i] = max(dp[j] | j<i, a[j]<a[i]) + 1
 * Порядок по возрастанию i
 * Ответ max(dp[i] | i=0..N)
 */


#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <ctime>
#include <random>
#include <limits>
#include <functional>

/**
 * @brief Структура для удобства использования ДП. Будет храниться в динамике.
 */
struct DpElement {
    int64_t number {0};  // член последовательности
    size_t length_subsequence {0};  // максимальная длина подпоследовательности
    size_t parent {0};  // индекс предыдущего элемента подпоследовательности
};

/**
 * @brief Поиск наибольшей возрастающей подпоследовательности методом ДП. Медленная реализация за O(n^2).
 * @param sequence Исходная последовательность.
 * @return Наибольшая возрастающая подпоследовательность.
 */
std::vector<int64_t> find_highest_increasing_subsequence_dummy(const std::vector<int64_t>& sequence) {
    if (sequence.empty()) {
        return sequence;
    }

    std::vector<DpElement> dp(sequence.size());
    std::transform(sequence.begin(), sequence.end(), dp.begin(), [](const auto& element){
        return DpElement{ element, 1, 0};
    });
    for (size_t i = 1; i < sequence.size(); ++i) {
        bool success = false;
        DpElement max_element;
        size_t max_element_index = 0;
        for(size_t j = 0; j < i; ++j)
        {
            if((dp[j].length_subsequence > max_element.length_subsequence) && (dp[j].number < sequence[i]))
            {
                success = true;
                max_element = dp[j];
                max_element_index = j;
            }
        }
        if (success) {
            dp[i].length_subsequence = max_element.length_subsequence + 1;
            dp[i].parent = max_element_index;
        }
    }

    auto max_element = std::max_element(dp.begin(), dp.end(), [](const auto& lhs, const auto& rhs){
        return (lhs.length_subsequence < rhs.length_subsequence);
    });
    std::vector<int64_t> subsequence(1, max_element->number);
    auto distance = std::distance(dp.begin(), max_element);
    assert(distance >= 0);
    auto current_index  = static_cast<size_t>(distance);
    while (dp[current_index].length_subsequence > 1) {
        current_index = dp[current_index].parent;
        subsequence.push_back(dp[current_index].number);
    }
    std::reverse(subsequence.begin(), subsequence.end());
    for (size_t i = 0; i<subsequence.size()-1; ++i)
    {
        assert(subsequence[i]<subsequence[i+1]);
    }
    return subsequence;
}

/**
 * @brief Поиск наибольшей возрастающей подпоследовательности методом ДП. Быстрая реализация за O(n log n).
 *
 * @details Алгоритм за O(n log n)
 * @details Для последовательности x будем хранить массивы:
 * @details last_elements[i] - наименьший по величине элемент, на который оканчивается возрастающая последовательность длины i
 * @details ast_indexes[i] - индекс last_elements[i] в исходной последовательности
 * @details Кроме того, будем хранить максимальную длину возрастающей подпоследовательности и индексы элементов этой подпоследовательности.
 * @details При переходе к следующему элементу должна сохраняться отсортированность last_elements.
 * @details Переход к следующему элементу: бинарным поиском ищем по last_elements позицию,
 * @details в которую можно вставить следующий элемент, чтобы продлить наибольшую из возможных подпоследовательностей.
 * @details При достижении максимума длины, фиксируем его и получившуюся в last_indexes последовательность.
 * @details Подробнее: https://ru.wikipedia.org/wiki/Задача_поиска_наибольшей_увеличивающейся_подпоследовательности
 *
 * @param sequence Исходная последовательность.
 * @return Наибольшая возрастающая подпоследовательность.
 */
std::vector<int64_t> find_highest_increasing_subsequence_quick(const std::vector<int64_t> &sequence) {
    if (sequence.empty()) {
        return sequence;
    }

    std::vector<int64_t> last_elements(sequence.size(), std::numeric_limits<int64_t>::max());
    std::vector<size_t> last_indexes(sequence.size(), 0);
    std::vector<size_t> best_last_indexes;
    last_indexes[0] = 0;
    last_elements[0] = sequence[0];
    size_t max_length{1};

    for (size_t i = 1; i < sequence.size(); ++i) {
        auto upper = std::lower_bound(last_elements.begin(), last_elements.end(), sequence[i]);
        if (sequence[i] < *upper) {
            *upper = sequence[i];
            auto distance = std::distance(last_elements.begin(), upper);
            assert(distance >= 0);
            last_indexes[distance] = i;
            if (distance + 1 > max_length) {
                max_length = (size_t) distance + 1;
                best_last_indexes = std::vector<size_t>(last_indexes.begin(), next(last_indexes.begin(), max_length));
            }
            for (auto j = 0; j < max_length - 1; ++j) {
                assert(last_elements[j] < last_elements[j + 1]);
            }
        }
    }

    std::vector<int64_t> result(max_length, 0);
    std::transform(best_last_indexes.begin(), best_last_indexes.end(), result.begin(),
                   [&sequence](const auto &element) {
                       return sequence[element];
                   });
    for (auto i = 0; i < result.size() - 1; ++i) {
        assert(result[i] < result[i + 1]);
    }
    return result;
}

/**
 * @brief Выбор используемой в данный момент реализации. По умолчанию - медленная.
 */
auto find_highest_increasing_subsequence = find_highest_increasing_subsequence_dummy;

// Начало тестов

void test_increasing_sequence() {
    std::vector<int64_t> sequence{0, 1, 2, 3};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    assert(sequence == subsequence);
}

void test_decreasing_sequence() {
    std::vector<int64_t> sequence{0, 1, 2, 3};
    std::reverse(sequence.begin(), sequence.end());
    auto subsequence = find_highest_increasing_subsequence(sequence);
    assert(subsequence.size() == 1);
}

void test_equal_numbers() {
    std::vector<int64_t> sequence(5, 0);
    auto subsequence = find_highest_increasing_subsequence(sequence);
    assert(subsequence.size() == 1);
}

void test_zigzag() {
    std::vector<int64_t> sequence{0, 1, 0, 1, 0, 1, 0, 1, 0};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{0, 1};
    assert(subsequence == true_result);
}

void test_one_element() {
    std::vector<int64_t> sequence{0};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{0};
    assert(subsequence == true_result);
}

void test_two_increasing_elements() {
    std::vector<int64_t> sequence{0, 1};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{0, 1};
    assert(subsequence == true_result);
}

void test_two_decreasing_elements() {
    std::vector<int64_t> sequence{1, 0};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    assert(subsequence.size() == 1);
}

void test_by_jungle_hunter() {
    std::vector<int64_t> sequence{0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{0, 2, 6, 9, 13, 15};
    assert(subsequence.size() == true_result.size());
}

void test_by_margus() {
    std::vector<int64_t> sequence{1, 9, 3, 8, 11, 4, 5, 6, 4, 19, 7, 1, 7};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{1, 3, 4, 5, 6, 7};
    assert(subsequence.size() == true_result.size());
}

void test_by_zzz() {
    std::vector<int64_t> sequence{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{1, 2, 3, 5, 8, 9};
    assert(subsequence.size() == true_result.size());
}

void test_empty() {
    std::vector<int64_t> sequence;
    auto subsequence = find_highest_increasing_subsequence(sequence);
    assert(subsequence.empty());
}

void test_negative_numbers() {
    std::vector<int64_t> sequence{0, -2, 1, -2, 2, 6, -1, 3, 2, 0, 2, 5, 6, 4, 6};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{-2, -1, 0, 2, 5, 6};
    assert(subsequence.size() == true_result.size());
}

void test_random_sequences() {
    std::random_device rd;
    std::uniform_int_distribution<int> uid(0, 50);
    for (auto i = 0; i < 100; ++i) {
        std::vector<int64_t> sequence(100, 0);
        for (auto j = 0; j < sequence.size(); ++j) {
            sequence[j] = uid(rd);
        }
        auto subsequence = find_highest_increasing_subsequence(sequence);
    }
    for (auto i = 0; i < 100; ++i) {
        std::vector<int64_t> sequence(6, 0);
        for (auto j = 0; j < sequence.size(); ++j) {
            sequence[j] = uid(rd);
        }
        auto subsequence = find_highest_increasing_subsequence(sequence);
    }
}

void test_compare_dummy_with_quick() {
    std::random_device rd;
    std::uniform_int_distribution<int> uid(0, 50);
    for (auto i = 0; i < 100; ++i) {
        std::vector<int64_t> sequence(100, 0);
        for (auto j = 0; j < sequence.size(); ++j) {
            sequence[j] = uid(rd);
        }
        auto subsequence1 = find_highest_increasing_subsequence_dummy(sequence);
        auto subsequence2 = find_highest_increasing_subsequence_quick(sequence);
        assert(subsequence1.size() == subsequence2.size());
    }
    for (auto i = 0; i < 100; ++i) {
        std::vector<int64_t> sequence(6, 0);
        for (auto j = 0; j < sequence.size(); ++j) {
            sequence[j] = uid(rd);
        }
        auto subsequence1 = find_highest_increasing_subsequence_dummy(sequence);
        auto subsequence2 = find_highest_increasing_subsequence_quick(sequence);
        assert(subsequence1.size() == subsequence2.size());
    }
}

void run_all_tests()
{
    test_increasing_sequence();
    test_decreasing_sequence();
    test_equal_numbers();
    test_zigzag();
    test_one_element();
    test_two_increasing_elements();
    test_two_decreasing_elements();
    test_by_jungle_hunter();
    test_by_margus();
    test_by_zzz();
    test_random_sequences();
    test_empty();
    test_negative_numbers();
    test_compare_dummy_with_quick();
}

// Конец тестов

int main(int argc, char *argv[]) {
    if (argc > 1)
    {
        if(std::string(argv[1]) == "quick")  // быстрая реализация
        {
            find_highest_increasing_subsequence = find_highest_increasing_subsequence_dummy;
        } else if(std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        } else if(std::string(argv[1]) == "test_for_quick")  // выбор быстрой реализации и запуск тестов
        {
            find_highest_increasing_subsequence = find_highest_increasing_subsequence_dummy;
            run_all_tests();
            return 0;
        }
    }

    // Чтение входных данных
    size_t N{0};
    std::cin >> N;
    std::vector<int64_t> sequence(N, 0);
    for (size_t i = 0; i < N; ++i) {
        std::cin >> sequence[i];
    }

    auto subsequence = find_highest_increasing_subsequence(sequence);

    // Вывод результата
    std::cout << subsequence.size() << std::endl;
    for (const auto &element: subsequence) {
        std::cout << element << " ";
    }

    return 0;
}