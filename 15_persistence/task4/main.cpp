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
#include <cmath>
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

    SequenceGenerator &operator=(const SequenceGenerator & other){
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
        compressed_coordinates_.erase(unique(compressed_coordinates_.begin(), compressed_coordinates_.end()), compressed_coordinates_.end());
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
    CoordinatesCompressor &operator=(const CoordinatesCompressor & other) = delete;
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

/**
 * @brief Пространство имён, объединяющее персистентные структуры данных.
 */
namespace persistence {
    template<typename T>
    class SegmentCopier {
    public:
        explicit SegmentCopier(SequenceGenerator<int64_t, int64_t> generator, size_t N): compressor_(generator,N) {
            size_ = round_pow2(compressor_.max_compressed() + 1);

            nodes_.reserve(2 * size_ - 1);
            for (size_t i = 0; i < size_ - 1; ++i) {
                nodes_.push_back({i * 2 + 1, i * 2 + 2, 0});
            }
            nodes_.push_back({0, 0, compressor_.compress(generator.get_current())});
            for (size_t i = 1; i < size_; ++i) {
                nodes_.push_back({0, 0, compressor_.compress(generator.next())});
            }
            for (int64_t i = size_ - 2; i >=0; --i) {
                nodes_.[i].sum = nodes_[i*2+1].sum + nodes_[i*2+2].sum;
            }

            roots_.push_back(0);
        }

        ~SegmentCopier() = default;

        SegmentCopier(const SegmentCopier &) = delete;
        SegmentCopier(SegmentCopier &&) = delete;
        SegmentCopier &operator=(const SegmentCopier & other) = delete;
        SegmentCopier&operator=(SegmentCopier &&) = delete;

        void print(const size_t l, const size_t r) const {
            assert(l < size_);
            assert(r < size_);
            assert(l <= r);
            assert(l >= 0);
            std::stack<std::tuple<size_t, size_t, size_t>> dfs;
            dfs.push({roots_.back(), 0, size_ - 1});
            while (!dfs.empty()) {
                auto [node_idx, from, to] = dfs.top();
                dfs.pop();
                if (nodes_[node_idx].left == 0) {
                    std::cout << nodes_[node_idx].sum << " ";
                } else {
                    auto from_left = from;
                    auto to_left = from_left + (to - from) / 2;
                    auto from_right = to_left;
                    auto to_right = to;
                    if (from_right <= r) {
                        dfs.push({nodes_[node_idx].right, from_right, to_right});
                    }
                    if (to_left >= l) {
                        dfs.push({nodes_[node_idx].left, from_left, to_left});
                    }
                }
            }
            std::cout << "\n";
        }

        void copy(const size_t begin_from, const size_t begin_to, const size_t length) {
            auto from_l = begin_from;
            auto from_r = begin_from + length - 1;
            auto to_l = begin_to;
            auto to_r = begin_to + length - 1;

            nodes_.push_back(nodes_[roots_.back()]);
            roots_.push_back(nodes_.size() - 1);

            std::stack<std::tuple<size_t, size_t, size_t>> dfs_old;
            dfs.push({roots_[roots_.size() - 2], 0, size_ - 1});
            while (!dfs.empty()) {
                auto [old_node_idx, new_node_idx, old_from, old_to, new_from, new_to] = dfs.top();
                dfs.pop();

                auto old_length = old_to - old_from + 1;
                auto new_length = new_to - old_to + 1;

                if ()
            }
        }

    private:

        struct Node {
            size_t left{0};
            size_t right{0};
            size_t sum{0};
        };

        /**
         * @brief Определяет наименьшую степень двойки, не меньшую заданного числа.
         * @details Используется знание о представлении чисел с плавающей точкой в памяти.
         * @details Подробнее: https://ru.wikipedia.org/wiki/Число_двойной_точности
         * @param number Число, которое нужно округлить вверх до стпени двойки.
         * @return Число, равное степени двойки. Минимальное, но не меньше, чем number.
         */
        static size_t round_pow2(size_t number) {
            // если убрать "- 1", то "не меньше" в описании нужно заменить на "больше"
            double x = static_cast<double>(number - 1);
            auto answer = reinterpret_cast<unsigned int *>(&x);
            ++answer;
            return static_cast<size_t>(1) << (((*answer & 0x7FF00000) >> 20) - 1022);
        }

        std::vector<size_t> roots_;
        std::vector<Node> nodes_;
        size_t size_{0};
        CoordinatesCompressor<T> compressor_;
    };
}



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
