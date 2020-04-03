/*
 * F. RMQ наоборот
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод rmq.in
 * вывод rmq.out
 *
 * Рассмотрим массив a[1..n]. Пусть Q(i, j) — ответ на запрос о нахождении минимума среди чисел a[i], ..., a[j].
 * Вам даны несколько запросов и ответы на них. Восстановите исходный массив.
 *
 * Входные данные
 * Первая строка входного файла содержит число n — размер массива, и m — число запросов (1 ≤ n, m ≤ 100 000). Следующие m строк содержат по три целых числа i, j и q, означающих, что Q(i, j) = q (1 ≤ i ≤ j ≤ n,  - 231 ≤ q ≤ 231 - 1).
 * Выходные данные
 * Если искомого массива не существует, выведите строку «inconsistent».
 * В противном случае в первую строку выходного файла выведите «consistent».
 * Во вторую строку выходного файла выведите элементы массива.
 * Элементами массива должны быть целые числа в интервале от  - 2^31 до 2^31 - 1 включительно.
 * Если решений несколько, выведите любое.
 *
 * Примеры
 * Входные данные
 * 3 2
 * 1 2 1
 * 2 3 2
 * Выходные данные
 * consistent
 * 1 2 2
 *
 * Входные данные
 * 3 3
 * 1 2 1
 * 1 1 2
 * 2 3 2
 * Выходные данные
 * inconsistent
 */

#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <random>

/**
 * @brief Реализация дерева отрезков с возможностью задать функцию для запроса и функции обновления узлов.
 * @details Функция запроса должна быть идемпотентной. Обе функции должны быть ассоциативны и коммутативны.
 * @details Подробнее:
 * @details https://neerc.ifmo.ru/wiki/index.php?title=Несогласованные_поддеревья._Реализация_массового_обновления
 * @details https://e-maxx.ru/algo/segment_tree
 * @tparam ElementType Тип элементов, хранящихся в дереве.
 */
template<typename ElementType>
class SegmentTree {
public:
    /**
     * @brief Конструирует дерево отрезков.
     * @param elements Элементы, изначально хранящиеся в интересуемом массиве.
     * @param query_operation Идемпотентная операция, по которой производится запрос. Например, min(lhs, rhs).
     * @param query_operation_neutral_element Нейтральный элемент относительно операции запроса. Например, inf.
     * @param update_operation Операция обновления элементов. Например, сложение.
     * @param update_operation_neutral_element Нейтральный элемент относительно операции обновления. Например, 0.
     * @param set_operation Операция установки нового значения.
     */
    explicit SegmentTree(
            const std::vector<ElementType> &elements,
            const std::function<ElementType(const ElementType &lhs, const ElementType &rhs)> query_operation,
            const ElementType &query_operation_neutral_element,
            const std::function<ElementType(const ElementType &lhs, const ElementType &rhs)> update_operation,
            const ElementType &update_operation_neutral_element,
            const std::function<ElementType(const ElementType &old_value, const ElementType &new_value)> set_operation)
            :
            query_operation_{query_operation},
            query_operation_neutral_element_{query_operation_neutral_element},
            update_operation_{update_operation},
            update_operation_neutral_element_{update_operation_neutral_element},
            set_operation_{set_operation} {
        size_t n = round_pow2(
                elements.size());  // удобно, когда размер дерева - степень двойки, дополним массив нейтральными элементами
        tree_.resize(n * 2, Node{query_operation_neutral_element_, update_operation_neutral_element_, false, 0, 0});

        std::transform(elements.rbegin(), elements.rend(), std::next(tree_.rbegin(), n - elements.size()),
                       [&update_operation_neutral_element](auto &element) {
                           Node node{element, update_operation_neutral_element, false, 0, 0};
                           return node;
                       });  // копируем элементы в дерево
        for (auto index = tree_.size() - 1; index > n; --index) {  // помечаем, что листы отвечают только за самих себя
            tree_[index].left = n - (tree_.size() - index);
            tree_[index].right = n - (tree_.size() - index);
        }

        for (auto i = n - 1; i > 0; --i) {
            // считаем значение в промежуточных узлах
            tree_[i].value = query_operation_(tree_[2 * i].value, tree_[2 * i + 1].value);
            // помечаем, за какие листы отвечают промежуточные узлы
            tree_[i].left = tree_[2 * i].left;
            tree_[i].right = tree_[2 * i + 1].right;
        }
    }

    /**
     * @brief Выполняет запрос на отрезке.
     * @param l Левый конец отрезка. Индекс элемента исходного массива.
     * @param r Правый конец отрезка. Индекс элемента исходного массива.
     * @return Результат запроса.
     */
    ElementType query(size_t l, size_t r) {
        if (l > r) {
            std::swap(l, r);
        }
        return query(1, l, r);
    }

    /**
     * @brief Выполняет обновление на отрезке с помощью операции обновления.
     * @param l Левый конец отрезка. Индекс элемента исходного массива.
     * @param r Правый конец отрезка. Индекс элемента исходного массива.
     * @param value Значение, на которое нужно обновить.
     */
    void update(size_t l, size_t r, const ElementType &value) {
        if (l > r) {
            std::swap(l, r);
        }
        update_node(1, l, r, value);
    }

    /**
     * @brief Выполняет установку значения элементов на отрезке.
     * @param l Левый конец отрезка. Индекс элемента исходного массива.
     * @param r Правый конец отрезка. Индекс элемента исходного массива.
     * @param value Значение, которое нужно установить.
     */
    void set(size_t l, size_t r, const ElementType &value) {
        if (l > r) {
            std::swap(l, r);
        }
        set_node(1, l, r, value);
    }

    /**
     * @brief Возвращает массив элементов из дерева.
     * @param l Левый конец возвращаемого отрезка. Индекс элемента исходного массива.
     * @param r Правый конец возвращаемого отрезка. Индекс элемента исходного массива.
     * @return Массив элементов.
     */
    std::vector<ElementType> get_segment(size_t l, size_t r) {
        if (l > r) {
            std::swap(l, r);
        }
        push_imbalance_to_lists(1);
        std::vector<ElementType> result;
        result.reserve(tree_.size() / 2);
        for (size_t i = tree_.size() / 2 + l; i <= tree_.size() / 2 + r; ++i) {
            result.push_back(update_operation_(tree_[i].value, tree_[i].imbalance));
        }
        return result;
    }

private:
    /**
     * @brief Структура, представляющая узел дерева.
     */
    struct Node {
        ElementType value{ElementType()};  // значение, хранящееся в узле
        ElementType imbalance{ElementType()};  // величина несоответствия значения актуальному после обновления
        bool refreshed{false};  // пометка, было ли значение полностью обновлено, но дети об этом не знают
        size_t left{0};  // индекс левого конца отрезка, за который отвечает узел
        size_t right{0};  // индекс правого конца отрезка, за который отвечает узел
    };

    std::vector<Node> tree_;
    std::function<ElementType(const ElementType &lhs, const ElementType &rhs)> query_operation_{std::min<ElementType>};
    ElementType query_operation_neutral_element_{std::numeric_limits<ElementType>::max()};
    std::function<ElementType(const ElementType &lhs, const ElementType &rhs)> update_operation_{
            [](const ElementType &lhs, const ElementType &rhs) { return lhs + rhs; }};
    ElementType update_operation_neutral_element_{ElementType()};
    std::function<ElementType(const ElementType &old_value, const ElementType &new_value)> set_operation_{
            [](const ElementType &old_value, const ElementType &new_value) { return new_value; }};

    /**
     * @brief Определяет наименьшую степень двойки, не меньшую заданного числа.
     * @details Используется знание о представлении чисел с плавающей точкой в памяти.
     * @details Подробнее: https://ru.wikipedia.org/wiki/Число_двойной_точности
     * @param number Число, которое нужно округлить вверх до стпени двойки.
     * @return Число, равное степени двойки. Минимальное, но не меньше, чем number.
     */
    static size_t round_pow2(const size_t number) {
        double x = static_cast<double>(number -
                                       1);  // если убрать "- 1", то "не меньше" в описании нужно заменить на "больше"
        auto answer = reinterpret_cast<unsigned int *>(&x);
        ++answer;
        return static_cast<size_t>(1) << (((*answer & 0x7FF00000) >> 20) - 1022);
    }

    /**
     * @brief Протолкнуть несоотвествие значений узлов вниз(потомкам).
     * @param node Индекс узла, из которого проталкиваем.
     */
    void push_imbalance(const size_t node) {
        if (tree_[node].refreshed) {
            tree_[2 * node].value = set_operation_(tree_[2 * node].value, tree_[node].value);
            tree_[2 * node].imbalance = tree_[node].imbalance;
            tree_[2 * node].refreshed = true;
            tree_[2 * node + 1].value = set_operation_(tree_[2 * node + 1].value, tree_[node].value);
            tree_[2 * node + 1].imbalance = tree_[node].imbalance;
            tree_[2 * node + 1].refreshed = true;
            tree_[node].imbalance = update_operation_neutral_element_;
            tree_[node].refreshed = false;
        } else {
            tree_[2 * node].imbalance = update_operation_(tree_[2 * node].imbalance, tree_[node].imbalance);
            tree_[2 * node + 1].imbalance = update_operation_(tree_[2 * node + 1].imbalance, tree_[node].imbalance);
            tree_[node].imbalance = update_operation_neutral_element_;
        }
    }

    /**
     * @brief Протолкнуть несоотвествие значений узлов вниз(потомкам) до самых листьев рекурсивно.
     * @param node Индекс узла, из которого проталкиваем в данный момент.
     */
    void push_imbalance_to_lists(const size_t node) {
        if (tree_[node].refreshed) {
            tree_[2 * node].value = set_operation_(tree_[2 * node].value, tree_[node].value);
            tree_[2 * node].imbalance = tree_[node].imbalance;
            tree_[2 * node].refreshed = true;
            tree_[2 * node + 1].value = set_operation_(tree_[2 * node + 1].value, tree_[node].value);
            tree_[2 * node + 1].imbalance = tree_[node].imbalance;
            tree_[2 * node + 1].refreshed = true;
            tree_[node].imbalance = update_operation_neutral_element_;
            tree_[node].refreshed = false;
        } else {
            tree_[2 * node].imbalance = update_operation_(tree_[2 * node].imbalance, tree_[node].imbalance);
            tree_[2 * node + 1].imbalance = update_operation_(tree_[2 * node + 1].imbalance, tree_[node].imbalance);
            tree_[node].imbalance = update_operation_neutral_element_;
        }
        if (2 * node < tree_.size() / 2) {
            push_imbalance_to_lists(2 * node);
            push_imbalance_to_lists(2 * node + 1);
        }
        tree_[node].value = query_operation_(
                update_operation_(tree_[2 * node].value, tree_[2 * node].imbalance),
                update_operation_(tree_[2 * node + 1].value, tree_[2 * node + 1].imbalance)
        );
    }

    /**
     * @brief Обновить значения листов, за которые отвечает узел. Используется функция обновления.
     * @param node Индекс обновляемого узла.
     * @param from Левая граница всего отрезка, который нужно обновить.
     * @param to Правая граница всего отрезка, который нужно обновить.
     * @param value Значение, на которое обновляем.
     */
    void update_node(const size_t node, const size_t from, const size_t to, const ElementType &value) {
        auto l = tree_[node].left;
        auto r = tree_[node].right;
        if (!segments_intersection({l, r}, {from, to})) {  // данный узел не нужен для обновления отрезка
            return;
        }
        if (segments_belong({l, r}, {from, to})) {  // данный узел полностью нужен для обновления отрезка
            tree_[node].imbalance = update_operation_(tree_[node].imbalance, value);
            return;
        }
        push_imbalance(node);
        // обновление детей
        update_node(2 * node, from, to, value);
        update_node(2 * node + 1, from, to, value);
        // пересчёт значения в текущем узле
        tree_[node].value = query_operation_(
                update_operation_(tree_[2 * node].value, tree_[2 * node].imbalance),
                update_operation_(tree_[2 * node + 1].value, tree_[2 * node + 1].imbalance)
        );
    }

    /**
     * @brief Установить значения листов, за которые отвечает узел.
     * @param node Индекс рассматриваемого узла.
     * @param from Левая граница всего отрезка, который нужно установить в новое значение.
     * @param to Правая граница всего отрезка, который нужно установить в новое значение.
     * @param value Новое значение.
     */
    void set_node(const size_t node, const size_t from, const size_t to, const ElementType &value) {
        auto l = tree_[node].left;
        auto r = tree_[node].right;
        if (!segments_intersection({l, r}, {from, to})) {  // данный узел не нужен для установки значений отрезка
            return;
        }
        if (segments_belong({l, r}, {from, to})) {  // данный узел полностью нужен для установки значений отрезка
            tree_[node].imbalance = update_operation_neutral_element_;
            tree_[node].value = set_operation_(tree_[node].value, value);
            tree_[node].refreshed = true;
            return;
        }
        push_imbalance(node);
        // обновление детей
        set_node(2 * node, from, to, value);
        set_node(2 * node + 1, from, to, value);
        // пересчёт значения в текущем узле
        tree_[node].value = query_operation_(
                update_operation_(tree_[2 * node].value, tree_[2 * node].imbalance),
                update_operation_(tree_[2 * node + 1].value, tree_[2 * node + 1].imbalance)
        );
    }

    /**
     * @brief Выполнить запрос на листьях, за которые отвечает узел.
     * @param node Индекс рассматриваемого узла.
     * @param from Левая граница всего отрезка, на котором выполняем запрос.
     * @param to Правая граница всего отрезка, на котором выполняем запрос.
     * @return Результат запроса в рамках текущего поддерева.
     */
    ElementType query(size_t node, size_t from, size_t to) {
        auto l = tree_[node].left;
        auto r = tree_[node].right;
        if (!segments_intersection({l, r}, {from, to})) {
            return query_operation_neutral_element_;
        }
        if (segments_belong({l, r}, {from, to})) {
            return update_operation_(tree_[node].value, tree_[node].imbalance);
        }
        push_imbalance(node);
        ElementType result = query_operation_(query(node * 2, from, to), query(node * 2 + 1, from, to));
        tree_[node].value = query_operation_(
                update_operation_(tree_[2 * node].value, tree_[2 * node].imbalance),
                update_operation_(tree_[2 * node + 1].value, tree_[2 * node + 1].imbalance)
        );
        return result;
    }

    /**
     * @brief Пересекаются ли отрезки?
     * @param lhs Первый отрезок.
     * @param rhs Второй отрезок.
     * @return true, если пересекаются, иначе - false.
     */
    static bool segments_intersection(const std::pair<size_t, size_t> lhs, const std::pair<size_t, size_t> rhs) {
        if ((lhs.first >= rhs.first) &&
            (lhs.first <= rhs.second)) {  // левый конец первого отрезка лежит внутри второго
            return true;
        }
        if ((lhs.second >= rhs.first) &&
            (lhs.second <= rhs.second)) {  // правый конец первого отрезка лежит внутри второго
            return true;
        }
        // остаётся две ситуации: либо не пересекаются, либо второй отрезок внутри первого
        if ((rhs.first >= lhs.first) && (rhs.first <= lhs.second)) {  // второй отрезок внутри первого
            return true;
        }
        return false;
    }

    /**
     * @brief Принадлежит ли первый отрезок второму?
     * @param inside Внутренний отрезок.
     * @param outside Внешний отрезок.
     * @return true, если первый отрезок внутри второго, иначе - false.
     */
    static bool segments_belong(const std::pair<size_t, size_t> inside, const std::pair<size_t, size_t> outside) {
        if ((inside.first < outside.first) ||
            (inside.first > outside.second)) {  // левый конец первого отрезка лежит вне второго отрезка
            return false;
        }
        if ((inside.second < outside.first) ||
            (inside.second > outside.second)) {  // правый конец первого отрезка лежит вне второго отрезка
            return false;
        }
        return true;
    }
};


// Начало тестов

void test_from_task_1() {
    size_t n = 3;
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree(std::vector<int64_t>(n, 0),
                              operation,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0,
                              [](const int64_t &old_value, const int64_t &new_value) { return new_value; });
    tree.set(0, 1, 1);
    tree.set(1, 2, 2);
    assert(tree.query(0, 1) == 1);
    assert(tree.query(1, 2) == 2);
    auto result = tree.get_segment(0, n - 1);
    std::vector<int64_t> true_result{1, 2, 2};
    assert(result == true_result);
}

void test_from_task_2() {
    size_t n = 3;
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree(std::vector<int64_t>(n, 0),
                              operation,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0,
                              [](const int64_t &old_value, const int64_t &new_value) { return new_value; });
    tree.set(0, 1, 1);
    tree.set(0, 0, 2);
    tree.set(1, 2, 2);
    assert(tree.query(0, 1) != 1);
    assert(tree.query(0, 0) == 2);
    assert(tree.query(1, 2) == 2);
}

void test_one_element_consistent() {
    size_t n = 1;
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree(std::vector<int64_t>(n, 0),
                              operation,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0,
                              [](const int64_t &old_value, const int64_t &new_value) { return new_value; });
    tree.set(0, 0, 1);
    assert(tree.query(0, 0) == 1);
    auto result = tree.get_segment(0, n - 1);
    std::vector<int64_t> true_result{1};
    assert(result == true_result);
}

void test_one_element_inconsistent() {
    size_t n = 1;
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree(std::vector<int64_t>(n, 0),
                              operation,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0,
                              [](const int64_t &old_value, const int64_t &new_value) { return new_value; });
    tree.set(0, 0, 1);
    tree.set(0, 0, 2);
    assert(tree.query(0, 0) != 1);
    assert(tree.query(0, 0) == 2);
    auto result = tree.get_segment(0, n - 1);
    std::vector<int64_t> true_result{2};
    assert(result == true_result);
}

void test_partial_set_32bit() {
    size_t n = 3;
    const int32_t &(*operation)(const int32_t &, const int32_t &) = std::min<int32_t>;
    SegmentTree<int32_t> tree(std::vector<int32_t>(n, 0),
                              operation,
                              std::numeric_limits<int32_t>::max(),
                              [](const int32_t lhs, const int32_t rhs) { return lhs + rhs; },
                              0,
                              [](const int64_t &old_value, const int64_t &new_value) { return new_value; });
    tree.set(0, 0, 1);
    assert(tree.query(0, 0) == 1);
    auto result = tree.get_segment(0, n - 1);
    std::vector<int32_t> true_result{1, 0, 0};
    assert(result == true_result);
}

void test_intersection() {
    size_t n = 8;
    const std::pair<int64_t, bool> &(*operation)(const std::pair<int64_t, bool> &,
                                                 const std::pair<int64_t, bool> &) = std::min<std::pair<int64_t, bool>>;
    SegmentTree<std::pair<int64_t, bool>> tree(
            std::vector<std::pair<int64_t, bool>>(n, {0, false}),
            operation,
            {std::numeric_limits<int64_t>::max(), false},
            [](const std::pair<int64_t, bool> lhs, const std::pair<int64_t, bool> rhs) {
                return std::pair<int64_t, bool>{lhs.first + rhs.first, lhs.second || rhs.second};
            },
            {0, false},
            [](const std::pair<int64_t, bool> &old_value, const std::pair<int64_t, bool> &new_value) {
                if (old_value.second) {
                    return old_value;
                }
                return std::pair<int64_t, bool>{new_value.first, true};
            });
    tree.set(2, 6, {3, false});
    tree.set(0, 4, {1, false});
    tree.set(4, 7, {2, false});
    auto result = tree.get_segment(0, n - 1);
    assert(tree.query(2, 6).first == 3);
    assert(tree.query(0, 4).first == 1);
    assert(tree.query(4, 7).first == 2);
}

void test_big_numbers() {
    size_t n = 3;
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree(std::vector<int64_t>(n, 0),
                              operation,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0,
                              [](const int64_t &old_value, const int64_t &new_value) { return new_value; });
    tree.set(0, 1, std::numeric_limits<int64_t>::min());
    tree.set(1, 2, std::numeric_limits<int64_t>::max());
    assert(tree.query(0, 1) == std::numeric_limits<int64_t>::min());
    assert(tree.query(1, 2) == std::numeric_limits<int64_t>::max());
    auto result = tree.get_segment(0, n - 1);
    std::vector<int64_t> true_result{std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max(),
                                     std::numeric_limits<int64_t>::max()};
    assert(result == true_result);
}

void test_random() {
    std::vector<int64_t> numbers;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int64_t> numbers_generator(-1000 * 1000, 1000 * 1000);
    const size_t n = 1000;
    numbers.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        numbers.push_back(numbers_generator(rd));
    }
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree(
            numbers,
            operation,
            std::numeric_limits<int64_t>::max(),
            [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
            0,
            [](const int64_t &old_value, const int64_t &new_value) { return new_value; }
    );
    std::uniform_int_distribution<size_t> requests_generator(0, n - 1);
    const size_t requests_number = n;
    for (size_t i = 0; i < requests_number; ++i) {
        auto l = requests_generator(rd);
        auto r = requests_generator(rd);
        if (l > r) {
            std::swap(l, r);
        }
        assert(l >= 0 && l < numbers.size());
        assert(r >= 0 && r < numbers.size());
        const auto result = tree.query(l, r);
        const auto true_result = *std::min_element(std::next(numbers.begin(), l), std::next(numbers.begin(), r + 1));
        assert(result == true_result);
    }
    assert(numbers == tree.get_segment(0, n - 1));
    for (size_t i = 0; i < requests_number / 2; ++i) {
        auto l = requests_generator(rd);
        auto r = requests_generator(rd);
        if (l > r) {
            std::swap(l, r);
        }
        assert(l >= 0 && l < numbers.size());
        assert(r >= 0 && r < numbers.size());
        const auto value = numbers_generator(rd);
        tree.set(l, r, value);
        for (auto iter = std::next(numbers.begin(), l); iter != std::next(numbers.begin(), r + 1); ++iter) {
            *iter = value;
        }
    }
    assert(numbers == tree.get_segment(0, n - 1));
    for (size_t i = 0; i < requests_number; ++i) {
        auto l = requests_generator(rd);
        auto r = requests_generator(rd);
        if (l > r) {
            std::swap(l, r);
        }
        assert(l >= 0 && l < numbers.size());
        assert(r >= 0 && r < numbers.size());
        const auto result = tree.query(l, r);
        const auto true_result = *std::min_element(std::next(numbers.begin(), l), std::next(numbers.begin(), r + 1));
        assert(result == true_result);
    }
    assert(numbers == tree.get_segment(0, n - 1));
    for (size_t i = 0; i < requests_number / 2; ++i) {
        auto l = requests_generator(rd);
        auto r = requests_generator(rd);
        if (l > r) {
            std::swap(l, r);
        }
        assert(l >= 0 && l < numbers.size());
        assert(r >= 0 && r < numbers.size());
        const auto value = numbers_generator(rd);
        tree.update(l, r, value);
        for (auto iter = std::next(numbers.begin(), l); iter != std::next(numbers.begin(), r + 1); ++iter) {
            *iter += value;
        }
    }
    assert(numbers == tree.get_segment(0, n - 1));
    for (size_t i = 0; i < requests_number; ++i) {
        auto l = requests_generator(rd);
        auto r = requests_generator(rd);
        if (l > r) {
            std::swap(l, r);
        }
        assert(l >= 0 && l < numbers.size());
        assert(r >= 0 && r < numbers.size());
        const auto result = tree.query(l, r);
        const auto true_result = *std::min_element(std::next(numbers.begin(), l), std::next(numbers.begin(), r + 1));
        assert(result == true_result);
    }
    assert(numbers == tree.get_segment(0, n - 1));
}

void run_all_tests() {
    test_from_task_1();
    test_from_task_2();
    test_random();
    test_big_numbers();
    test_one_element_consistent();
    test_one_element_inconsistent();
    test_partial_set_32bit();
    test_intersection();
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

    // Решение задачи
    // Читаем данные и инициализируем дерево
    size_t n{0}, m{0};
    std::ifstream fin("rmq.in");
    fin >> n >> m;

    // Создаём дерево
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree(
            std::vector<int64_t>(n, std::numeric_limits<int32_t>::min()),  // заполняем значениями -2^31, т.к. они минимальны по условию задачи
            operation,  // запросы минимума интересны
            std::numeric_limits<int64_t>::max(),  // нейтральный для минимума элемент
            [](const int64_t lhs, const int64_t rhs) {  // операция add в задаче не используется, но дерево нужно инициализировать
                return lhs + rhs;
            },
            0,  // нейтральный относительно сложения элемент
            [](const int64_t &old_value, const int64_t &new_value) {  // операция установки нового значения элемента
                    return std::max(new_value, old_value);  // в пересечениях отрезков запросов должен быть максимальный элемент
            });
    std::vector<std::tuple<size_t, size_t, int64_t>> requests;  // массив запросов
    requests.reserve(m);
    for (size_t i = 0; i < m; ++i) {
        size_t l{1}, r{1};
        int64_t q{0};
        fin >> l >> r >> q;
        requests.emplace_back(l, r, q);
        tree.set(l - 1, r - 1, q);  // заполняем дерево
    }
    fin.close();

    bool consistent = true;
    for (auto[l, r, q]: requests) {
        if (tree.query(l - 1, r - 1) != q) {  // проверяем, что дерево удовлетворяет требованиям
            consistent = false;
        }
    }

    // Вывод результата
    std::ofstream fout("rmq.out");
    if (consistent) {
        fout << "consistent" << std::endl;
        auto result = tree.get_segment(0, n - 1);
        for (auto element: result) {
            fout << element << " ";
        }
    } else {
        fout << "inconsistent";
    }
    fout.close();

    return 0;
}