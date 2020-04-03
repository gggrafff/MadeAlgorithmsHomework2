/*
 * B. RSQ
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Входные данные
 * В первой строке находится число n — размер массива. (1 ≤ n ≤ 500 000)
 * Во второй строке находится n чисел ai — элементы массива.
 * Далее содержится описание операций, их количество не превышает 1 000 000.
 * В каждой строке находится одна из следующих операций:
 *     set i x — установить a[i] в x.
 *     sum i j — вывести значение суммы элементов в массиве на отрезке с i по j, гарантируется, что (1 ≤ i ≤ j ≤ n).
 *
 * Все числа во входном файле и результаты выполнения всех операций не превышают по модулю 10^18.
 * Выходные данные
 * Выведите последовательно результат выполнения всех операций sum. Следуйте формату выходного файла из примера.
 * Пример
 * Входные данные
 * 5
 * 1 2 3 4 5
 * sum 2 5
 * sum 1 5
 * sum 1 4
 * sum 2 4
 * set 1 10
 * set 2 3
 * set 5 2
 * sum 2 5
 * sum 1 5
 * sum 1 4
 * sum 2 4
 *
 * Выходные данные
 * 14
 * 15
 * 10
 * 9
 * 12
 * 22
 * 20
 * 10
 */

/**
 * Задачу можно решить как деревом отрезков, так и деревом Фенвика.
 * Реализовал оба алгоритма. Контест проходил деревом Фенвика.
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>
#include <numeric>
#include <functional>

/**
 * @brief Реализация дерева Фенвика для целых чисел.
 * @details Подробнее: https://neerc.ifmo.ru/wiki/index.php?title=Дерево_Фенвика
 */
class FenwickTree {
public:
    /**
     * @brief Конструирует дерево Фенвика из массива чисел.
     * @param numbers Массив чисел, по которым хотим делать запросы.
     */
    explicit FenwickTree(const std::vector<int64_t> &numbers) {
        numbers_ = numbers;
        fenwick_sums_.resize(numbers.size(), 0);
        for (size_t i = 0; i < numbers.size(); ++i) {
            add_to_element(i, numbers[i]);
        }
    }

    /**
     * @brief Запрос префиксной суммы.
     * @param r Индекс, по который включительно нужна префиксная сумма.
     * @return Результат запроса.
     */
    int64_t get_prefix_sum(int64_t r) {
        assert(r >= 0 && r < fenwick_sums_.size());
        int64_t result = 0;
        while (r >= 0) {
            assert(r >= 0 && r < fenwick_sums_.size());
            result += fenwick_sums_[r];
            r = fenwick(r) - 1;
        }
        return result;
    }

    /**
     * @brief Запрос суммы на отрезке.
     * @param l Левая граница отрезка. Индекс.
     * @param r Правая граница отрезка. Индекс.
     * @return Результат запроса.
     */
    int64_t get_sum(size_t l, size_t r) {
        if (l > r) {
            std::swap(l, r);
        }
        assert(r >= 0 && r < fenwick_sums_.size());
        assert(l >= 0 && l < fenwick_sums_.size());
        if (l == 0) {
            return get_prefix_sum(r);
        }
        return get_prefix_sum(r) - get_prefix_sum(l - 1);
    }

    /**
     * @brief Увеличить элемент.
     * @param index Индекс элемента.
     * @param value Значение, на которое увеличиваем.
     */
    void add_to_element(size_t index, const int64_t value) {
        while (index < fenwick_sums_.size()) {
            assert(index >= 0 && index < fenwick_sums_.size());
            fenwick_sums_[index] += value;
            index = index | (index + 1);
        }
    }

    /**
     * @brief Установить новое значение элемента.
     * @param index Индекс элемента.
     * @param value Значение, которое устанавливаем.
     */
    void set_element(const size_t index, const int64_t value) {
        assert(index >= 0 && index < numbers_.size());
        const auto diff = value - numbers_[index];
        numbers_[index] = value;
        add_to_element(index, diff);
    }

private:
    /**
     * @brief Функция Фенвика.
     * @tparam NumberType Числовой тип данных.
     * @param i Аргумент функции Фенвика.
     * @return Результат функции Фенвика.
     */
    template<typename NumberType>
    static NumberType fenwick(const NumberType i) {
        return (i & (i + 1));
    }

    std::vector<int64_t> fenwick_sums_;
    std::vector<int64_t> numbers_;
};

/**
 * @brief Реализация дерева отрезков для запросов на отрезке и изменения отдельных элементов.
 * @tparam ElementType Тип элементов, хранящихся в дереве. Должен быть определён operator+ для обновления элементов.
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
    ElementType query(size_t l, size_t r) {
        if (l > r) {
            std::swap(l, r);
        }
        auto n = tree_.size() / 2;
        l += n;
        r += n;
        ElementType result = query_operation_neutral_element_;
        while (l <= r) {
            if (l % 2 == 1) {
                result = query_operation_(result, tree_[l++]);
            }
            if (r % 2 == 0) {
                result = query_operation_(result, tree_[r--]);
            }
            l /= 2;
            r /= 2;
        }
        return result;
    }

    /**
     * @brief Изменить элемент. Для изменения используется operator+.
     * @param index Индекс элемента.
     * @param value Значение, на которое нужно изменить.
     */
    void add(size_t index, const ElementType value) {
        auto n = tree_.size() / 2;
        index += n;
        tree_[index] += value;
        for (index /= 2; index >= 1; index /= 2) {
            tree_[index] = query_operation_(tree_[2 * index], tree_[2 * index + 1]);
        }
    }

    /**
     * @brief Установить новое значение элемента.
     * @param index Индекс элемента.
     * @param value Новое значение.
     */
    void set(const size_t index, const ElementType value) {
        auto n = tree_.size() / 2;
        const auto diff = value - tree_[index + n];
        add(index, diff);
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
        double x = static_cast<double>(number - 1);  // если убрать "- 1", то "не меньше" в описании нужно заменить на "больше"
        auto answer = reinterpret_cast<unsigned int *>(&x);
        ++answer;
        return static_cast<size_t>(1) << (((*answer & 0x7FF00000) >> 20) - 1022);
    }

};

// Начало тестов

void test_from_task_fenwick() {
    FenwickTree tree({1, 2, 3, 4, 5});
    assert(tree.get_sum(2 - 1, 5 - 1) == 14);
    assert(tree.get_sum(1 - 1, 5 - 1) == 15);
    assert(tree.get_sum(1 - 1, 4 - 1) == 10);
    assert(tree.get_sum(2 - 1, 4 - 1) == 9);
    tree.set_element(1 - 1, 10);
    tree.set_element(2 - 1, 3);
    tree.set_element(5 - 1, 2);
    assert(tree.get_sum(2 - 1, 5 - 1) == 12);
    assert(tree.get_sum(1 - 1, 5 - 1) == 22);
    assert(tree.get_sum(1 - 1, 4 - 1) == 20);
    assert(tree.get_sum(2 - 1, 4 - 1) == 10);
}

void test_from_task_segment() {
    SegmentTree<int64_t> tree({1LL, 2LL, 3LL, 4LL, 5LL}, [](const auto &lhs, const auto &rhs) { return lhs + rhs; }, 0);
    assert(tree.query(2 - 1, 5 - 1) == 14);
    assert(tree.query(1 - 1, 5 - 1) == 15);
    assert(tree.query(1 - 1, 4 - 1) == 10);
    assert(tree.query(2 - 1, 4 - 1) == 9);
    tree.set(1 - 1, 10);
    tree.set(2 - 1, 3);
    tree.set(5 - 1, 2);
    assert(tree.query(2 - 1, 5 - 1) == 12);
    assert(tree.query(1 - 1, 5 - 1) == 22);
    assert(tree.query(1 - 1, 4 - 1) == 20);
    assert(tree.query(2 - 1, 4 - 1) == 10);
}

void test_random_fenwick() {
    std::vector<int64_t> numbers;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int64_t> numbers_generator(-1000 * 1000 * 1000, 1000 * 1000 * 1000);
    const size_t n = 1000;
    numbers.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        numbers.push_back(numbers_generator(rd));
    }
    FenwickTree tree(numbers);
    std::uniform_int_distribution<size_t> requests_generator(0, n - 1);
    const size_t requests_number = 1000;
    for (size_t i = 0; i < requests_number; ++i) {
        auto l = requests_generator(rd);
        auto r = requests_generator(rd);
        if (l > r) {
            std::swap(l, r);
        }
        assert(l >= 0 && l < numbers.size());
        assert(r >= 0 && r < numbers.size());
        const auto result = tree.get_sum(l, r);
        const auto true_result = std::accumulate(std::next(numbers.begin(), l), std::next(numbers.begin(), r + 1), 0LL);
        assert(result == true_result);
    }
    for (size_t i = 0; i < requests_number / 2; ++i) {
        const auto index = requests_generator(rd);
        const auto value = numbers_generator(rd);
        assert(index >= 0 && index < numbers.size());
        tree.set_element(index, value);
        numbers[index] = value;
    }
    for (size_t i = 0; i < requests_number; ++i) {
        auto l = requests_generator(rd);
        auto r = requests_generator(rd);
        if (l > r) {
            std::swap(l, r);
        }
        assert(l >= 0 && l < numbers.size());
        assert(r >= 0 && r < numbers.size());
        const auto result = tree.get_sum(l, r);
        const auto true_result = std::accumulate(std::next(numbers.begin(), l), std::next(numbers.begin(), r + 1), 0LL);
        assert(result == true_result);
    }
}

void test_random_segment() {
    std::vector<int64_t> numbers;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int64_t> numbers_generator(-1000 * 1000 * 1000, 1000 * 1000 * 1000);
    const size_t n = 1000;
    numbers.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        numbers.push_back(numbers_generator(rd));
    }
    SegmentTree<int64_t> tree(numbers, [](const auto &lhs, const auto &rhs) { return lhs + rhs; }, 0);
    std::uniform_int_distribution<size_t> requests_generator(0, n - 1);
    const size_t requests_number = 1000;
    for (size_t i = 0; i < requests_number; ++i) {
        auto l = requests_generator(rd);
        auto r = requests_generator(rd);
        if (l > r) {
            std::swap(l, r);
        }
        assert(l >= 0 && l < numbers.size());
        assert(r >= 0 && r < numbers.size());
        const auto result = tree.query(l, r);
        const auto true_result = std::accumulate(std::next(numbers.begin(), l), std::next(numbers.begin(), r + 1), 0LL);
        assert(result == true_result);
    }
    for (size_t i = 0; i < requests_number / 2; ++i) {
        const auto index = requests_generator(rd);
        const auto value = numbers_generator(rd);
        assert(index >= 0 && index < numbers.size());
        tree.set(index, value);
        numbers[index] = value;
    }
    for (size_t i = 0; i < requests_number; ++i) {
        auto l = requests_generator(rd);
        auto r = requests_generator(rd);
        if (l > r) {
            std::swap(l, r);
        }
        assert(l >= 0 && l < numbers.size());
        assert(r >= 0 && r < numbers.size());
        const auto result = tree.query(l, r);
        const auto true_result = std::accumulate(std::next(numbers.begin(), l), std::next(numbers.begin(), r + 1), 0LL);
        assert(result == true_result);
    }
}

void run_all_tests() {
    test_from_task_fenwick();
    test_from_task_segment();
    test_random_fenwick();
    test_random_segment();
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
    std::vector<int64_t> numbers(n, 0);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> numbers[i];
    }
    FenwickTree tree(numbers);
    std::string operation;
    while (std::cin >> operation) {
        if (operation == "sum") {
            size_t l{0}, r{0};
            std::cin >> l >> r;
            assert(l != 0 && l <= numbers.size());
            assert(r != 0 && r <= numbers.size());
            std::cout << tree.get_sum(l - 1, r - 1) << std::endl;
        } else if (operation == "set") {
            size_t index{0};
            uint64_t value{0};
            std::cin >> index >> value;
            assert(index != 0 && index <= numbers.size());
            tree.set_element(index - 1, value);
        } else {
            break;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return 0;
}