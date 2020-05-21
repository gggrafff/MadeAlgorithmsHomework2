/*
 * B. Множественный поиск
 * ограничение по времени на тест 3 секунды
 * ограничение по памяти на тест 1024 мегабайта
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дан массив строк s i и строка t. Требуется для каждой строки s i определить, встречается ли она в t как подстрока.
 *
 * Входные данные
 * Первая строка входного файла содержит целое число n — число элементов в s (1 ≤ n ≤ 10^6).
 * Следующие n строк содержат по одной строке s i. Сумма длин всех строк из s не превосходит 10^6.
 * Последняя строка входного файла содержит t (1 ≤ t ≤ 10^6). Все строки состоят из строчных латинских букв.
 *
 * Выходные данные
 * Для каждой строки si выведите «YES», если она встречается в t и «NO» в противном случае.
 * Строки нумеруются в порядке появления во входном файле.
 *
 * Пример
 * Входные данные
 * 3
 * abc
 * abcdr
 * abcde
 * xabcdef
 * Выходные данные
 * YES
 * NO
 * YES
 */

 /*
  * Подробнее про поиск подстрок:
  * https://codeforces.com/edu/course/2/lesson/2/3
  * https://neerc.ifmo.ru/wiki/index.php?title=Алгоритм_поиска_подстроки_в_строке_с_помощью_суффиксного_массива
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
    std::vector<size_t> get_lcp_neighboring() const {
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
    const std::string original = "xabcdef";
    SuffixArray suffix_array(original);
    auto pos1 = suffix_array.find_substring("abc");
    assert(pos1.has_value());
    assert(pos1.value() == 1);
    auto pos2 = suffix_array.find_substring("abcdr");
    assert(!pos2.has_value());
    auto pos3 = suffix_array.find_substring("abcde");
    assert(pos3.has_value());
    assert(pos3.value() == 1);
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
    size_t n{0};
    std::cin >> n;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::vector<std::string> substrings;
    for (size_t i=0; i< n; ++i) {
        std::string substring;
        std::cin >> substring;
        substrings.push_back(substring);
    }
    std::string original;
    std::cin >> original;
    SuffixArray suffix_array(original);
    for(auto& substring: substrings) {
        auto pos = suffix_array.find_substring(substring);
        if (pos.has_value()) {
            std::cout << "YES\n";
        } else {
            std::cout << "NO\n";
        }
    }

    return 0;
}