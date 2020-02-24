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

#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <ctime>
#include <random>

struct DpElement {
    int64_t number {0};
    size_t length_subsequence {1};
    size_t parent {0};
};

std::vector<int64_t> find_highest_increasing_subsequence(const std::vector<int64_t>& sequence) {
    std::vector<DpElement> dp(sequence.size());
    std::transform(sequence.begin(), sequence.end(), dp.begin(), [](const auto& element){
        return DpElement{ element, 1, 0};
    });
    for (size_t i = 1; i < sequence.size(); ++i) {
        auto right = sequence[i];
        auto max_element = std::max_element(dp.begin(), next(dp.begin(), i), [&right](const auto& lhs, const auto& rhs){
            return ((lhs.length_subsequence < rhs.length_subsequence) && (rhs.number < right));
        });
        if (max_element->number < dp[i].number) {
            dp[i].length_subsequence = max_element->length_subsequence + 1;
            auto distance = std::distance(dp.begin(), max_element);
            assert(distance >= 0);
            dp[i].parent = static_cast<size_t>(distance);
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

void test_increasing_sequence()
{
    std::vector<int64_t> sequence{0, 1, 2, 3};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    assert(sequence == subsequence);
}

void test_decreasing_sequence()
{
    std::vector<int64_t> sequence{0, 1, 2, 3};
    std::reverse(sequence.begin(), sequence.end());
    auto subsequence = find_highest_increasing_subsequence(sequence);
    assert(subsequence.size() == 1);
}

void test_equals_number()
{
    std::vector<int64_t> sequence(5, 0);
    auto subsequence = find_highest_increasing_subsequence(sequence);
    assert(subsequence.size() == 1);
}

void test_zigzag()
{
    std::vector<int64_t> sequence{ 0, 1, 0, 1, 0, 1, 0, 1, 0};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{ 0, 1 };
    assert(subsequence == true_result);
}

void test_one_element()
{
    std::vector<int64_t> sequence{ 0};
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{ 0 };
    assert(subsequence == true_result);
}

void test_two_increasing_elements()
{
    std::vector<int64_t> sequence{ 0, 1 };
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{ 0, 1 };
    assert(subsequence == true_result);
}

void test_two_decreasing_elements()
{
    std::vector<int64_t> sequence{ 1, 0 };
    auto subsequence = find_highest_increasing_subsequence(sequence);
    assert(subsequence.size() == 1);
}

void test_by_jungle_hunter()
{
    std::vector<int64_t> sequence{ 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15 };
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{ 0, 2, 6, 9, 13, 15 };
    assert(subsequence.size() == true_result.size());
}

void test_by_margus()
{
    std::vector<int64_t> sequence{ 1, 9, 3, 8, 11, 4, 5, 6, 4, 19, 7, 1, 7 };
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{ 1, 3, 4, 5, 6, 7 };
    assert(subsequence.size() == true_result.size());
}

void test_by_zzz()
{
    std::vector<int64_t> sequence{ 3,1,4,1,5,9,2,6,5,3,5,8,9,7,9 };
    auto subsequence = find_highest_increasing_subsequence(sequence);
    std::vector<int64_t> true_result{ 1,2,3,5,8,9 };
    assert(subsequence.size() == true_result.size());
}

void test_random_sequences()
{
    std::random_device rd;
    std::uniform_int_distribution<int> uid(0, 50);
    for(auto i=0; i<100; ++i)
    {
        std::vector<int64_t> sequence(100, 0);
        for(auto j=0; j<100; ++j) {
            sequence[j] = uid(rd);
        }
        auto subsequence = find_highest_increasing_subsequence(sequence);
    }
}

int main(int argc, char *argv[]) {
    const bool is_test = true;
    if (is_test)
    {
        test_increasing_sequence();
        test_decreasing_sequence();
        test_equals_number();
        test_zigzag();
        test_one_element();
        test_two_increasing_elements();
        test_two_decreasing_elements();
        test_by_jungle_hunter();
        test_by_margus();
        test_by_zzz();
        test_random_sequences();
        return 0;
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
        std::cout << element<< " ";
    }

    return 0;
}