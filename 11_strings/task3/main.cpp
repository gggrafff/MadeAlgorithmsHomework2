/*
 * C. Количество подстрок
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 512 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Вычислите количество различных подстрок строки s.
 *
 * Входные данные
 * Единственная строка входного файла содержит строку s (1 ≤ |s| ≤ 400 000). Строка состоит из строчных латинских букв.
 *
 * Выходные данные
 * Выведите одно число — ответ на задачу.
 * Пример
 * Входные данные
 * ababb
 * Выходные данные
 * 11
 */

 /*
  * Подробнее про суффиксный массив:
  * https://web.stanford.edu/class/cs97si/suffix-array.pdf
  * http://opentrains.mipt.ru/zksh/files/zksh2015/lectures/Suffix_Array_lecture_A.pdf
  * Подробнее про поиск количества различных подстрок:
  * https://codeforces.com/edu/course/2/lesson/2/5
  * https://www.quora.com/Given-a-string-how-do-I-find-the-number-of-distinct-substrings-of-the-string/answer/Siddharth-Subramanian
  * https://ankurkkhurana.me/2012/07/01/suffix-arrays-a-simple-tutorial/
  * https://stackoverflow.com/questions/2560262/generate-all-unique-substrings-for-given-string/18623477#18623477
  */


#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>
#include <utility>
#include <optional>


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
    std::vector<size_t> get_lcp_neighboring() {
        if (lcp_neighboring_.empty()) {
            build_lcp_neighboring();
        }
        return lcp_neighboring_;
    }

    /**
     * @brief Поиск подстроки в исходной строке с помощью суффиксного массива.
     * @param substring Искомая подстрока.
     * @return Индекс начала подстроки в строке, если она найдена.
     */
    std::optional<size_t> find_substring(const std::string& substring) {
        /*
         * Заметим, что любая подстрока — это префикс какого-то суффикса.
         * Давайте рассмотрим суффиксный массив строки s.
         * Пусть длина строки p равна |p|.
         * Отрежем от каждого суффикса первые |p| символов, получим все подстроки длины |p| в отсортированном порядке.
         * Теперь среди них нам нужно найти строку p. Это можно сделать обычным бинарным поиском.
         * Время работы бинарного поиска будет O(|p|logn), потому что каждое сравнение строк работает за время O(|p|).
         */
        auto& original = original_;
        auto found = std::lower_bound(suffixes_.begin(), suffixes_.end(), substring, [&original](auto& suf, auto& rhs){
            return (original.compare(suf, std::string::npos, rhs) < 0);
        });
        if (found == suffixes_.end()){
            return std::nullopt;
        }
        if (original.substr(*found, substring.size()) != substring){
            return std::nullopt;
        }
        return *found;
    }

    /**
     * @brief Подсчёт количества различных подстрок в оригинальной строке.
     * @return Количество различных подстрок.
     */
    uint64_t count_different_substrings(){
        /*
         * Вспомним, что любая строка — это префикс какого-то суффикса, таким образом нам нужно посчитать,
         * сколько различных префиксов у суффиксов строки s. Давайте перебирать суффиксы строки s в
         * отсортированном порядке. Посмотрим на очередной суффикс. Давайте посмотрим, сколько новых префиксов
         * он нам добавляет. Общее число префиксов равно длине суффикса. Чтобы узнать, какие из них уже встречались
         * в предыдущих строках, нужно просто посмотреть на LCP этого суффикса с предыдущим. Таким образом, мы
         * можем за O(1) понять, сколько новых префиксов добавляет каждый суффикс. Сложив это вместе, получим ответ.
         */
        if (lcp_neighboring_.empty()) {
            build_lcp_neighboring();
        }
        uint64_t count = original_.size() - 1 - suffixes_[1];
        for(size_t i = 2; i < suffixes_.size(); ++i) {
            count += original_.size() - 1 - suffixes_[i] - lcp_neighboring_[i - 1];
        }
        return count;
    }

    /**
     * @brief Подсчёт количества различных подстрок в оригинальной строке.
     * @return Количество различных подстрок.
     */
    uint64_t count_different_substrings_2(){
        /*
         * M = n * (n − 1) / 2 - sum(lcp[i])
         */
        if (lcp_neighboring_.empty()) {
            build_lcp_neighboring();
        }
        uint64_t n = original_.size();
        uint64_t count = n * (n - 1) / 2;
        for( auto lcp: lcp_neighboring_) {
            count -= lcp;
        }
        return count;
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
    const std::string original = "ababb";
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    assert(count_substrings == 11);
}

void test_aaba() {
    const std::string original = "aaba";
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    assert(count_substrings == 8);
}

void test_one_symbol() {
    const std::string original = "a";
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    assert(count_substrings == 1);
}

void test_ababba() {
    const std::string original = "ababba";
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    assert(count_substrings == 15);
}

void test_mmuc() {
    const std::string original = "mmuc";
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    assert(count_substrings == 9);
}

void test_xmnnnuu() {
    const std::string original = "xmnnnuu";
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    assert(count_substrings == 24);
}

void test_nnnn() {
    const std::string original = "nnnn";
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    assert(count_substrings == 4);
}

void test_banana() {
    const std::string original = "banana";
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    assert(count_substrings == 15);
}

void test_random_big() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> big_numbers_generator(0, 1000);
    for (size_t test_no=0; test_no<1000; ++test_no) {
        std::string original;
        auto length = big_numbers_generator(rd) + 1;
        for (size_t i = 0; i < length; ++i) {
            original += int('a') + big_numbers_generator(rd) % 26;
        }
        SuffixArray suffix_array(original);
        auto count1 = suffix_array.count_different_substrings();
        auto count2 = suffix_array.count_different_substrings_2();
        assert(count1 == count2);
    }
}

void test_random_small() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> small_numbers_generator(0, 10);
    for (size_t test_no=0; test_no<1000; ++test_no) {
        std::string original;
        auto length = small_numbers_generator(rd) + 1;
        for (size_t i = 0; i < length; ++i) {
            original += int('a') + small_numbers_generator(rd) % 3;
        }
        SuffixArray suffix_array(original);
        auto count1 = suffix_array.count_different_substrings();
        auto count2 = suffix_array.count_different_substrings_2();
        assert(count1 == count2);
    }
}

void test_abcde() {
    const std::string original = "abcde";
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    assert(count_substrings == 15);
}

void test_aaaaa() {
    const std::string original = "aaaaa";
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    assert(count_substrings == 5);
}

void test_abacabadabacaba() {
    const std::string original = "abacabadabacaba";
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    assert(count_substrings == 85);
}


void run_all_tests() {
    test_from_task();
    test_aaba();
    test_one_symbol();
    test_ababba();
    test_mmuc();
    test_xmnnnuu();
    test_nnnn();
    test_banana();
    test_random_big();
    test_random_small();
    test_abcde();
    test_aaaaa();
    test_abacabadabacaba();
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
    std::string original;
    std::cin >> original;
    SuffixArray suffix_array(original);
    auto count_substrings = suffix_array.count_different_substrings();
    std::cout << count_substrings;

    return 0;
}