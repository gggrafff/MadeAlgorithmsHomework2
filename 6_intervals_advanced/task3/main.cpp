
/*
 * C. Окна
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * На экране расположены прямоугольные окна, каким-то образом перекрывающиеся
 * (со сторонами, параллельными осям координат). Вам необходимо найти точку, которая покрыта наибольшим числом из них.
 *
 * Входные данные
 * В первой строке входного файла записано число окон n (1 ≤ n ≤ 50000).
 * Следующие n строк содержат координаты окон x(1, i) y(1, i) x(2, i) y(2, i),
 * где (x(1, i), y(1, i)) — координаты левого верхнего угла i-го окна,
 * а (x(2, i), y(2, i)) — правого нижнего (на экране компьютера y растет сверху вниз, а x — слева направо).
 * Все координаты — целые числа, по модулю не превосходящие 2·10^5.
 *
 * Выходные данные
 * В первой строке выходного файла выведите максимальное число окон,
 * покрывающих какую-либо из точек в данной конфигурации.
 * Во второй строке выведите два целых числа, разделенные пробелом — координаты точки,
 * покрытой максимальным числом окон. Окна считаются замкнутыми, т.е. покрывающими свои граничные точки.
 *
 * Примеры
 * Входные данные
 * 2
 * 0 0 3 3
 * 1 1 4 4
 * Выходные данные
 * 2
 * 1 3
 * Входные данные
 * 1
 * 0 0 1 1
 * Выходные данные
 * 1
 * 0 1
 */

/**
 * ==============================================
 * Для облегчения проверки:
 * Дерево отрезков скопировано в точности из 3 задачи 5 ДЗ.
 * ==============================================
 */


#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <set>
#include <functional>
#include <array>
#include <tuple>

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
     * @param number Число, которое нужно округлить вверх до степени двойки.
     * @return Число, равное степени двойки. Минимальное, но не меньше, чем number.
     */
    static size_t round_pow2(size_t number) {
        double x = static_cast<double>(number -
                                       1);  // если убрать "- 1", то "не меньше" в описании нужно заменить на "больше"
        auto answer = reinterpret_cast<unsigned int *>(&x);
        ++answer;
        return static_cast<size_t>(1) << (((*answer & 0x7FF00000) >> 20) - 1022);
    }

    /**
     * @brief Протолкнуть несоответствие значений узлов вниз(потомкам).
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

/**
 * @brief Структура описывает запросы к дереву.
 */
struct Request {
    int32_t left{0};  // x-координата границы окна
    int32_t top{0};  // y-координата верхней границы окна
    int32_t bottom{0};  // y-координата нижней границы окна
    int16_t diff{0};  // +1, если это левая граница; -1, если это правая граница
};

/**
 * @brief Из координат углов окна делает два запроса и добавляет в массив запросов.
 * @param x1 x-координата левой границы окна.
 * @param y1 y-координата верхней границы окна.
 * @param x2 x-координата правой границы окна.
 * @param y2 y-координата нижней границы окна.
 * @param requests Массив запросов.
 */
void window_to_requests(int32_t x1, int32_t y1, int32_t x2, int32_t y2, std::vector<Request> &requests) {
    requests.push_back({x1, y1, y2, +1});
    requests.push_back({x2, y1, y2, -1});
}

// Начало тестов

void test_square() {
    const int32_t &(*query)(const int32_t &, const int32_t &) = std::max<int32_t>;
    const size_t length = 2 * 100000;
    SegmentTree<int32_t> tree(std::vector<int32_t>(length, 0),
                              query,
                              std::numeric_limits<int32_t>::min(),
                              [](const int32_t lhs, const int32_t rhs) { return lhs + rhs; },
                              0);

    auto requestsCompare = [](auto &lhs, auto &rhs) {
        if (lhs.left == rhs.left) {
            return lhs.diff > rhs.diff;
        }
        return lhs.left < rhs.left;
    };
    std::vector<Request> requests;
    window_to_requests(1, 1, 3, 3, requests);
    window_to_requests(3, 1, 5, 3, requests);
    window_to_requests(1, 3, 3, 5, requests);
    window_to_requests(3, 3, 5, 5, requests);
    std::sort(requests.begin(), requests.end(), requestsCompare);
    int32_t max_cover = 0;
    std::pair<size_t, size_t> max_coords{0, 0};
    for (auto &request : requests) {
        tree.update(request.top, request.bottom, request.diff);
        const auto cover = tree.query(0, length - 1);
        if ((request.diff > 0) && (cover > max_cover)) {
            max_cover = cover;
            max_coords = {request.left, request.top};
        }
    }
    assert(max_cover == 4);
    assert(max_coords.first == 3);
    assert(max_coords.second == 3);
}

void test_equal_windows() {
    const int32_t &(*query)(const int32_t &, const int32_t &) = std::max<int32_t>;
    const size_t length = 2 * 100000;
    SegmentTree<int32_t> tree(std::vector<int32_t>(length, 0),
                              query,
                              std::numeric_limits<int32_t>::min(),
                              [](const int32_t lhs, const int32_t rhs) { return lhs + rhs; },
                              0);

    auto requestsCompare = [](auto &lhs, auto &rhs) {
        if (lhs.left == rhs.left) {
            return lhs.diff > rhs.diff;
        }
        return lhs.left < rhs.left;
    };
    std::vector<Request> requests;
    window_to_requests(1, 1, 3, 3, requests);
    window_to_requests(1, 1, 3, 3, requests);
    std::sort(requests.begin(), requests.end(), requestsCompare);
    int32_t max_cover = 0;
    std::pair<size_t, size_t> max_coords{0, 0};
    for (auto &request : requests) {
        tree.update(request.top, request.bottom, request.diff);
        const auto cover = tree.query(0, length - 1);
        if ((request.diff > 0) && (cover > max_cover)) {
            max_cover = cover;
            max_coords = {request.left, request.top};
        }
    }
    assert(max_cover == 2);
    assert((max_coords.second >= 1) && (max_coords.second <= 3));
    assert((max_coords.first >= 1) && (max_coords.first <= 3));
}

void test_pyramid() {
    const int32_t &(*query)(const int32_t &, const int32_t &) = std::max<int32_t>;
    const size_t length = 2 * 100000;
    SegmentTree<int32_t> tree(std::vector<int32_t>(length, 0),
                              query,
                              std::numeric_limits<int32_t>::min(),
                              [](const int32_t lhs, const int32_t rhs) { return lhs + rhs; },
                              0);

    auto requestsCompare = [](auto &lhs, auto &rhs) {
        if (lhs.left == rhs.left) {
            return lhs.diff > rhs.diff;
        }
        return lhs.left < rhs.left;
    };
    std::vector<Request> requests;
    window_to_requests(3, 1, 5, 5, requests);
    window_to_requests(3, 3, 5, 7, requests);
    window_to_requests(3, 3, 5, 5, requests);
    window_to_requests(1, 3, 3, 5, requests);
    std::sort(requests.begin(), requests.end(), requestsCompare);

    int32_t max_cover = 0;
    std::pair<size_t, size_t> max_coords{0, 0};
    for (auto &request : requests) {
        tree.update(request.top, request.bottom, request.diff);
        const auto cover = tree.query(0, length - 1);
        if ((request.diff > 0) && (cover > max_cover)) {
            max_cover = cover;
            max_coords = {request.left, request.top};
        }
    }
    assert(max_cover == 4);
    assert((max_coords.second >= 3) && (max_coords.second <= 5));
    assert(max_coords.first == 3);
}

void test_pyramid_2() {
    const size_t length = 2 * 100000;
    std::vector<std::pair<int32_t, size_t>> initial(length, {0, 0});
    for (size_t i = 0; i < length; ++i) {
        initial[i].second = i;
    }
    SegmentTree<std::pair<int32_t, size_t>> tree(initial,
                                                 [](const auto &lhs, const auto &rhs) {
                                                     if (lhs.first > rhs.first)
                                                         return lhs;
                                                     else
                                                         return rhs;
                                                 },
                                                 {std::numeric_limits<int32_t>::min(), 0},
                                                 [](const std::pair<int32_t, size_t> &lhs,
                                                    const std::pair<int32_t, size_t> &rhs) {
                                                     return std::pair<int32_t, size_t>(lhs.first + rhs.first,
                                                                                       std::max(lhs.second,
                                                                                                rhs.second));
                                                 },
                                                 {0, 0});

    auto requestsCompare = [](auto &lhs, auto &rhs) {
        if (lhs.left == rhs.left) {
            return lhs.diff > rhs.diff;
        }
        return lhs.left < rhs.left;
    };
    std::vector<Request> requests;
    window_to_requests(1, 3, 5, 5, requests);
    window_to_requests(3, 3, 7, 5, requests);
    window_to_requests(3, 3, 5, 5, requests);
    window_to_requests(3, 1, 5, 3, requests);
    std::sort(requests.begin(), requests.end(), requestsCompare);

    int32_t max_cover = 0;
    std::pair<size_t, size_t> max_coords{0, 0};
    for (auto &request : requests) {
        tree.update(request.top, request.bottom, {request.diff, 0});
        if (request.diff > 0) {
            const auto cover = tree.query(0, length - 1);
            if (cover.first > max_cover) {
                max_cover = cover.first;
                max_coords = {request.left, cover.second};
            }
        }
    }
    assert(max_cover == 4);
    assert((max_coords.first >= 3) && (max_coords.first <= 5));
    assert(max_coords.second == 3);
}

void test_pyramid_3() {
    const size_t length = 2 * 100000;
    std::vector<std::pair<int32_t, size_t>> initial(length, {0, 0});
    for (size_t i = 0; i < length; ++i) {
        initial[i].second = i;
    }
    SegmentTree<std::pair<int32_t, size_t>> tree(initial,
                                                 [](const auto &lhs, const auto &rhs) {
                                                     if (lhs.first > rhs.first)
                                                         return lhs;
                                                     else
                                                         return rhs;
                                                 },
                                                 {std::numeric_limits<int32_t>::min(), 0},
                                                 [](const std::pair<int32_t, size_t> &lhs,
                                                    const std::pair<int32_t, size_t> &rhs) {
                                                     return std::pair<int32_t, size_t>(lhs.first + rhs.first,
                                                                                       std::max(lhs.second,
                                                                                                rhs.second));
                                                 },
                                                 {0, 0});

    auto requestsCompare = [](auto &lhs, auto &rhs) {
        if (lhs.left == rhs.left) {
            return lhs.diff > rhs.diff;
        }
        return lhs.left < rhs.left;
    };
    std::vector<Request> requests;
    window_to_requests(1, 3, 5, 5, requests);
    window_to_requests(3, 3, 7, 5, requests);
    window_to_requests(3, 3, 5, 5, requests);
    window_to_requests(3, 5, 5, 7, requests);
    std::sort(requests.begin(), requests.end(), requestsCompare);

    int32_t max_cover = 0;
    std::pair<size_t, size_t> max_coords{0, 0};
    for (auto &request : requests) {
        tree.update(request.top, request.bottom, {request.diff, 0});
        if (request.diff > 0) {
            const auto cover = tree.query(0, length - 1);
            if (cover.first > max_cover) {
                max_cover = cover.first;
                max_coords = {request.left, cover.second};
            }
        }
    }
    assert(max_cover == 4);
    assert((max_coords.first >= 3) && (max_coords.first <= 5));
    assert(max_coords.second == 5);
}

void test_pyramid_4() {
    const size_t length = 2 * 100000;
    std::vector<std::pair<int32_t, size_t>> initial(length, {0, 0});
    for (size_t i = 0; i < length; ++i) {
        initial[i].second = i;
    }
    SegmentTree<std::pair<int32_t, size_t>> tree(initial,
                                                 [](const auto &lhs, const auto &rhs) {
                                                     if (lhs.first > rhs.first)
                                                         return lhs;
                                                     else
                                                         return rhs;
                                                 },
                                                 {std::numeric_limits<int32_t>::min(), 0},
                                                 [](const std::pair<int32_t, size_t> &lhs,
                                                    const std::pair<int32_t, size_t> &rhs) {
                                                     return std::pair<int32_t, size_t>(lhs.first + rhs.first,
                                                                                       std::max(lhs.second,
                                                                                                rhs.second));
                                                 },
                                                 {0, 0});

    auto requestsCompare = [](auto &lhs, auto &rhs) {
        if (lhs.left == rhs.left) {
            return lhs.diff > rhs.diff;
        }
        return lhs.left < rhs.left;
    };
    std::vector<Request> requests;
    window_to_requests(1, 3, 5, 5, requests);
    window_to_requests(3, 3, 7, 5, requests);
    window_to_requests(3, 3, 5, 5, requests);
    window_to_requests(5, 5, 7, 7, requests);
    std::sort(requests.begin(), requests.end(), requestsCompare);

    int32_t max_cover = 0;
    std::pair<size_t, size_t> max_coords{0, 0};
    for (auto &request : requests) {
        tree.update(request.top, request.bottom, {request.diff, 0});
        if (request.diff > 0) {
            const auto cover = tree.query(0, length - 1);
            if (cover.first > max_cover) {
                max_cover = cover.first;
                max_coords = {request.left, cover.second};
            }
        }
    }
    assert(max_cover == 4);
    assert(max_coords.first == 5);
    assert(max_coords.second == 5);
}

void test_negative_coords() {
    const size_t length = 2 * (2 * 100000 + 1);
    std::vector<std::pair<int32_t, size_t>> initial(length, {0, 0});
    for (size_t i = 0; i < length; ++i) {
        initial[i].second = i;
    }
    SegmentTree<std::pair<int32_t, size_t>> tree(initial,
                                                 [](const auto &lhs, const auto &rhs) {
                                                     if (lhs.first > rhs.first)
                                                         return lhs;
                                                     else
                                                         return rhs;
                                                 },
                                                 {std::numeric_limits<int32_t>::min(),
                                                  std::numeric_limits<size_t>::min()},
                                                 [](const std::pair<int32_t, size_t> &lhs,
                                                    const std::pair<int32_t, size_t> &rhs) {
                                                     return std::pair<int32_t, size_t>(lhs.first + rhs.first,
                                                                                       std::max(lhs.second,
                                                                                                rhs.second));
                                                 },
                                                 {0, std::numeric_limits<size_t>::min()});

    auto requestsCompare = [](auto &lhs, auto &rhs) {
        if (lhs.left == rhs.left) {
            return lhs.diff > rhs.diff;
        }
        return lhs.left < rhs.left;
    };
    std::vector<Request> requests;
    window_to_requests(-2, -2, -1, -1, requests);
    std::sort(requests.begin(), requests.end(), requestsCompare);

    int32_t max_cover = 0;
    std::pair<int32_t, int32_t> max_coords{0, 0};
    for (auto &request : requests) {
        tree.update(request.top + (2 * 100000 + 1), request.bottom + (2 * 100000 + 1), {request.diff, 0});
        if (request.diff > 0) {
            const auto cover = tree.query(0, length - 1);
            if (cover.first > max_cover) {
                max_cover = cover.first;
                max_coords = {request.left, cover.second - (2 * 100000 + 1)};
            }
        }
    }
    assert(max_cover == 1);
    assert((max_coords.first >= -2) && (max_coords.first <= -1));
    assert((max_coords.second >= -2) && (max_coords.second <= -1));
}

void test_1x1() {
    const size_t length = 2 * (2 * 100000 + 1);
    std::vector<std::pair<int32_t, size_t>> initial(length, {0, 0});
    for (size_t i = 0; i < length; ++i) {
        initial[i].second = i;
    }
    SegmentTree<std::pair<int32_t, size_t>> tree(initial,
                                                 [](const auto &lhs, const auto &rhs) {
                                                     if (lhs.first > rhs.first)
                                                         return lhs;
                                                     else
                                                         return rhs;
                                                 },
                                                 {std::numeric_limits<int32_t>::min(),
                                                  std::numeric_limits<size_t>::min()},
                                                 [](const std::pair<int32_t, size_t> &lhs,
                                                    const std::pair<int32_t, size_t> &rhs) {
                                                     return std::pair<int32_t, size_t>(lhs.first + rhs.first,
                                                                                       std::max(lhs.second,
                                                                                                rhs.second));
                                                 },
                                                 {0, std::numeric_limits<size_t>::min()});

    auto requestsCompare = [](auto &lhs, auto &rhs) {
        if (lhs.left == rhs.left) {
            return lhs.diff > rhs.diff;
        }
        return lhs.left < rhs.left;
    };
    std::vector<Request> requests;
    window_to_requests(-1, -1, -1, -1, requests);
    std::sort(requests.begin(), requests.end(), requestsCompare);

    int32_t max_cover = 0;
    std::pair<int32_t, int32_t> max_coords{0, 0};
    for (auto &request : requests) {
        tree.update(request.top + (2 * 100000 + 1), request.bottom + (2 * 100000 + 1), {request.diff, 0});
        if (request.diff > 0) {
            const auto cover = tree.query(0, length - 1);
            if (cover.first > max_cover) {
                max_cover = cover.first;
                max_coords = {request.left, cover.second - (2 * 100000 + 1)};
            }
        }
    }
    assert(max_cover == 1);
    assert(max_coords.first == -1);
    assert(max_coords.second == -1);
}

void test_small_windows() {
    const int32_t &(*query)(const int32_t &, const int32_t &) = std::max<int32_t>;
    const size_t length = 2 * 100000;
    SegmentTree<int32_t> tree(std::vector<int32_t>(length, 0),
                              query,
                              std::numeric_limits<int32_t>::min(),
                              [](const int32_t lhs, const int32_t rhs) { return lhs + rhs; },
                              0);

    auto requestsCompare = [](auto &lhs, auto &rhs) {
        if (lhs.left == rhs.left) {
            return lhs.diff > rhs.diff;
        }
        return lhs.left < rhs.left;
    };
    std::vector<Request> requests;
    window_to_requests(3, 1, 3, 3, requests);
    window_to_requests(3, 1, 3, 3, requests);
    window_to_requests(3, 1, 3, 3, requests);
    std::sort(requests.begin(), requests.end(), requestsCompare);

    int32_t max_cover = 0;
    std::pair<size_t, size_t> max_coords{0, 0};
    for (auto &request : requests) {
        tree.update(request.top, request.bottom, request.diff);
        const auto cover = tree.query(0, length - 1);
        if (cover > max_cover) {
            max_cover = cover;
            max_coords = {request.left, request.top};
        }
    }
    assert(max_cover == 3);
    assert((max_coords.second >= 1) && (max_coords.second <= 3));
    assert(max_coords.first == 3);
}

void run_all_tests() {
    test_square();
    test_equal_windows();
    test_pyramid();
    test_pyramid_2();
    test_pyramid_3();
    test_pyramid_4();
    test_small_windows();
    test_negative_coords();
    test_1x1();
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

    /**
     * В дереве отрезков сохраним состояние текущего столбца на экране (покрытие точек, находящихся в столбце).
     * Изначально текущий столбец - самый левый на экране.
     * Сдвигаем текущий столбец, при этом производим запросы к дереву:
     *      Если встретили начало окна, то делаем +1 для покрытия точек, которые пересекаются с окном.
     *      Если встретили конец окна, то делаем -1 для покрытия точек, которые пересекаются с окном.
     * После каждого шага берём максимум покрытия в столбце, если это наибольший максимум,
     * то сохраняем его вместе с координатами.
     */

    // Решение задачи
    size_t n{0};
    std::cin >> n;

    // Создадим массив для запросов дереву.
    auto requestsCompare = [](auto &lhs, auto &rhs) {
        if (lhs.left == rhs.left) {
            return lhs.diff > rhs.diff;
        }
        return lhs.left < rhs.left;
    };
    std::vector<Request> requests;
    requests.reserve(2 * n);

    // Для отладки массив windows используем.
    // Не уверен, что компилятор сможет соптимизировать его, поэтому можно выпилить из релиза.
    std::vector<std::tuple<int32_t, int32_t, int32_t, int32_t>> windows;
    windows.reserve(n);

    // Заполним массивы
    for (size_t i = 0; i < n; ++i) {
        int32_t x1{0}, y1{0}, x2{0}, y2{0};
        std::cin >> x1 >> y1 >> x2 >> y2;
        window_to_requests(x1, y1, x2, y2, requests);
        windows.emplace_back(x1, y1, x2, y2);
    }

    std::sort(requests.begin(), requests.end(), requestsCompare);  // сортируем запросы слева направо

    // Инициализируем дерево
    const int32_t max_abs_coord = (2 * 100000);  // Все координаты — целые числа, по модулю не превосходящие 2·10^5
    const size_t length = 2 * max_abs_coord + 1;  // Всего точек с -2·10^5 по 2·10^5
    std::vector<std::pair<int32_t, size_t>> initial(length, {0, 0});  // вектор, инициализирующий дерево
    for (size_t i = 0; i < length; ++i) {
        initial[i].second = i;  // расставляем y коориднату для точек
    }
    SegmentTree<std::pair<int32_t, size_t>> tree(  // элемент - точка экрана, пара чисел (покрытие, y координата)
            initial,
            [](const auto &lhs, const auto &rhs) {  // выбираем максимум по покрытию
                if (lhs.first > rhs.first)
                    return lhs;
                else
                    return rhs;
            },
            {std::numeric_limits<int32_t>::min(), std::numeric_limits<size_t>::min()},  // нейтральный для максимума
            [](const std::pair<int32_t, size_t> &lhs,
               const std::pair<int32_t, size_t> &rhs) {
                return std::pair<int32_t, size_t>(lhs.first + rhs.first,  // суммируем покрытие
                                                  std::max(lhs.second, rhs.second));
            },
            {0, std::numeric_limits<size_t>::min()});  // нейтральный для операции обновления

    // Сделаем запросы и найдём ответ
    int32_t max_cover = 0;
    std::pair<int32_t, int32_t> max_coords{0, 0};
    for (auto &request : requests) {
        tree.update(request.top + max_abs_coord, request.bottom + max_abs_coord, {request.diff, 0});
        if (request.diff > 0) {
            const auto cover = tree.query(0, length - 1);
            if (cover.first > max_cover) {
                max_cover = cover.first;
                max_coords = {request.left, cover.second - max_abs_coord};
            }
        }
    }

    // Проверки для отладки
    assert((max_coords.first <= max_abs_coord) && (max_coords.first >= -max_abs_coord));
    assert((max_coords.second <= max_abs_coord) && (max_coords.second >= -max_abs_coord));

    int32_t max_cover_debug = 0;
    for (auto[x1, y1, x2, y2]: windows) {
        if ((max_coords.first <= x2) && (max_coords.first >= x1)) {
            if ((max_coords.second <= y2) && (max_coords.second >= y1)) {
                max_cover_debug += 1;
            }
        }
    }
    assert(max_cover_debug == max_cover);

    // Вывод результата
    std::cout << max_cover << "\n" << max_coords.first << " " << max_coords.second;
    return 0;
}
