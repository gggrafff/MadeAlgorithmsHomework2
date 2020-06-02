/*
 * E. Циклические сдвиги
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 512 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * k-м циклическим сдвигом строки S называется строка, полученная перестановкой k первых символов строки S в конец строки.
 *
 * Рассмотрим все различные циклические сдвиги строки S и отсортируем их по возрастанию.
 * Требуется вычислить i-ю строчку этого массива.
 *
 * Например, для строки abacabac существует четыре различных циклических сдвига:
 * нулевой (abacabac), первый (bacabaca), второй (acabacab) и третий (cabacaba).
 * После сортировки по возрастанию получится такой массив: abacabac, acabacab, bacabaca, cabacaba.
 *
 * Входные данные
 * В первой строке входного файла записана строка S, длиной не более 100 000 символов с ASCII-кодами от 32 до 126.
 * Во второй строке содержится единственное целое число k (1 ≤ k ≤ 100 000).
 *
 * Выходные данные
 * В выходной файл выведите k-й по возрастанию циклический сдвиг строки S, или слово IMPOSSIBLE,
 * если такого сдвига не существует.
 *
 * Примеры
 * Входные данные
 * abacabac
 * 4
 * Выходные данные
 * cabacaba
 *
 * Входные данные
 * abacabac
 * 5
 * Выходные данные
 * IMPOSSIBLE
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
 * @brief Поиск циклических сдвигов строки.
 */
class CyclicShifter {
public:
    /**
     * @brief Конструктор ищет циклические сдвиги строки.
     * @param original Строка, для которой ищем циклические сдвиги.
     */
    explicit CyclicShifter(std::string original) : original_(std::move(original)) {
        build_shifts();
    }

    /**
     * @brief Получить k-й циклический сдвиг из упорядоченного списка сдвигов..
     * @param k Индекс циклического сдвига (с 0).
     * @return Циклический сдвиг строки.
     */
    std::optional<std::string> get_cyclic_shift(size_t k) {
        if (k >= classes_number_) {
            return std::nullopt;
        }
        if (k == 0) {
            return original_.substr(shifts_[0]) + original_.substr(0, shifts_[0]);
        }
        size_t last_class = classes_[shifts_[0]];
        size_t count = 0;
        for (size_t i = 1; i < shifts_.size(); ++i) {
            if (classes_[shifts_[i]] != last_class) {
                last_class = classes_[shifts_[i]];
                count += 1;
                if (count == k) {
                    return original_.substr(shifts_[i]) + original_.substr(0, shifts_[i]);
                }
            }
        }
        return std::nullopt;
    }

private:

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
     * @brief Конструирует циклические сдвиги.
     */
    void build_shifts() {
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
        shifts_.resize(original_.size(), 0);
        for (size_t i = 0; i < original_.size(); ++i) {
            auto position = positions[original_[i]]++;
            assert((position >= 0) && (position < shifts_.size()));
            shifts_[position] = i;  // suffixes_ будет хранить индексы начал отсортированных подстрок
        }
        // По этому массиву построим массив классов эквивалентности.
        // Одинаковые символы принадлежат одному классу.
        classes_ = std::vector<size_t>(original_.size());
        classes_number_ = 1;
        char last_char = original_[shifts_[0]];
        for (auto suf: shifts_) {
            assert((suf >= 0) && (suf < original_.size()));
            if (original_[suf] != last_char) {
                last_char = original_[suf];
                classes_number_ += 1;
            }
            assert((suf >= 0) && (suf < classes_.size()));
            classes_[suf] = classes_number_ - 1;
        }

        // нулевая итерация завершена
        // сортируем подстроки длиной 2 * cur_len = 2^k
        for (size_t cur_len = 1; cur_len <= original_.size(); cur_len *= 2) {
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
                sorted_by_second[i] = (shifts_[i] + original_.size() - cur_len) % original_.size();
            }
            // сортируем по первой половине
            // Чтобы произвести устойчивую сортировку по первым элементам пар, применим сортировку подсчетом за O(n).
            count = std::vector<size_t>(classes_.size());
            for (auto by2: sorted_by_second) {
                auto index = classes_[by2];
                assert((index >= 0) && (index < count.size()));
                count[index] += 1;
            }
            positions = calc_positions(count);
            for (size_t i = 0; i < sorted_by_second.size(); ++i) {
                auto position = positions[classes_[sorted_by_second[i]]]++;
                assert((position >= 0) && (position < shifts_.size()));
                shifts_[position] = sorted_by_second[i];
            }
            // Осталось пересчитать номера классов эквивалентности c,
            // пройдя по новой перестановке p и сравнивая соседние элементы (как пары двух чисел).
            std::vector<size_t> new_classes(original_.size(), 0);
            classes_number_ = 1;
            for (size_t i = 1; i < shifts_.size(); ++i) {
                auto mid1 = (shifts_[i] + cur_len) % original_.size();
                auto mid2 = (shifts_[i - 1] + cur_len) % original_.size();

                assert((shifts_[i] >= 0) && (shifts_[i] < classes_.size()));
                assert((shifts_[i - 1] >= 0) && (shifts_[i - 1] < classes_.size()));
                assert((mid1 >= 0) && (mid1 < classes_.size()));
                assert((mid2 >= 0) && (mid2 < classes_.size()));

                if ((classes_[shifts_[i]] != classes_[shifts_[i - 1]]) or (classes_[mid1] != classes_[mid2])) {
                    classes_number_++;
                }
                assert((shifts_[i] >= 0) && (shifts_[i] < new_classes.size()));
                new_classes[shifts_[i]] = classes_number_ - 1;
            }
            classes_ = new_classes;
        }
    }

    const size_t alphabet_size = 256;
    std::string original_;
    std::vector<size_t> shifts_;
    std::vector<size_t> classes_;
    size_t classes_number_{0};
};


// Начало тестов


void test_from_task_1() {
    const std::string original = "abacabac";
    CyclicShifter shifter(original);
    auto shift = shifter.get_cyclic_shift(3);
    assert(shift.has_value());
    assert(shift.value() == "cabacaba");
}

void test_from_task_2() {
    const std::string original = "abacabac";
    CyclicShifter shifter(original);
    auto shift = shifter.get_cyclic_shift(4);
    assert(!shift.has_value());
}

void test_nl() {
    const std::string original = "nl";
    CyclicShifter shifter(original);
    auto shift = shifter.get_cyclic_shift(0);
    assert(shift.has_value());
    assert(shift.value() == "ln");
    shift = shifter.get_cyclic_shift(1);
    assert(shift.has_value());
    assert(shift.value() == "nl");
    shift = shifter.get_cyclic_shift(2);
    assert(!shift.has_value());
}

void test_a() {
    const std::string original = "a";
    CyclicShifter shifter(original);
    auto shift = shifter.get_cyclic_shift(0);
    assert(shift.has_value());
    assert(shift.value() == "a");
    shift = shifter.get_cyclic_shift(1);
    assert(!shift.has_value());
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
        CyclicShifter shifter(original);
        auto shift = shifter.get_cyclic_shift(0);
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
        CyclicShifter shifter(original);
        auto shift = shifter.get_cyclic_shift(0);
    }
}


void run_all_tests() {
    test_from_task_1();
    test_from_task_2();
    test_random_big();
    test_random_small();
    test_nl();
    test_a();
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
    size_t k{0};
    std::cin >> k;
    CyclicShifter shifter(original);
    auto shift = shifter.get_cyclic_shift(k - 1);
    if (shift.has_value()) {
        std::cout << shift.value();
    } else {
        std::cout << "IMPOSSIBLE";
    }

    return 0;
}