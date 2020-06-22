/*
 * D. Менеджер памяти
 * ограничение по времени на тест 10 секунд
 * ограничение по памяти на тест 1024 мегабайта
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Одно из главных нововведений новейшей операционной системы Indows 7 — новый менеджер памяти.
 * Он работает с массивом длины N и позволяет выполнять три самые современные операции:
 * copy(a, b, l) — скопировать отрезок длины [a,a+l−1] в [b,b+l−1]
 * sum(l, r) — посчитать сумму элементов массива на отрезке [l,r]
 * print(l, r) — напечатать элементы с l по r, включительно
 * Вы являетесь разработчиком своей операционной системы, и Вы, безусловно, не можете обойтись без инновационных
 * технологий. Вам необходимо реализовать точно такой же менеджер памяти.
 *
 * Входные данные
 * Первая строка входного файла содержит целое число N (1≤N≤1000000) — размер массива, с которым будет работать
 * Ваш менеджер памяти.
 * Во второй строке содержатся четыре числа 1≤X1,A,B,M≤10^9+10.
 * С помощью них можно сгенерировать исходный массив чисел X1,X2,⋯,XN. Xi+1=(A∗Xi+B)modM
 * Следующая строка входного файла содержит целое число K (1≤K≤200000) — количество запросов, которые необходимо
 * выполнить Вашему менеджеру памяти.
 * Далее в K строках содержится описание запросов. Запросы заданы в формате:
 * cpy a b l — для операции copy
 * sum l r — для операции sum (l≤r)
 * out l r — для операции print (l≤r)
 * Гарантируется, что суммарная длина запросов print не превышает 3000. Также гарантируется, что все запросы корректны.
 *
 * Выходные данные
 * Для каждого запроса sum или print выведите в выходной файл на отдельной строке результат запроса.
 *
 * Пример
 * входные данные
 * 6
 * 1 4 5 7
 * 7
 * out 1 6
 * cpy 1 3 2
 * out 1 6
 * sum 1 4
 * cpy 1 2 4
 * out 1 6
 * sum 1 6
 * выходные данные
 * 1 2 6 1 2 6
 * 1 2 1 2 2 6
 * 6
 * 1 1 2 1 2 6
 * 13
 */

/*
 * Подробнее о :
 *
 */

#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <functional>
#include <math.h>
#include <array>
#include <utility>
#include <iomanip>
#include <optional>
#include <complex>
#include <string>
#include <numeric>
#include <cctype>
#include <utility>
#include <stack>

/**
 * @brief Генератор последовательностей вида a[i] = (x * a[i-1] + y) mod n.
 * @tparam CoefficientsType Тип коэффициентов x, y.
 * @tparam SequenceType Тип элементов a[i].
 */
template<typename CoefficientsType, typename SequenceType>
class SequenceGenerator {
public:
    /**
     * @brief Конструируем генератор.
     * @param x Коэффициент х из выражения a[i] = (x * a[i-1] + y) mod n.
     * @param y Коэффициент y из выражения a[i] = (x * a[i-1] + y) mod n.
     * @param a0 Первый элемент последовательности.
     * @param module Модуль n из выражения a[i] = (x * a[i-1] + y) mod n.
     */
    SequenceGenerator(CoefficientsType x, CoefficientsType y, SequenceType a0, uint64_t module) :
            x_(x),
            y_(y),
            a_current_(a0),
            module_(module) {}

    ~SequenceGenerator() = default;

    // SequenceGenerator(const SequenceGenerator &) = default;
    // SequenceGenerator(SequenceGenerator &&) = default;

    SequenceGenerator &operator=(const SequenceGenerator &other) {
        if (this != &other) { // self-assignment check expected
            module_ = other.module_;
            x_ = other.x_;
            y_ = other.y_;
            a_current_ = other.a_current_;
        }
        return *this;
    };
    // SequenceGenerator &operator=(SequenceGenerator &&) = default;

    /**
     * @brief Генерирует следующий элемент последовательности.
     * @return Следующий элемент последовательности.
     */
    SequenceType next() {
        a_current_ = (x_ * a_current_ + y_) % module_;
        return a_current_;
    }

    /**
     * @brief Генерирует следующий элемент последовательности c разовой корректировкой коэффициентов и текущего состояния.
     * @details a[i] = ((x + dx) * (a[i-1] + da) + (y + dy)) mod n
     * @param dx Поправка для коэффициента x.
     * @param dy Поправка для коэффициента y.
     * @param da Поправка для текущего элемента.
     * @return Следующий элемент последовательности.
     */
    SequenceType next(CoefficientsType dx, CoefficientsType dy, SequenceType da) {
        a_current_ = ((x_ + dx) * (a_current_ + da) + (y_ + dy)) % module_;
        return a_current_;
    }

    /**
     * @brief Возвращает текущий элемент последовательности.
     * @return Текущий элемент последовательности.
     */
    SequenceType get_current() {
        return a_current_;
    }

    /**
     * @brief Генерирует следующий элемент последовательности и берет его по заданному модулю.
     * @param module Модуль, по которому нужно взять следующий элемент.
     * @return Остаток от деления следующего элемента последовательности на module.
     */
    SequenceType next_by_module(uint64_t module) {
        return next() % module;
    }

    /**
     * @brief Генерирует несколько следующих элементов последовательности.
     * @param count Количество генерируемых элементов.
     * @return Массив сгенерированных элементов.
     */
    std::vector<SequenceType> next(size_t count) {
        std::vector<SequenceType> result;
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(next());
        }
        return result;
    }

    /**
     * @brief Генерирует несколько следующих элементов последовательности и возвращает вместе с текущим элементом.
     * @param length Общая длина последовательности.
     * @return Текущий элемент и несколько сгенерированных.
     */
    std::vector<SequenceType> get_sequence(size_t length) {
        std::vector<SequenceType> result;
        result.reserve(length);
        result.push_back(a_current_);
        for (size_t i = 0; i < length - 1; ++i) {
            result.push_back(next());
        }
        return result;
    }

    /**
     * @brief Генерирует несколько следующих элементов последовательности и возвращает вместе с текущим элементом.
     * @brief Элементы берутся по заданному модулю.
     * @param length Общая длина последовательности.
     * @param module Модуль, по которому берём элементы.
     * @return Текущий элемент и несколько сгенерированных.
     */
    std::vector<SequenceType> get_sequence_by_module(size_t length, uint64_t module) {
        std::vector<SequenceType> result;
        result.reserve(length);
        result.push_back(a_current_ % module);
        for (size_t i = 0; i < length - 1; ++i) {
            result.push_back(next_by_module(module));
        }
        return result;
    }

private:
    uint64_t module_{1 << 16};
    CoefficientsType x_{0};
    CoefficientsType y_{0};
    SequenceType a_current_{0};
};

class MemoryManager;

template<typename T>
class TreapImplicit {
public:
    TreapImplicit() = default;

    virtual ~TreapImplicit() = default;

    virtual void push_back(const T &value) {
        insert(size(), value);
    }

    virtual void push_front(const T &value) {
        insert(0, value);
    }

    virtual void insert(const size_t position, const T &value) {
        if (size() == 0) {
            nodes_.push_back({value, value, generate_priority(), 1, std::nullopt, std::nullopt});
            return;
        }
        if (position == 0) {
            nodes_.push_back({value, value, generate_priority(), 1, std::nullopt, std::nullopt});
            root_ = TreapImplicit<T>::merge(nodes_.size() - 1, root_);
        } else if (position == size()) {
            nodes_.push_back({value, value, generate_priority(), 1, std::nullopt, std::nullopt});
            root_ = TreapImplicit<T>::merge(root_, nodes_.size() - 1);
        } else {
            auto[t1, t2] = split(root_, position);
            nodes_.push_back({value, value, generate_priority(), 1, std::nullopt, std::nullopt});
            root_ = TreapImplicit<T>::merge(t1, nodes_.size() - 1);
            root_ = TreapImplicit<T>::merge(root_, t2);
        }
    }

    virtual void erase(size_t position) {
        erase(root_, position);
    }

    virtual size_t size() const {
        if (nodes_.size() > 0) {
            auto size = nodes_[root_.value()].count;
            assert(size <= nodes_.size());
            return size;
        } else {
            return 0;
        }
    }

    void print(size_t from, size_t to) {
        print(root_, from, to);
        std::cout << "\n";
    }

    T query(size_t from, size_t to) {
        return query(root_, from, to);
    }

protected:
    using NodePointer = std::optional<size_t>;

    virtual uint64_t generate_priority() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<uint64_t> priority_generator;

        return priority_generator(gen);
    }

    virtual void erase(const NodePointer root_idx, const size_t target_element_idx) {
        if (!root_idx.has_value()) {
            return;
        }

        size_t left_count =0;
        if (nodes_[root_idx.value()].left.has_value()){
            left_count = nodes_[nodes_[root_idx.value()].left.value()].count;
        }

        if (left_count == target_element_idx) {
            auto subtree_root_idx = merge(nodes_[root_idx.value()].left, nodes_[root_idx.value()].right);
            nodes_[root_idx.value()] = nodes_[subtree_root_idx.value()];
            nodes_.erase(nodes_.begin() + subtree_root_idx.value());
        } else if (left_count > target_element_idx) {
            erase(nodes_[root_idx.value()].left, target_element_idx);
            update(root_idx);
        } else {
            erase(nodes_[root_idx.value()].right, target_element_idx - left_count - 1);
            update(root_idx);
        }
    }

    virtual NodePointer find(const NodePointer root_idx, const size_t target_element_idx) const {
        if (!root_idx.has_value()) {
            return std::nullopt;
        }

        auto &root = nodes_[root_idx.value()];
        auto &left = nodes_[root.left.value()];

        if (left.count == target_element_idx) {
            return root_idx;
        } else if (left.count > target_element_idx) {
            return find(root.left, target_element_idx);
        } else {
            return find(root.right, target_element_idx - left.count - 1);
        }
    }

    virtual std::tuple<NodePointer, NodePointer> split(NodePointer root_idx, size_t k) {
        if (!root_idx.has_value()) { return {std::nullopt, std::nullopt}; }

        size_t left_count =0;
        if (nodes_[root_idx.value()].left.has_value()){
            left_count = nodes_[nodes_[root_idx.value()].left.value()].count;
        }

        if (left_count >= k) {
            auto[t1, t2] = split(nodes_[root_idx.value()].left, k);
            nodes_[root_idx.value()].left = t2;
            update(root_idx);
            return {t1, root_idx};
        } else {
            auto[t1, t2] = split(nodes_[root_idx.value()].right, k - left_count - 1);
            nodes_[root_idx.value()].right = t1;
            update(root_idx);
            return {root_idx, t2};
        }
    }

    virtual NodePointer merge(NodePointer root1_idx, NodePointer root2_idx) {
        if (!root2_idx.has_value()) { return root1_idx; }
        if (!root1_idx.has_value()) { return root2_idx; }

        if (nodes_[root1_idx.value()].priority > nodes_[root2_idx.value()].priority) {
            nodes_[root1_idx.value()].right = TreapImplicit<T>::merge(nodes_[root1_idx.value()].right, root2_idx);
            update(root1_idx);
            return root1_idx;
        } else {
            nodes_[root2_idx.value()].left = TreapImplicit<T>::merge(root1_idx, nodes_[root2_idx.value()].left);
            update(root2_idx);
            return root2_idx;
        }
    }

    virtual void update(NodePointer node) {
        if (!node.has_value()) {
            return;
        }
        auto idx = node.value();

        auto count = 1;
        auto query_result = nodes_[idx].value;

        if (nodes_[idx].left.has_value()){
            count += nodes_[nodes_[idx].left.value()].count;
            query_result = query_operation_(query_result, nodes_[nodes_[idx].left.value()].query_result);
        }
        if (nodes_[idx].right.has_value()){
            count += nodes_[nodes_[idx].right.value()].count;
            query_result = query_operation_(query_result, nodes_[nodes_[idx].right.value()].query_result);
        }

        nodes_[idx].count = count;
        nodes_[idx].query_result = query_result;
    }

    /**
     * @brief Пересекаются ли отрезки?
     * @param lhs Первый отрезок.
     * @param rhs Второй отрезок.
     * @return true, если пересекаются, иначе - false.
     */
    static bool segments_intersection(const std::pair<int64_t, int64_t> lhs, const std::pair<int64_t, int64_t> rhs) {
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
    static bool segments_belong(const std::pair<int64_t, int64_t> inside, const std::pair<int64_t, int64_t> outside) {
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

    void print(NodePointer root_idx, int64_t from, int64_t to) const {
        if (!root_idx.has_value()) {
            return;
        }

        auto &root = nodes_[root_idx.value()];
        size_t left_count = 0;

        if (root.left.has_value()) {
            auto &left = nodes_[root.left.value()];
            if (segments_intersection({0, left.count - 1}, {from, to})) {
                print(root.left, from, to);
            }
            left_count = left.count;
        }

        if ((left_count >= from) && (left_count <= to)) {
            std::cout << root.value << " ";
        }

        if (root.right.has_value()){
            auto &right = nodes_[root.right.value()];
            if (segments_intersection({left_count + 1, left_count + 1 + right.count - 1}, {from, to})) {
                print(root.right, from - left_count - 1, to - left_count - 1);
            }
        }
    }

    T query(NodePointer root_idx, int64_t from, int64_t to) const {
        if (!root_idx.has_value()) {
            return neutral_element_;
        }

        assert(root_idx.value() < nodes_.size());
        auto& root = nodes_[root_idx.value()];
        size_t left_count = 0;
        if (root.left.has_value()){
            assert(root.left.value() < nodes_.size());
            left_count = nodes_[root.left.value()].count;
            assert(left_count <= size());
        }
        size_t right_count = 0;
        if (root.right.has_value()){
            assert(root.right.value() < nodes_.size());
            right_count = nodes_[root.right.value()].count;
            assert(left_count <= size());
        }

        size_t left_border = 0;
        size_t right_border = left_count + 1 + right_count - 1;

        if (!segments_intersection({left_border, right_border}, {from, to})) {
            return neutral_element_;
        }
        assert(to>=0);
        if (segments_belong({left_border, right_border}, {from, to})) {
            return root.query_result;
        }
        auto result = neutral_element_;
        auto length = to - from + 1;
        if (left_count > 0) {
            result = query_operation_(result,query(root.left, from, std::min(from + left_count - 1, from + length - 1)));
        }
        if (left_count < length) {
            result = query_operation_(result, root.value);
        }
        if ((right_count > 0) && (left_count + 1 < length)) {
            result = query_operation_(result,query(root.right, std::max(0ll, from - 1 - left_count), to - 1 - left_count));
        }
        return result;
    }

    struct Node {
        T value;
        T query_result;
        uint64_t priority{0};
        uint64_t count{1};
        NodePointer left{0};
        NodePointer right{0};
    };

    std::vector<Node> nodes_;
    NodePointer& get_root() {
        return root_;
    }
    void set_root(NodePointer root) {
        root_ = root;
    }
private:
    NodePointer root_{0};
    std::function<T(const T &lhs, const T &rhs)> query_operation_ = [](auto &lhs, auto &rhs) { return lhs + rhs; };
    T neutral_element_{0};
};

namespace persistence {
    template<typename T>
    class TreapImplicitPersistent: public TreapImplicit<T> {
    public:
        void copy(size_t from_begin, size_t to_begin, size_t length) {
            auto for_copy = std::get<1>(split(TreapImplicit<T>::get_root(), from_begin));
            for_copy = std::get<0>(split(for_copy, length));

            auto left_part = std::get<0>(split(TreapImplicit<T>::get_root(), to_begin));
            auto right_part = std::get<1>(split(TreapImplicit<T>::get_root(), to_begin + length));

            auto new_root = merge(left_part, for_copy);
            new_root = merge(new_root, right_part);
            TreapImplicit<T>::set_root(new_root);
        }
    protected:
        using NodePointer = typename TreapImplicit<T>::NodePointer;
        std::tuple<NodePointer, NodePointer> split(NodePointer root_idx, size_t k) override {
            if (!root_idx.has_value()) { return {std::nullopt, std::nullopt}; }

            auto& nodes = TreapImplicit<T>::nodes_;

            nodes.push_back(nodes[root_idx.value()]);
            root_idx = nodes.size() - 1;

            size_t left_count = 0;
            if (nodes[root_idx.value()].left.has_value()){
                left_count = nodes[nodes[root_idx.value()].left.value()].count;
            }

            if (left_count >= k) {
                auto[t1, t2] = split(nodes[root_idx.value()].left, k);
                nodes[root_idx.value()].left = t2;
                TreapImplicit<T>::update(root_idx);
                return {t1, root_idx};
            } else {
                auto[t1, t2] = split(nodes[root_idx.value()].right, k - left_count - 1);
                nodes[root_idx.value()].right = t1;
                TreapImplicit<T>::update(root_idx);
                return {root_idx, t2};
            }
        }

        NodePointer merge(NodePointer root1_idx, NodePointer root2_idx) override{
            if (!root2_idx.has_value()) { return root1_idx; }
            if (!root1_idx.has_value()) { return root2_idx; }

            auto& nodes = TreapImplicit<T>::nodes_;

            auto left_size = nodes[root1_idx.value()].count;
            auto right_size = nodes[root2_idx.value()].count;
            auto rang = TreapImplicit<T>::generate_priority() % (left_size + right_size);
            if (rang > right_size) {
                nodes.push_back(nodes[root1_idx.value()]);
                root1_idx = nodes.size() - 1;

                nodes[root1_idx.value()].right = merge(nodes[root1_idx.value()].right, root2_idx);
                TreapImplicit<T>::update(root1_idx);
                return root1_idx;
            } else {
                nodes.push_back(nodes[root2_idx.value()]);
                root2_idx = nodes.size() - 1;

                nodes[root2_idx.value()].left = merge(root1_idx, nodes[root2_idx.value()].left);
                TreapImplicit<T>::update(root2_idx);
                return root2_idx;
            }
        }
    private:

    };
}


class MemoryManager {
public:
    MemoryManager(SequenceGenerator<int64_t, int64_t> array_generator, size_t N) {
        assert(N > 0);
        memory_.push_back(array_generator.get_current());
        for (size_t i = 1; i < N; ++i) {
            memory_.push_back(array_generator.next());
        }
    }

    void print(size_t from, size_t to) {
        memory_.print(from, to);
    }

    uint32_t sum(size_t from, size_t to) {
        return memory_.query(from, to);
    }

    void copy(size_t from_begin, size_t to_begin, size_t length) {
        memory_.copy(from_begin, to_begin, length);
    }

private:
    persistence::TreapImplicitPersistent<uint32_t> memory_;
};


// Начало тестов

void test_from_task() {
    size_t N = 6;
    SequenceGenerator<int64_t, int64_t> array_generator(4, 5, 1, 7);
    MemoryManager manager(array_generator, N);
    manager.print(0, 5);
    std::cout << std::endl;
    manager.copy(0, 2, 2);
    manager.print(0, 5);
    std::cout << std::endl;
    auto sum1 = manager.sum(0, 3);
    assert(sum1 == 6);
    manager.copy(0, 1, 4);
    manager.print(0, 5);
    std::cout << std::endl;
    auto sum2 = manager.sum(0, 5);
    assert(sum2 == 13);
}


void run_all_tests() {
    test_from_task();
}

// Конец тестов

int main(int argc, char *argv[]) {
    /*run_all_tests();
    return 0;*/

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
    size_t N{0};
    std::cin >> N;
    int64_t x0, x, y, m;
    std::cin >> x0 >> x >> y>> m;
    SequenceGenerator<int64_t, int64_t> array_generator(x, y, x0, m);
    MemoryManager manager(array_generator, N);
    size_t K{0};
    std::cin >> K;
    for (size_t i=0;i<K;++i) {
        std::string operation;
        std::cin >> operation;
        if (operation == "cpy") {
            size_t a, b, l;
            std::cin >> a >> b >>l;
            manager.copy(a - 1, b - 1, l);
        } else if (operation == "sum") {
            size_t l, r;
            std::cin >> l >> r;
            std::cout << manager.sum(l - 1, r - 1) << "\n";
        } else if (operation == "out") {
            size_t l, r;
            std::cin >> l >> r;
            manager.print(l - 1, r - 1);
        }
    }

    return 0;
}
