/*
 * A. Звезды
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Вася любит наблюдать за звездами. Но следить за всем небом сразу ему тяжело.
 * Поэтому он наблюдает только за частью пространства, ограниченной кубом размером n × n × n.
 * Этот куб поделен на маленькие кубики размером 1 × 1 × 1. Во время его наблюдений могут происходить следующие события:
 *     В каком-то кубике появляются или исчезают несколько звезд.
 *     К нему может заглянуть его друг Петя и поинтересоваться, сколько видно звезд в части пространства, состоящей из нескольких кубиков.
 *
 * Входные данные
 * Первая строка входного файла содержит натуральное число 1 ≤ n ≤ 128.
 * Координаты кубиков — целые числа от 0 до n - 1.
 * Далее следуют записи о происходивших событиях по одной в строке. В начале строки записано число m. Если m равно:
 *     1, то за ним следуют 4 числа — x, y, z (0 ≤ x, y, z < N) и
 *        k ( - 20000 ≤ k ≤ 20000) — координаты кубика и величина, на которую в нем изменилось количество видимых звезд;
 *     2, то за ним следуют 6 чисел — x1, y1, z1, x2, y2, z2 (0 ≤ x1 ≤ x2 < N, 0 ≤ y1 ≤ y2 < N, 0 ≤ z1 ≤ z2 < N),
 *        которые означают, что Петя попросил подсчитать количество звезд в кубиках (x, y, z)
 *        из области: x1 ≤ x ≤ x2, y1 ≤ y ≤ y2, z1 ≤ z ≤ z2;
 *     3, то это означает, что Васе надоело наблюдать за звездами и отвечать на вопросы Пети.
 *        Эта запись встречается во входном файле только один раз и будет последней.
 * Количество записей во входном файле не больше 100 002.
 *
 * Выходные данные
 * Для каждого Петиного вопроса выведите искомое количество звезд.
 *
 * Пример
 * Входные данные
 * 2
 * 2 1 1 1 1 1 1
 * 1 0 0 0 1
 * 1 0 1 0 3
 * 2 0 0 0 0 0 0
 * 2 0 0 0 0 1 0
 * 1 0 1 0 -2
 * 2 0 0 0 1 1 1
 * 3
 * Выходные данные
 * 0
 * 1
 * 4
 * 2
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>


// Начало определения многомерного вектора

template<size_t dim, typename T>
struct multidimensional_vector {
    typedef std::vector<typename multidimensional_vector<dim - 1, T>::type> type;
};

template<typename T>
struct multidimensional_vector<0, T> {
    typedef T type;
};

template<size_t dim, typename T>
using multi_vector = typename multidimensional_vector<dim, T>::type;


// Конец определения многомерного вектора


/**
 * @brief Реализация трёхмерного дерева Фенвика для чисел.
 * @tparam NumberType Тип элементов, хранящихся в дереве Фенвика.
 * @details Подробнее: https://neerc.ifmo.ru/wiki/index.php?title=Дерево_Фенвика
 */
template<typename NumberType>
class FenwickTree3D {
public:
    /**
     * @brief Конструирует дерево Фенвика для куба. Заполняет нулями.
     * @param xyz_size Размер стороны куба.
     */
    explicit FenwickTree3D(const size_t xyz_size) {
        elements_.resize(xyz_size, multi_vector<2, NumberType>(xyz_size, multi_vector<1, NumberType>(xyz_size, 0)));
        fenwick_sums_.resize(xyz_size, multi_vector<2, NumberType>(xyz_size, multi_vector<1, NumberType>(xyz_size, 0)));
    }

    /**
     * @brief Конструирует дерево Фенвика из массива чисел.
     * @param numbers Трёхмерный массив чисел, по которым хотим делать запросы.
     */
    explicit FenwickTree3D(const multi_vector<3, NumberType> &numbers) {
        elements_ = numbers;
        fenwick_sums_.resize(elements_.size(),
                             multi_vector<2, NumberType>(elements_.back().size(),
                                                         multi_vector<1, NumberType>(elements_.back().back().size(),
                                                                                     0)));
        for (size_t i = 0; i < elements_.size(); i = (i | (i + 1))) {
            for (size_t j = 0; j < elements_.back().size(); j = (j | (j + 1))) {
                for (size_t k = 0; k < elements_.back().back().size(); k = (k | (k + 1))) {
                    add_to_element(i, j, k, elements_[i][j][k]);
                }
            }
        }
    }

    /**
     * @brief Запрос префиксной суммы в трёхмерном пространстве.
     * @param x Индекс по оси x, по который включительно нужна префиксная сумма.
     * @param y Индекс по оси y, по который включительно нужна префиксная сумма.
     * @param z Индекс по оси z, по который включительно нужна префиксная сумма.
     * @return Результат запроса.
     */
    NumberType get_prefix_sum(const int64_t x, const int64_t y, const int64_t z) {
        if ((x < 0) || (y < 0) || (z < 0)) {
            return 0;
        }
        assert(z >= 0 && z < fenwick_sums_.size());
        assert(y >= 0 && y < fenwick_sums_.back().size());
        assert(x >= 0 && x < fenwick_sums_.back().back().size());
        NumberType result = 0;
        for (auto i = x; i >= 0; i = fenwick(i) - 1) {
            for (auto j = y; j >= 0; j = fenwick(j) - 1) {
                for (auto k = z; k >= 0; k = fenwick(k) - 1) {
                    result += fenwick_sums_[i][j][k];
                }
            }
        }
        return result;
    }

    /**
     * @brief Запрос суммы на параллелепипеде в трёхмерном пространстве.
     * @details Параллелепипед задаётся координатами двух углов.
     * @param x1 Координата x первого угла параллелепипеда.
     * @param y1 Координата y первого угла параллелепипеда.
     * @param z1 Координата z первого угла параллелепипеда.
     * @param x2 Координата x второго угла параллелепипеда.
     * @param y2 Координата y второго угла параллелепипеда.
     * @param z2 Координата z второго угла параллелепипеда.
     * @return Результат запроса.
     */
    NumberType get_sum(int64_t x1, int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2) {
        if (x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
            std::swap(z1, z2);
        }
        assert((x2 >= x1) && (y2 >= y1) && (z2 >= z1));
        assert(z1 >= 0 && z1 < fenwick_sums_.size());
        assert(z2 >= 0 && z2 < fenwick_sums_.size());
        assert(y1 >= 0 && y1 < fenwick_sums_.back().size());
        assert(y2 >= 0 && y2 < fenwick_sums_.back().size());
        assert(x1 >= 0 && x1 < fenwick_sums_.back().back().size());
        assert(x2 >= 0 && x2 < fenwick_sums_.back().back().size());

        auto value1 =
                +get_prefix_sum(x2, y2, z2)
                - get_prefix_sum(x1 - 1, y2, z2)
                - get_prefix_sum(x2, y1 - 1, z2)
                + get_prefix_sum(x1 - 1, y1 - 1, z2);  // без учёта оси z

        auto value2 =
                +get_prefix_sum(x2, y2, z1 - 1)
                - get_prefix_sum(x1 - 1, y2, z1 - 1)
                - get_prefix_sum(x2, y1 - 1, z1 - 1)
                + get_prefix_sum(x1 - 1, y1 - 1, z1 - 1);  // лишнее учтённое по оси z

        return value1 - value2;
    }

    /**
     * @brief Увеличить элемент.
     * @param x Координата x изменяемого элемента.
     * @param y Координата y изменяемого элемента.
     * @param z Координата z изменяемого элемента.
     * @param value Значение, на которое увеличиваем.
     */
    void add_to_element(const size_t x, const size_t y, const size_t z, const NumberType value) {
        assert(z >= 0 && z < elements_.size());
        assert(y >= 0 && y < elements_.back().size());
        assert(x >= 0 && x < elements_.back().back().size());
        for (size_t i = x; i < fenwick_sums_.size(); i = (i | (i + 1))) {
            for (size_t j = y; j < fenwick_sums_.back().size(); j = (j | (j + 1))) {
                for (size_t k = z; k < fenwick_sums_.back().back().size(); k = (k | (k + 1))) {
                    fenwick_sums_[i][j][k] += value;
                }
            }
        }
    }

    /**
     * @brief Установить новое значение элемента.
     * @param x Координата x изменяемого элемента.
     * @param y Координата y изменяемого элемента.
     * @param z Координата z изменяемого элемента.
     * @param value Значение, которое устанавливаем.
     */
    void set_element(const size_t x, const size_t y, const size_t z, const int64_t value) {
        assert(z >= 0 && z < elements_.size());
        assert(y >= 0 && y < elements_.back().size());
        assert(x >= 0 && x < elements_.back().back().size());
        const auto diff = value - elements_[x][y][z];
        elements_[x][y][z] = value;
        add_to_element(x, y, z, diff);
    }

private:
    /**
     * @brief Функция Фенвика.
     * @tparam T Числовой тип данных.
     * @param i Аргумент функции Фенвика.
     * @return Результат функции Фенвика.
     */
    template<typename T>
    static T fenwick(const T i) {
        return (i & (i + 1));
    }

    multi_vector<3, NumberType> fenwick_sums_;
    multi_vector<3, NumberType> elements_;
};


// Начало тестов

void test_multi_vector() {
    multi_vector<2, int> vec;
    for (auto i = 0; i < 5; ++i) {
        vec.emplace_back();
        for (auto j = 0; j < 5; ++j) {
            vec.back().push_back(i * 5 + j);
        }
    }
    for (auto i = 0; i < 5; ++i) {
        for (auto j = 0; j < 5; ++j) {
            assert(vec[i][j] == i * 5 + j);
        }
    }
}

void test_from_task() {
    FenwickTree3D<int64_t> tree(2);
    assert(tree.get_sum(1, 1, 1, 1, 1, 1) == 0);
    tree.add_to_element(0, 0, 0, 1);
    tree.add_to_element(0, 1, 0, 3);
    assert(tree.get_sum(0, 0, 0, 0, 0, 0) == 1);
    assert(tree.get_sum(0, 0, 0, 0, 1, 0) == 4);
    tree.add_to_element(0, 1, 0, -2);
    assert(tree.get_sum(0, 0, 0, 1, 1, 1) == 2);
}

void run_all_tests() {
    test_multi_vector();
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
    FenwickTree3D<int64_t> tree(n);
    uint16_t operation{3};
    std::cin >> operation;
    while (operation != 3) {
        switch (operation) {
            case 1: {
                size_t x{0}, y{0}, z{0};
                int64_t k;
                std::cin >> x >> y >> z >> k;
                tree.add_to_element(x, y, z, k);
            }
                break;
            case 2: {
                size_t x1{0}, y1{0}, z1{0};
                size_t x2{0}, y2{0}, z2{0};
                std::cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
                std::cout << tree.get_sum(x1, y1, z1, x2, y2, z2) << "\n";
            }
                break;
            default:
                break;
        }
        std::cin >> operation;
    }

    return 0;
}