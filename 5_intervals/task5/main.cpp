/*
 * E. Криптография
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод crypto.in
 * вывод crypto.out
 *
 * Задано n матриц A1, A2, ..., An размера 2 × 2.
 * Необходимо для нескольких запросов вычислить произведение матриц Ai, Ai + 1, ..., Aj.
 * Все вычисления производятся по модулю r.
 *
 * Входные данные
 * Первая строка входного файла содержит числа r (1 ≤ r ≤ 10 000), n (1 ≤ n ≤ 200 000) и m (1 ≤ m ≤ 200 000).
 * Следующие n блоков по две строки содержащие по два числа в строке — описания матриц.
 * Затем следуют m пар целых чисел от 1 до n, запросы на произведение на отрезке.
 * Выходные данные
 * Выведите m блоков по две строки,по два числа в каждой — произведения на отрезках.
 * Разделяйте блоки пустой строкой. Все вычисления производятся по модулю r
 * Пример
 * Входные данные
 * 3 4 4
 * 0 1
 * 0 0
 *
 * 2 1
 * 1 2
 *
 * 0 0
 * 0 2
 *
 * 1 0
 * 0 2
 *
 * 1 4
 * 2 3
 * 1 3
 * 2 2
 *
 * Выходные данные
 * 0 2
 * 0 0
 *
 * 0 2
 * 0 1
 *
 * 0 1
 * 0 0
 *
 * 2 1
 * 1 2
 */

#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>
#include <set>
#include <functional>

/**
 * @brief Реализация дерева отрезков для запросов на отрезке по некоммутативной операции без изменения дерева.
 * @tparam ElementType Тип элементов, хранящихся в дереве.
 */
template<typename ElementType>
class SegmentTree {
public:
    /**
     * @brief Конструирует дерево отрезков.
     * @param elements Исходный массив, на котором будем производить запросы.
     * @param query_operation Операция запроса.
     * @param neutral_element Нейтральный элемент для операции запроса.
     */
    explicit SegmentTree(
            const std::vector<ElementType> &elements,
            std::function<ElementType(const ElementType &lhs, const ElementType &rhs)> query_operation,
            const ElementType &neutral_element) :
            query_operation_{query_operation}, query_operation_neutral_element_{neutral_element} {
        size_t n = round_pow2(elements.size());  // удобно, когда количество элементов - степень двойки
        tree_.resize(n * 2, query_operation_neutral_element_);
        std::copy(elements.rbegin(), elements.rend(), std::next(tree_.rbegin(), n - elements.size()));
        for (auto i = n - 1; i > 0; --i) {
            tree_[i] = query_operation_(tree_[2 * i], tree_[2 * i + 1]);
        }
    }

    /**
     * @brief Выполнить запрос на отрезке.
     * @param l Левая граница отрезка. Индекс.
     * @param r Правая граница отрезка. Индекс.
     * @return Результат запроса.
     */
    ElementType query(size_t l, size_t r) const {
        if (l > r) {
            std::swap(l, r);
        }
        auto n = tree_.size() / 2;
        l += n;
        r += n;
        ElementType result_left{query_operation_neutral_element_};
        ElementType result_right{query_operation_neutral_element_};
        while (l <= r) {
            if (l % 2 == 1) {
                result_left = query_operation_(result_left, tree_[l]);
                l += 1;
            }
            if (r % 2 == 0) {
                result_right = query_operation_(tree_[r], result_right);
                r -= 1;
            }
            l /= 2;
            r /= 2;
        }
        return query_operation_(result_left, result_right);
    }

private:
    std::vector<ElementType> tree_;
    std::function<ElementType(const ElementType &lhs, const ElementType &rhs)> query_operation_{std::min<ElementType>};
    ElementType query_operation_neutral_element_{ElementType()};

    /**
     * @brief Определяет наименьшую степень двойки, не меньшую заданного числа.
     * @details Используется знание о представлении чисел с плавающей точкой в памяти.
     * @details Подробнее: https://ru.wikipedia.org/wiki/Число_двойной_точности
     * @param number Число, которое нужно округлить вверх до стпени двойки.
     * @return Число, равное степени двойки. Минимальное, но не меньше, чем number.
     */
    static size_t round_pow2(size_t number) {
        double x = static_cast<double>(number -
                                       1);  // если убрать "- 1", то "не меньше" в описании нужно заменить на "больше"
        auto answer = reinterpret_cast<unsigned int *>(&x);
        ++answer;
        return static_cast<size_t>(1) << (((*answer & 0x7FF00000) >> 20) - 1022);
    }


};

/**
 * @brief Реализация матриц целых чисел 2x2. Реализованы только операции, необходимые для решения задачи.
 */
class Matrix2x2 {
public:
    /**
     * @brief Конструирует матрицу.
     * @param a11 Элемент матрицы. 1-я строка, 1-й столбец.
     * @param a12 Элемент матрицы. 1-я строка, 2-й столбец.
     * @param a21 Элемент матрицы. 2-я строка, 1-й столбец.
     * @param a22 Элемент матрицы. 2-я строка, 2-й столбец.
     */
    Matrix2x2(uint64_t a11, uint64_t a12, uint64_t a21, uint64_t a22) : elements_{a11, a12, a21, a22} {}

    /**
     * @brief Сравнение матриц на равенство. Нужно для тестов.
     * @param rhs Матрица, с которой сравниваем.
     * @return True, если матрицы равны, иначе - false.
     */
    bool operator==(const Matrix2x2 &rhs) const {
        return elements_ == rhs.elements_;
    }

    /**
     * @brief Проверка матриц на неравенство. Нужно для тестов.
     * @param rhs Матрица, с которой сравниваем.
     * @return True, если матрицы не равны, иначе - false.
     */
    bool operator!=(const Matrix2x2 &rhs) const {
        return !(rhs == *this);
    }

    /**
     * @brief Печатает матрицу в поток вывода.
     * @tparam StreamType Тип потока вывода.
     * @param stream Поток вывода.
     */
    template<typename StreamType>
    void print(StreamType &stream) {
        stream << elements_[0] << " " << elements_[1];
        stream << "\n";
        stream << elements_[2] << " " << elements_[3];
    }

    /**
     * @brief Умножение матриц.
     * @param b Матрица, на которую умножаем.
     * @return Произведение матриц.
     */
    Matrix2x2 operator*(const Matrix2x2 &b) const {
        auto c11 = this->elements_[0] * b.elements_[0] + this->elements_[1] * b.elements_[2];
        auto c12 = this->elements_[0] * b.elements_[1] + this->elements_[1] * b.elements_[3];
        auto c21 = this->elements_[2] * b.elements_[0] + this->elements_[3] * b.elements_[2];
        auto c22 = this->elements_[2] * b.elements_[1] + this->elements_[3] * b.elements_[3];
        return {c11, c12, c21, c22};
    }

    /**
     * @brief Умножение матриц inplace.
     * @param b Матрица, на которую умножаем.
     * @return Произведение матриц.
     */
    Matrix2x2 &operator*=(const Matrix2x2 &b) {
        auto c11 = this->elements_[0] * b.elements_[0] + this->elements_[1] * b.elements_[2];
        auto c12 = this->elements_[0] * b.elements_[1] + this->elements_[1] * b.elements_[3];
        auto c21 = this->elements_[2] * b.elements_[0] + this->elements_[3] * b.elements_[2];
        auto c22 = this->elements_[2] * b.elements_[1] + this->elements_[3] * b.elements_[3];
        elements_[0] = c11;
        elements_[1] = c12;
        elements_[2] = c21;
        elements_[3] = c22;
        return *this;
    }

    /**
     * @brief Взятие элементов матрицы по модулю inplace.
     * @param r Модуль, по которому берем элементы.
     * @return Матрица, в которой каждый элемент равен остатку от деления исходного значения на r.
     */
    Matrix2x2 &operator%=(const uint64_t &r) {
        for (auto &element: elements_) {
            element %= r;
        }
        return *this;
    }

private:
    std::array<uint64_t, 4> elements_;  // a11, a12, a21, a22
};


// Начало тестов

void test_from_task() {
    Matrix2x2 m1{0, 1, 0, 0};
    Matrix2x2 m2{2, 1, 1, 2};
    Matrix2x2 m3{0, 0, 0, 2};
    Matrix2x2 m4{1, 0, 0, 2};
    uint64_t r = 3;

    SegmentTree<Matrix2x2> tree(
            {m1, m2, m3, m4},
            [r](const auto &lhs, const auto &rhs) {
                auto result = lhs * rhs;
                result %= r;
                return result;
            },
            Matrix2x2{1, 0, 0, 1});

    Matrix2x2 ans1{0, 2, 0, 0};
    Matrix2x2 ans2{0, 2, 0, 1};
    Matrix2x2 ans3{0, 1, 0, 0};
    Matrix2x2 ans4{2, 1, 1, 2};

    auto result1 = tree.query(0, 3);
    auto result2 = tree.query(1, 2);
    auto result3 = tree.query(0, 2);
    auto result4 = tree.query(1, 1);

    assert(ans1 == result1);
    assert(ans2 == result2);
    assert(ans3 == result3);
    assert(ans4 == result4);
}

void run_all_tests() {
    test_from_task();
}

// Конец тестов

int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);

    if (argc > 1) {
        if (std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        }
    }

    // Чтение матриц
    std::ifstream fin("crypto.in");
    uint64_t r{1};
    size_t n{0}, m{0};
    fin >> r >> n >> m;
    std::vector<Matrix2x2> matrixes;
    for (size_t i = 0; i < n; ++i) {
        int64_t a11{0}, a12{0}, a21{0}, a22{0};
        fin >> a11 >> a12 >> a21 >> a22;
        matrixes.emplace_back(a11, a12, a21, a22);
    }

    // Выполнение запросов
    SegmentTree<Matrix2x2> tree(
            matrixes,
            [r](const auto &lhs, const auto &rhs) {
                auto result = lhs * rhs;
                result %= r;
                return result;
            },
            Matrix2x2{1, 0, 0, 1});
    std::ofstream fout("crypto.out");
    for (size_t i = 0; i < m; ++i) {
        size_t left{1}, right{1};
        fin >> left >> right;
        auto result = tree.query(left - 1, right - 1);
        result.print(fout);
        fout << "\n" << "\n";
    }
    fin.close();
    fout.close();

    return 0;
}