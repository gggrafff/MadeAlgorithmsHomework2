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

template<typename T>
class CoordinatesCompressor {
public:
    CoordinatesCompressor() = default;

    CoordinatesCompressor(SequenceGenerator<int64_t, int64_t> generator, size_t N) {
        compressed_coordinates_.reserve(N);
        compressed_coordinates_.push_back(generator.get_current());
        for (size_t i = 1; i < N; ++i) {
            compressed_coordinates_.push_back(generator.next());
        }

        sort(compressed_coordinates_.begin(), compressed_coordinates_.end());
        compressed_coordinates_.erase(unique(compressed_coordinates_.begin(), compressed_coordinates_.end()),
                                      compressed_coordinates_.end());
    }

    explicit CoordinatesCompressor(std::vector<T> coordinates, bool sorted = false) : compressed_coordinates_(
            std::move(coordinates)) {
        if (!sorted) {
            sort(compressed_coordinates_.begin(), compressed_coordinates_.end());
        }
        compressed_coordinates_.erase(unique(compressed_coordinates_.begin(), compressed_coordinates_.end()),
                                      compressed_coordinates_.end());
    }

    CoordinatesCompressor(const CoordinatesCompressor &) = delete;

    CoordinatesCompressor(CoordinatesCompressor &&) = delete;

    CoordinatesCompressor &operator=(const CoordinatesCompressor &other) = delete;

    CoordinatesCompressor &operator=(CoordinatesCompressor &&) = delete;

    [[nodiscard]] size_t compress(T coordinate) const {
        return std::distance(compressed_coordinates_.begin(),
                             std::lower_bound(compressed_coordinates_.begin(), compressed_coordinates_.end(),
                                              coordinate));
    }

    [[nodiscard]] T decompress(size_t compressed_coordinate) const {
        return compressed_coordinates_[compressed_coordinate];
    }

    [[nodiscard]] size_t max_compressed() const {
        return compressed_coordinates_.size() - 1;
    }

private:
    std::vector<T> compressed_coordinates_;
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
        if (position == 0){
            nodes_.push_back({value, value, generate_priority(), 1, 0, 0});
            root_ = merge(nodes_.size() - 1, root_);
        } else if (position == size()){
            nodes_.push_back({value, value, generate_priority(), 1, 0, 0});
            root_ = merge(root_, nodes_.size() - 1);
        } else {
            auto [t1, t2] = split(root_, position);
            nodes_.push_back({value, value, generate_priority(), 1, 0, 0});
            root_ = merge(t1, nodes_.size() - 1);
            root_ = merge(root_, t2);
        }
    }

    virtual void erase(size_t position) {
        erase(root_, position);
    }

    virtual size_t size() const {
        auto size = nodes_.size();
        if (size > 0) {
            assert(nodes_[root_].count == size);
        }
        return size;
    }

    friend MemoryManager;
protected:
    using NodePointer = size_t;

    virtual uint64_t generate_priority() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<uint64_t> priority_generator;

        return priority_generator(gen);
    }

    virtual void erase(const NodePointer root_idx, const size_t target_element_idx) {
        auto &root = nodes_[root_idx];
        auto &left = nodes_[root.left];

        if (left.count == target_element_idx) {
            auto subtree_root_idx = merge(nodes_[root_idx].left, nodes_[root_idx].right);
            nodes_[root_idx] = nodes_[subtree_root_idx];
            nodes_.erase(nodes_.begin() + subtree_root_idx);
        } else if (left.count > element_idx) {
            erase(root.left, target_element_idx);
            update(root_idx);
        } else {
            erase(root.right, target_element_idx);
            update(root_idx);
        }
    }

    virtual NodePointer find(const NodePointer root_idx, const size_t target_element_idx) const {
        auto &root = nodes_[root_idx];
        auto &left = nodes_[root.left];

        if (left.count == target_element_idx) {
            return root_idx;
        } else if (left.count > element_idx) {
            return find(root.left, target_element_idx);
        } else {
            return find(root.right, target_element_idx);
        }
    }

    virtual std::tuple<NodePointer, NodePointer> split(NodePointer root_idx, size_t k) {
        if (root_idx == 0) { return {0, 0}; }

        auto &root = nodes_[root_idx];
        auto &left = nodes_[root.left];

        if (left.count >= k) {
            auto[t1, t2] = split(root.left, k);
            root.left = t2;
            update(root_idx);
            return {t1, root_idx};
        } else {
            auto[t1, t2] = split(root.right, k - left.count - 1);
            root.right = t1;
            update(root_idx);
            return {root_idx, t2};
        }
    }

    virtual NodePointer merge(NodePointer root1_idx, NodePointer root2_idx) {
        if (root2_idx == 0) { return root1_idx; }
        if (root1_idx == 0) { return root2_idx; }

        auto &root1 = nodes_[root1_idx];
        auto &root2 = nodes_[root2_idx];
        if (root1.priority > root2.priority) {
            root1.right = merge(root1.right, root2_idx);
            return root1_idx;
        } else {
            root2.left = merge(root1_idx, root2.left);
            return root2_idx;
        }
    }

    virtual void update(NodePointer t) {
        nodes_[t].count = 1 + nodes_[nodes_[t].left].count + nodes_[nodes_[t].right].count;
        nodes_[t].query_result = query_operation(nodes_[nodes_[t].left].query_result, nodes_[nodes_[t].right].query_result);
    }

    struct Node {
        T value;
        T query_result;
        uint64_t priority;
        uint64_t count;
        NodePointer left;
        NodePointer right;
    };
private:
    std::vector<Node> nodes_;
    NodePointer root_{0};
    std::function<T(const T &lhs, const T &rhs)> query_operation = [](auto& lhs, auto& rhs){return lhs+rhs;};
};

/*template<typename T>
class TreapImplicitCompressed: public TreapImplicit<T> {
public:
    template<typename Generator>
    explicit TreapImplicitCompressed(Generator generator, size_t N): compressor_(generator, N) {
        if (N > 0) {
            push_back(compressor_.compress(generator.get_current()));
            for(size_t i=1;i<N;++i){
                push_back(compressor_.compress(generator.next()));
            }
        }
    }

    void insert(const size_t position, const T &value) override {
        TreapImplicit<T>::insert(position, compressor_.compress(value));
    }

protected:
    void update(typename TreapImplicit<T>::NodePointer t) override{
        nodes_[t].count = 1 + nodes_[nodes_[t].left].count + nodes_[nodes_[t].right].count;
        nodes_[t].query_result = query_operation(
                compressor_.decompress(nodes_[nodes_[t].left].query_result),
                compressor_.decompress(nodes_[nodes_[t].right].query_result)
                );
    }

    CoordinatesCompressor<T> compressor_;
};
*/

class MemoryManager{
public:
    MemoryManager(SequenceGenerator<int64_t,int64_t> array_generator, size_t N){
        assert(N>0);
        memory_.push_back(array_generator.get_current());
    }
private:
    TreapImplicit<uint32_t> memory_;
};


// Начало тестов

void test_from_task() {
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

    return 0;
}
