/*
 * C. RMQ2
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Входные данные
 * В первой строке находится число n — размер массива. (1 ≤ n ≤ 105)
 * Во второй строке находится n чисел ai — элементы массива.
 * Далее содержится описание операций, их количество не превышает 2·10^5.
 * В каждой строке находится одна из следующих операций:
 *     set i j x — установить все a[k], i ≤ k ≤ j в x.
 *     add i j x — увеличить все a[k], i ≤ k ≤ j на x.
 *     min i j — вывести значение минимального элемента в массиве на отрезке с i по j, гарантируется, что (1 ≤ i ≤ j ≤ n).
 * Все числа во входном файле и результаты выполнения всех операций не превышают по модулю 10^18.
 * Выходные данные
 * Выведите последовательно результат выполнения всех операций min. Следуйте формату выходного файла из примера.
 *
 * Пример
 * Входные данные
 * 5
 * 1 2 3 4 5
 * min 2 5
 * min 1 5
 * min 1 4
 * min 2 4
 * set 1 3 10
 * add 2 4 4
 * min 2 5
 * min 1 5
 * min 1 4
 * min 2 4
 * Выходные данные
 * 2
 * 1
 * 1
 * 2
 * 5
 * 5
 * 8
 * 8
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <random>

/**
 * @brief Реализация дерева отрезков с возможностью задать функцию для запроса и функцию обновления узлов.
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
     */
    explicit SegmentTree(
            const std::vector<ElementType> &elements,
            const std::function<ElementType(const ElementType &lhs, const ElementType &rhs)> query_operation,
            const ElementType &query_operation_neutral_element,
            const std::function<ElementType(const ElementType &lhs, const ElementType &rhs)> update_operation,
            const ElementType &update_operation_neutral_element) :
                query_operation_{query_operation},
                query_operation_neutral_element_{query_operation_neutral_element},
                update_operation_{update_operation},
                update_operation_neutral_element_{update_operation_neutral_element} {
        size_t n = round_pow2(elements.size());  // удобно, когда размер дерева - степень двойки, дополним массив нейтральными элементами
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
    void update(size_t l, size_t r, const ElementType value) {
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
    void set(size_t l, size_t r, const ElementType value) {
        if (l > r) {
            std::swap(l, r);
        }
        set_node(1, l, r, value);
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

    /**
     * @brief Протолкнуть несоотвествие значений узлов вниз(потомкам).
     * @param node Индекс узла, из которого проталкиваем.
     */
    void push_imbalance(size_t node) {
        if (tree_[node].refreshed) {
            tree_[2 * node].value = tree_[node].value;
            tree_[2 * node].imbalance = tree_[node].imbalance;
            tree_[2 * node].refreshed = true;
            tree_[2 * node + 1].value = tree_[node].value;
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
     * @brief Обновить значения листов, за которые отвечает узел. Используется функция обновления.
     * @param node Индекс обновляемого узла.
     * @param from Левая граница всего отрезка, который нужно обновить.
     * @param to Правая граница всего отрезка, который нужно обновить.
     * @param value Значение, на которое обновляем.
     */
    void update_node(size_t node, size_t from, size_t to, ElementType value) {
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
    void set_node(size_t node, size_t from, size_t to, ElementType value) {
        auto l = tree_[node].left;
        auto r = tree_[node].right;
        if (!segments_intersection({l, r}, {from, to})) {  // данный узел не нужен для установки значений отрезка
            return;
        }
        if (segments_belong({l, r}, {from, to})) {  // данный узел полностью нужен для установки значений отрезка
            tree_[node].imbalance = update_operation_neutral_element_;
            tree_[node].value = value;
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

void test_from_task() {
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree({1LL, 2LL, 3LL, 4LL, 5LL},
                              operation,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0);
    assert(tree.query(1, 4) == 2);
    assert(tree.query(0, 4) == 1);
    assert(tree.query(0, 3) == 1);
    assert(tree.query(1, 3) == 2);
    tree.set(0, 2, 10);
    tree.update(1, 3, 4);
    assert(tree.query(1, 4) == 5);
    assert(tree.query(0, 4) == 5);
    assert(tree.query(0, 3) == 8);
    assert(tree.query(1, 3) == 8);
}

void test_set_update_set_update() {
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree({1LL, 2LL, 3LL, 4LL, 5LL},
                              operation,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0);
    assert(tree.query(1, 4) == 2);
    assert(tree.query(0, 4) == 1);
    assert(tree.query(0, 3) == 1);
    assert(tree.query(1, 3) == 2);
    tree.set(0, 2, 10);
    tree.update(1, 3, -4);
    tree.set(1, 2, 1);
    tree.update(1, 4, 2);
    assert(tree.query(1, 4) == 2);
    assert(tree.query(0, 4) == 2);
    assert(tree.query(0, 2) == 3);
    assert(tree.query(1, 2) == 3);
}

void test_query() {
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree({1LL, 2LL, 3LL, 4LL, 5LL},
                              operation,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0);
    assert(tree.query(1, 4) == 2);
    assert(tree.query(0, 4) == 1);
    assert(tree.query(0, 3) == 1);
    assert(tree.query(1, 3) == 2);
}

void test_update() {
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree({1LL, 2LL, 3LL, 4LL, 5LL},
                              operation,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0);
    tree.update(1, 3, 4);
    assert(tree.query(1, 4) == 5);
    assert(tree.query(0, 4) == 1);
    assert(tree.query(0, 3) == 1);
    assert(tree.query(1, 3) == 6);
}

void test_set() {
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree({1LL, 2LL, 3LL, 4LL, 5LL},
                              operation,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0);
    tree.set(0, 2, 10);
    assert(tree.query(1, 4) == 4);
    assert(tree.query(0, 4) == 4);
    assert(tree.query(0, 3) == 4);
    assert(tree.query(1, 3) == 4);
}

void test_several_set() {
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree({1LL, 2LL, 3LL, 4LL, 5LL},
                              operation,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0);
    tree.set(0, 2, 10);
    tree.set(2, 4, -7);
    assert(tree.query(1, 1) == 10);
    assert(tree.query(0, 1) == 10);
    assert(tree.query(0, 3) == -7);
    assert(tree.query(1, 3) == -7);
}

void test_random() {
    std::vector<int64_t> numbers;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int64_t> numbers_generator(-1000 * 1000, 1000 * 1000);
    const size_t n = 100;
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
            0
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
}

void run_all_tests() {
    test_query();
    test_update();
    test_set();
    test_from_task();
    test_several_set();
    test_set_update_set_update();
    test_random();
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
    // Чтение массива чисел
    size_t n{0};
    std::cin >> n;
    std::vector<int64_t> numbers(n, 0);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> numbers[i];
    }
    // Создание дерева
    const int64_t &(*query)(const int64_t &, const int64_t &) = std::min<int64_t>;
    SegmentTree<int64_t> tree(numbers,
                              query,
                              std::numeric_limits<int64_t>::max(),
                              [](const int64_t lhs, const int64_t rhs) { return lhs + rhs; },
                              0);
    // Операции над деревом и запросы
    std::string operation;
    while (std::cin >> operation) {
        if (operation == "min") {
            size_t l{0}, r{0};
            std::cin >> l >> r;
            assert(l != 0 && l <= numbers.size());
            assert(r != 0 && r <= numbers.size());
            std::cout << tree.query(l - 1, r - 1) << std::endl;
        } else if (operation == "set") {
            size_t l{0}, r{0};
            int64_t value{0};
            std::cin >> l >> r >> value;
            assert(l != 0 && l <= numbers.size());
            assert(r != 0 && r <= numbers.size());
            tree.set(l - 1, r - 1, value);
        } else if (operation == "add") {
            size_t l{0}, r{0};
            int64_t value{0};
            std::cin >> l >> r >> value;
            assert(l != 0 && l <= numbers.size());
            assert(r != 0 && r <= numbers.size());
            tree.update(l - 1, r - 1, value);
        } else {
            break;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return 0;
}