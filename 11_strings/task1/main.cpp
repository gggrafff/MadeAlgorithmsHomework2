/*
 * A. Суффиксный массив
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 512 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Постройте суффиксный массив для заданной строки s, для каждых двух соседних суффиксов найдите длину максимального общего префикса.
 *
 * Входные данные
 * Первая строка входного файла содержит строку s (1 ≤ |s| ≤ 400 000). Строка состоит из строчных латинских букв.
 *
 * Выходные данные
 * В первой строке выведите |s| различных чисел — номера первых символов суффиксов строки s так, чтобы соответствующие суффиксы были упорядочены в лексикографически возрастающем порядке. Во второй строке выведите |s| - 1 чисел — длины наибольших общих префиксов.
 *
 * Пример
 * Входные данные
 * ababb
 * Выходные данные
 * 1 3 5 2 4
 * 2 0 1 1
 */

/*
 * Подробнее про построение суффиксного массива:
 * https://neerc.ifmo.ru/wiki/index.php?title=Суффиксный_массив
 * https://neerc.ifmo.ru/wiki/index.php?title=Построение_суффиксного_массива_с_помощью_стандартных_методов_сортировки
 * https://neerc.ifmo.ru/wiki/index.php?title=Алгоритм_цифровой_сортировки_суффиксов_циклической_строки
 * Подробнее про LCP:
 * https://neerc.ifmo.ru/wiki/index.php?title=Алгоритм_Касаи_и_др.
 */

#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>
#include <utility>

/**
 * Класс для построения и использования суффиксного массива строки.
 */
class SuffixArray {
public:
    /**
     * @brief Конструирует суффиксный массив для строки.
     * @param original Строка, для которой строим суффиксный массив.
     */
    explicit SuffixArray(std::string original) : original_(std::move(original)) {
        build_suffix_array();
    }

    /**
     * @brief Подсчитывает lcp для соседних суффиксов в суффиксном массиве.
     * @details Алгоритм Касаи, Аримуры, Арикавы, Ли, Парка
     */
    void build_lcp_neighboring() {
        /*
         * Нам не нужно сравнивать все символы, когда мы вычисляем LCP между суффиксом Si и его соседним суффиксом
         * в массиве Suf^−1. Чтобы вычислить LCP всех соседних суффиксов в массиве Suf^−1 эффективно,
         * будем рассматривать суффиксы по порядку начиная с S1 и заканчивая Sn.
         */
        auto len = original_.size();
        lcp_neighboring_.resize(len, 0);
        std::vector<size_t> pos(len);   // pos[] — массив, обратный массиву suf
        for (size_t i = 0; i < len; ++i) {
            pos[suffixes_[i]] = i;
        }
        size_t k = 0;
        for (size_t i = 0; i < len; ++i) {
            if (k > 0) {
                k -= 1;
            }
            if (pos[i] == len - 1) {
                lcp_neighboring_[len - 1] = 0;
                k = 0;
                continue;
            } else {
                auto j = suffixes_[pos[i] + 1];
                while ((std::max(i + k, j + k) < len) && (original_[i + k] == original_[j + k])) {
                    k += 1;
                }
                lcp_neighboring_[pos[i]] = k;
            }
        }
        lcp_neighboring_.erase(std::prev(lcp_neighboring_.end()));
    }

    /**
     * @brief Получить суффиксный массив.
     * @return Суффиксный массив (массив индексов).
     */
    std::vector<size_t> get_suffixes() const {
        return suffixes_;
    }

    /**
     * @brief Получить lcp всех соседних суффиксов.
     * @return Массив со значениями lcp для соседних суффиксов.
     */
    std::vector<size_t> get_lcp_neighboring() const {
        return lcp_neighboring_;
    }

private:
    /* преобразует масив count, так что
     теперь он содержит позиции в массиве suffs с которых
     необходимо вставлять подстроки, начинающиеся с соответствующих символов */
    /**
     * @brief Часть сортировки подсчётом.
     * @brief Из массива количеств объектов формирует массив начал их позиций после сортировки.
     * @details Из массива количеств формирует массив, который содержит позиции в суффиксном массиве,
     * @details с которых необходимо вставлять подстроки, начинающиеся с соответствующих символов
     * @param count Массив количеств объектов (результат первого шага сортировки подсчётом).
     * @return Массив начал позиций объектов после сортировки.
     */
    std::vector<size_t> calc_positions(const std::vector<size_t> &count) {
        std::vector<size_t> result;
        result.push_back(0);
        for (size_t i = 0; i < count.size() - 1; ++i) {
            result.push_back(result.back() + count[i]);
        }
        return result;
    }

    /**
     * @brief Конструирует суффиксный массив.
     */
    void build_suffix_array() {
        // добавим к строке символ, который меньше любого другого символа в строке
        original_ += terminator_;

        // нулевая итерация
        // На нулевой итерации отсортируем циклические подстроки длины 1, т.е. первые символы строк, и разделим
        // их на классы эквивалентности (одинаковые символы должны быть отнесены к одному классу эквивалентности).
        std::vector<size_t> count(alphabet_size, 0);
        for (auto ch: original_) {  // первый шаг сортировки подсчётом
            count[ch] += 1;  // считаем, сколько символов каждого класса (типа) встречается в строке
        }
        // При помощи сортировки подсчетом построим массив,
        // содержащий номера суффиксов, отсортированных в лексикографическом порядке.
        auto positions = calc_positions(count);
        suffixes_.resize(original_.size(), 0);
        for (size_t i = 0; i < original_.size(); ++i) {
            auto position = positions[original_[i]]++;
            assert((position >= 0) && (position < suffixes_.size()));
            suffixes_[position] = i;  // suffixes_ будет хранить индексы начал отсортированных подстрок
        }
        // По этому массиву построим массив классов эквивалентности.
        // Одинаковые символы принадлежат одному классу.
        std::vector<size_t> classes(original_.size());
        size_t classes_number = 0;
        char last_char = terminator_;
        for (auto suf: suffixes_) {
            assert((suf >= 0) && (suf < original_.size()));
            if (original_[suf] != last_char) {
                last_char = original_[suf];
                classes_number += 1;
            }
            assert((suf >= 0) && (suf < classes.size()));
            classes[suf] = classes_number;
        }

        // нулевая итерация завершена
        // сортируем подстроки длиной 2 * cur_len = 2^k
        size_t cur_len = 1;
        while (cur_len <= original_.size()) {
            /*
             * Отсортируем подстроки длины 2^k по данным парам и запишем порядок в массив p.
             * Воспользуемся здесь приёмом, на котором основана цифровая сортировка:
             * отсортируем пары сначала по вторым элементам, а затем по первым (устойчивой сортировкой).
             * Однако вторые элементы уже упорядочены — этот порядок задан в массиве от предыдущей итерации.
             * Тогда, чтобы получить порядок пар по вторым элементам, надо от каждого элемента массива p отнять 2^(k − 1)
             * (p даёт упорядочение подстрок длины 2^(k − 1), и при переходе к строке вдвое большей длины эти подстроки
             * становятся их вторыми половинками, поэтому от позиции второй половинки отнимается длина первой половинки).
             */
            // сортируем по второй половине подстроки
            std::vector<size_t> sorted_by_second(original_.size());
            for (size_t i = 0; i < sorted_by_second.size(); ++i) {
                sorted_by_second[i] = (suffixes_[i] + original_.size() - cur_len) % original_.size();
            }
            // сортируем по первой половине
            // Чтобы произвести устойчивую сортировку по первым элементам пар, применим сортировку подсчетом за O(n).
            count = std::vector<size_t>(classes.size(), 0);
            for (auto by2: sorted_by_second) {
                auto index = classes[by2];
                assert((index >= 0) && (index < count.size()));
                count[index] += 1;
            }
            positions = calc_positions(count);
            for (size_t i = 0; i < sorted_by_second.size(); ++i) {
                auto position = positions[classes[sorted_by_second[i]]]++;
                assert((position >= 0) && (position < suffixes_.size()));
                suffixes_[position] = sorted_by_second[i];
            }
            // Осталось пересчитать номера классов эквивалентности c,
            // пройдя по новой перестановке p и сравнивая соседние элементы (как пары двух чисел).
            std::vector<size_t> new_classes(original_.size());
            classes_number = 0;
            for (size_t i = 1; i < original_.size(); ++i) {
                auto mid1 = (suffixes_[i] + cur_len) % original_.size();
                auto mid2 = (suffixes_[i - 1] + cur_len) % original_.size();

                assert((suffixes_[i] >= 0) && (suffixes_[i] < classes.size()));
                assert((suffixes_[i - 1] >= 0) && (suffixes_[i - 1] < classes.size()));
                assert((mid1 >= 0) && (mid1 < classes.size()));
                assert((mid2 >= 0) && (mid2 < classes.size()));

                if ((classes[suffixes_[i]] != classes[suffixes_[i - 1]]) or (classes[mid1] != classes[mid2])) {
                    classes_number++;
                }
                assert((suffixes_[i] >= 0) && (suffixes_[i] < new_classes.size()));
                new_classes[suffixes_[i]] = classes_number;
            }
            classes = new_classes;
            cur_len *= 2;
        }
    }

    const size_t alphabet_size = 256;
    const char terminator_{'\x00'};
    std::string original_;
    std::vector<size_t> suffixes_;
    std::vector<size_t> lcp_neighboring_;
};


// Начало тестов


void test_from_task() {
    std::string original = "ababb";
    SuffixArray suffix_array(original);
    auto suffixes = suffix_array.get_suffixes();
    assert((suffixes == std::vector<size_t>{5, 0, 2, 4, 1, 3}));
    suffix_array.build_lcp_neighboring();
    auto lcp = suffix_array.get_lcp_neighboring();
    assert((lcp == std::vector<size_t>{0, 2, 0, 1, 1}));
}

void test_abacaba() {
    std::string original = "abacaba";
    SuffixArray suffix_array(original);
    auto suffixes = suffix_array.get_suffixes();
    assert((suffixes == std::vector<size_t>{7, 6, 4, 0, 2, 5, 1, 3}));
    suffix_array.build_lcp_neighboring();
    auto lcp = suffix_array.get_lcp_neighboring();
    assert((lcp == std::vector<size_t>{0, 1, 3, 1, 0, 2, 0}));
}

void test_aabaaca() {
    std::string original = "aabaaca";
    SuffixArray suffix_array(original);
    auto suffixes = suffix_array.get_suffixes();
    assert((suffixes == std::vector<size_t>{7, 6, 0, 3, 1, 4, 2, 5}));
    suffix_array.build_lcp_neighboring();
    auto lcp = suffix_array.get_lcp_neighboring();
    assert((lcp == std::vector<size_t>{0, 1, 2, 1, 1, 0, 0}));
}

void test_random_life_or_death() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> big_numbers_generator(0, 1000);
    for (size_t test_no=0; test_no<1000; ++test_no) {
        std::string original;
        auto length = big_numbers_generator(rd);
        for (size_t i = 0; i < length; ++i) {
            original += int('a') + big_numbers_generator(rd) % 26;
        }
        SuffixArray suffix_array(original);
        auto suffixes = suffix_array.get_suffixes();
        suffix_array.build_lcp_neighboring();
        auto lcp = suffix_array.get_lcp_neighboring();
    }
}


void run_all_tests() {
    test_abacaba();
    test_aabaaca();
    test_from_task();
    test_random_life_or_death();
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
    std::string str;
    std::cin >> str;
    assert(!str.empty());
    SuffixArray suffix_array(str);
    auto sufs = suffix_array.get_suffixes();
    assert(sufs.size() == str.size() + 1);
    for (size_t i = 1; i < sufs.size(); ++i) {
        std::cout << sufs[i] + 1 << " ";
    }
    std::cout << std::endl;
    suffix_array.build_lcp_neighboring();
    auto lcp = suffix_array.get_lcp_neighboring();
    assert(lcp.size() == str.size());
    for (size_t i = 1; i < lcp.size(); ++i) {
        std::cout << lcp[i] << " ";
    }

    return 0;
}