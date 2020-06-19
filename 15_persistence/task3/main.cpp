/*
 * K-я порядковая статистика на отрезке
 * ограничение по времени на тест 4.0 с
 * ограничение по памяти на тест 512 МБ
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дан массив из N неотрицательных чисел, строго меньших 10^9.
 * Вам необходимо ответить на несколько запросов о величине k-й порядковой статистики на отрезке [l, r].
 *
 * Входные данные
 * Первая строка содержит число N (1 ≤ N ≤ 450 000) — размер массива.
 * Вторая строка может быть использована для генерации ai — начальных значений элементов массива.
 * Она содержит три числа a1, l и m (0 ≤ a1, l, m < 10^9); для i от 2 до N
 * a[i] = (a[i-1]*l + m) mod 10^9
 * В частности, 0 ≤ ai < 10^9.
 * Третья строка содержит одно целое число B (1 ≤ B ≤ 1000) — количество групп запросов.
 * Следующие B строк описывают одну группу запросов.
 * Каждая группа запросов описывается 10 числами.
 * Первое число G обозначает количество запросов в группе.
 * Далее следуют числа x1, lx и mx, затем y1, ly и my, затем, k1, lk и mk
 * (1 ≤ x1 ≤ y1 ≤ N, 1 ≤ k1 ≤ y1 - x1 + 1, 0 ≤ lx, mx, ly, my, lk, mk < 10^9).
 * Эти числа используются для генерации вспомогательных последовательностей xg и yg,
 * а также параметров запросов ig, jg и kg (1 ≤ g ≤ G)
 * x[g] = (((i[g-1] - 1) * lx + mx) mod N) + 1, (2 ≤ g ≤ G)
 * y[g] = (((i[g-1] - 1) * ly + my) mod N) + 1, (2 ≤ g ≤ G)
 * i[g] = min(x[g], y[g]), (1 ≤ g ≤ G)
 * j[g] = min(x[g], y[g]), (1 ≤ g ≤ G)
 * k[g] = (((k[g-1] - 1) * lk + mk) mod (j[g] - i[g] + 1)) + 1, (2 ≤ g ≤ G)
 * Сгенерированные последовательности описывают запросы, g-й запрос состоит в поиске k[g]-го по величине числа
 * среди элементов отрезка [i[g], j[g]].
 *
 * Суммарное количество запросов не превосходит 600 000.
 * Выходные данные
 * Выведите единственное число — сумму ответов на запросы.
 *
 * Пример
 * входные данные
 * 5
 * 1 1 1
 * 5
 * 1
 * 1 0 0 3 0 0 2 0 0
 * 1
 * 2 0 0 5 0 0 3 0 0
 * 1
 * 1 0 0 5 0 0 5 0 0
 * 1
 * 3 0 0 3 0 0 1 0 0
 * 1
 * 1 0 0 4 0 0 1 0 0
 * выходные данные
 * 15
 */

/*
 * Подробнее о сжатии координат:
 * https://archive.lksh.ru/2018/august/B'/notes/03.pdf
 * Подробнее о порядковых статистиках на отрезке:
 * https://au.cdkrot.me/get/spbau-2020-va/algorithms
 * https://ejudge.lksh.ru/archive/2015/08/Aprime/lection/main.pdf
 * https://codeforces.com/blog/entry/2954
 */

#include <random>
#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <cassert>
#include <array>
#include <optional>
#include <string>
#include <numeric>

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

/**
 * @brief Класс, производящий сжатие координат. Например: 10, 20, 30 -> 0, 1, 2.
 * @tparam T Тип данных сжимаемых коориднат.
 */
template<typename T>
class CoordinatesCompressor {
public:
    CoordinatesCompressor() = default;

    /**
     * @brief Конструирует сжиматель координат, генерируя значения с помощью генератора последовательностей.
     * @param generator Генератор последовательностей.
     * @param N Длина последовательности.
     */
    CoordinatesCompressor(SequenceGenerator<int64_t, int64_t> generator, size_t N) {
        compressed_coordinates_.reserve(N);
        compressed_coordinates_.push_back(generator.get_current());
        for (size_t i = 1; i < N; ++i) {
            compressed_coordinates_.push_back(generator.next());
        }

        sort(compressed_coordinates_.begin(), compressed_coordinates_.end());
        compressed_coordinates_.erase(unique(compressed_coordinates_.begin(), compressed_coordinates_.end()), compressed_coordinates_.end());
    }

    /**
     * @brief Конструирует сжиматель координат из массива сжимаемых координат.
     * @param coordinates Массив сжимаемых координат.
     * @param sorted True, если массив уже отсортирован.
     */
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

    /**
     * @brief Сжать координату.
     * @param coordinate Сжимаемое значение.
     * @return Результат прямого преобразования.
     */
    [[nodiscard]] size_t compress(T coordinate) const {
        return std::distance(compressed_coordinates_.begin(),
                             std::lower_bound(compressed_coordinates_.begin(), compressed_coordinates_.end(),
                                              coordinate));
    }

    /**
     * @brief Разжать координату.
     * @param compressed_coordinate Сжатое значение.
     * @return Результат обратного преобразования.
     */
    [[nodiscard]] T decompress(size_t compressed_coordinate) const {
        return compressed_coordinates_[compressed_coordinate];
    }

    /**
     * @brief Узнать максимальное сжатое значение.
     * @return Максимальное значение сжатой координаты.
     */
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
    /**
     * @brief Класс, подсчитывающий k-ю порядковую статистику на отрезке.
     * @details На основе персистентного ДО, организованного для подсчёта количества разных элементов.
     * @tparam T Тип данных, хранящихся в дереве.
     */
    template<typename T>
    class KthOrderStatistic {
        /*
         * Представим, что на плоскости отмечены точки с координатами (i, a[i]).
         * Тогда ответом на запрос будет k-я по высоте точка в вертикальной полосе,
         * края которой соответствуют границам запроса.
         *
         * Сожмём координаты и построим персистентное дерево отрезков на сумме,
         * добавляя в позицию trans(a[i]) единицу, где trans(a) - значение а в сжатых координатах
         * (должно получиться такое дерево, что итоговая версия будет возвращать кол-во элементов,
         * значения которых после сжатия лежат в диапазоне от l до r).
         * Теперь, за логарифм спускаясь из корня соответствующей версии дерева,
         * мы умеем отвечать на запрос о k-й порядковой статистике на префиксе массива.
         * Так как деревья отрезков для разных префиксов изоморфны, то дерево для вертикальной полосы,
         * соответствующей отрезку запроса [L; R], можно получить, вычитая из значения в каждой вершине
         * дерева версии R значение в соответствующей вершине версии L-1.
         * Так как нам интересны значения только в O(log(n)) вершинах этого дерева,
         * то не станем строить его явно, а просто будем спускаться по обеим версиям и получать нужные
         * значения в несуществующем дереве для полосы от L до R.
         */
    public:
        /**
         * @brief Конструирует дерево, получая элементы из генератора последовательности.
         * @param generator Генератор последовательности.
         * @param N Длина последовательности.
         */
        explicit KthOrderStatistic(SequenceGenerator<int64_t, int64_t> generator, size_t N): compressor_(generator,N) {
            size_ = round_pow2(compressor_.max_compressed() + 1);

            nodes_.reserve(2 * size_ - 1);
            for (size_t i = 0; i < size_ - 1; ++i) {
                nodes_.push_back({i * 2 + 1, i * 2 + 2, 0});
            }
            for (size_t i = 0; i < size_; ++i) {
                nodes_.push_back({0, 0, 0});
            }

            roots_.reserve(N);
            roots_.push_back(0);
            // Подсчёт количества элементов.
            inc(compressor_.compress(generator.get_current()));
            for(size_t i=1;i<N;++i) {
                inc(compressor_.compress(generator.next()));
            }

            assert(nodes_[roots_.back()].sum == N);
        }

        /**
         * @brief Конструирует дерево из массива элементов.
         * @param elements Массив элементов.
         */
        explicit KthOrderStatistic(const std::vector<T>& elements):compressor_(elements) {
            size_ = round_pow2(compressor_.max_compressed() + 1);

            for (size_t i = 0; i < size_ - 1; ++i) {
                nodes_.push_back({i * 2 + 1, i * 2 + 2, 0});
            }
            for (size_t i = 0; i < size_; ++i) {
                nodes_.push_back({0, 0, 0});
            }
            roots_.push_back(0);

            // Подсчёт количества элементов.
            for (const auto &element: elements) {
                inc(compressor_.compress(element));
            }

            assert(nodes_[roots_.back()].sum == elements.size());
        }

        ~KthOrderStatistic() = default;

        KthOrderStatistic(const KthOrderStatistic &) = delete;
        KthOrderStatistic(KthOrderStatistic &&) = delete;
        KthOrderStatistic &operator=(const KthOrderStatistic & other) = delete;
        KthOrderStatistic&operator=(KthOrderStatistic &&) = delete;

        /**
         * @brief Ищет к-ю порядковую статистику на отрезке.
         * @param k Порядок статистики.
         * @param l Левая граница отрезка.
         * @param r Правая граница отрезка.
         * @return Значение статистики.
         */
        [[nodiscard]] T find_statistic(int64_t k, const size_t l, const size_t r) const {
            assert(l < roots_.size());
            assert(r < roots_.size());
            assert(l <= r);
            assert(l > 0);
            size_t current_node_l = roots_[l - 1];
            size_t current_node_r = roots_[r];
            size_t result_index = 0;
            size_t level_size = size_ / 2;
            while (nodes_[current_node_l].left != 0) {
                auto left_sum = nodes_[nodes_[current_node_r].left].sum -
                                nodes_[nodes_[current_node_l].left].sum;
                if (k <= left_sum) {
                    current_node_l = nodes_[current_node_l].left;
                    current_node_r = nodes_[current_node_r].left;
                } else {
                    result_index += level_size;
                    k -= left_sum;
                    current_node_l = nodes_[current_node_l].right;
                    current_node_r = nodes_[current_node_r].right;
                }
                level_size /= 2;
            }
            assert(k <= nodes_[current_node_r].sum - nodes_[current_node_l].sum);
            auto result = compressor_.decompress(result_index);
            return result;
        }

    private:
        struct Node {
            const size_t left{0};
            const size_t right{0};
            const size_t sum{0};
        };

        /**
         * @brief Увеличить на единицу значение элемента в дереве.
         * @param element_index Индекс увеличиваемого элемента.
         */
        void inc(size_t element_index) {
            size_t current_node = roots_.back();
            roots_.push_back(nodes_.size());
            size_t border = size_ / 2;  // граница, левее которой ответственен левый ребёнок, а начиная с неё - правый
            size_t border_difference = border / 2;
            // Спускаемся от корня к листьям
            while (nodes_[current_node].left != 0) {
                auto sum = nodes_[current_node].sum + 1;
                // Если изменён элемент в левом поддереве, то создаём нового левого ребёнка
                if (element_index < border) {
                    auto left = nodes_.size() + 1;
                    auto right = nodes_[current_node].right;  // правого ребнка не меняем, ссылаемся на имеющегося
                    nodes_.push_back({left, right, sum});
                    border -= border_difference;
                    current_node = nodes_[current_node].left;
                } else { // Если изменён элемент в правом поддереве, то создаём нового правого ребёнка
                    auto left = nodes_[current_node].left;  // левого ребнка не меняем, ссылаемся на имеющегося
                    auto right = nodes_.size() + 1;
                    nodes_.push_back({left, right, sum});
                    border += border_difference;
                    current_node = nodes_[current_node].right;
                }
                border_difference /= 2;
            }
            nodes_.push_back({0, 0, nodes_[current_node].sum + 1});
        }

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
    size_t N = 5;
    int64_t a0 = 1;
    int64_t l{1}, m{1};
    SequenceGenerator<int64_t, int64_t> array_generator(l, m, a0, 1e9);
    std::vector<uint32_t> elements;
    elements.push_back(a0);
    for (size_t i = 1; i < N; ++i) {
        elements.push_back(array_generator.next());
    }
    for (const auto &element: elements) {
        std::cout << element << " ";
    }
    std::cout << std::endl;
    persistence::KthOrderStatistic<uint32_t> statistics(elements);
    size_t B = 5;
    int64_t sum = 0;
    {
        size_t G = 1;
        int64_t x0 = 1;
        int64_t lx{0}, mx{0};
        int64_t y0 = 3;
        int64_t ly{0}, my{0};
        int64_t k0 = 2;
        int64_t lk{0}, mk{0};

        auto i0 = std::min(x0, y0);
        auto j0 = std::max(x0, y0);

        auto result = statistics.find_statistic(k0, i0, j0);
        sum += result;
    }
    {
        size_t G = 1;
        int64_t x0 = 2;
        int64_t lx{0}, mx{0};
        int64_t y0 = 5;
        int64_t ly{0}, my{0};
        int64_t k0 = 3;
        int64_t lk{0}, mk{0};

        auto i0 = std::min(x0, y0);
        auto j0 = std::max(x0, y0);

        auto result = statistics.find_statistic(k0, i0, j0);
        sum += result;
    }
    {
        size_t G = 1;
        int64_t x0 = 1;
        int64_t lx{0}, mx{0};
        int64_t y0 = 5;
        int64_t ly{0}, my{0};
        int64_t k0 = 5;
        int64_t lk{0}, mk{0};

        auto i0 = std::min(x0, y0);
        auto j0 = std::max(x0, y0);

        auto result = statistics.find_statistic(k0, i0, j0);
        sum += result;
    }
    {
        size_t G = 1;
        int64_t x0 = 3;
        int64_t lx{0}, mx{0};
        int64_t y0 = 3;
        int64_t ly{0}, my{0};
        int64_t k0 = 1;
        int64_t lk{0}, mk{0};

        auto i0 = std::min(x0, y0);
        auto j0 = std::max(x0, y0);

        auto result = statistics.find_statistic(k0, i0, j0);
        sum += result;
    }
    {
        size_t G = 1;
        int64_t x0 = 1;
        int64_t lx{0}, mx{0};
        int64_t y0 = 4;
        int64_t ly{0}, my{0};
        int64_t k0 = 1;
        int64_t lk{0}, mk{0};

        auto i0 = std::min(x0, y0);
        auto j0 = std::max(x0, y0);

        auto result = statistics.find_statistic(k0, i0, j0);
        sum += result;
    }
    assert(sum == 15);
}

void test_statistics(){
    std::vector<uint32_t> elements{1, 5, 2, 6, 3, 7, 4};
    persistence::KthOrderStatistic<uint32_t> statistics(elements);
    assert(statistics.find_statistic(3, 2, 5) == 5);
    assert(statistics.find_statistic(1, 4, 4) == 6);
    assert(statistics.find_statistic(3, 1, 7) == 3);
}

void run_all_tests() {
    test_from_task();
    test_statistics();
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
    assert(N <= 450000);
    int64_t a0{0};
    int64_t l{0}, m{0};
    std::cin >> a0 >> l >> m;
    SequenceGenerator<int64_t, int64_t> array_generator(l, m, a0, 1e9);
    persistence::KthOrderStatistic<uint32_t> statistics(array_generator, N);
    uint16_t B{0};
    std::cin >> B;
    int64_t sum = 0;
    for (uint16_t group = 0; group < B; ++group) {
        uint32_t G{0};
        std::cin >> G;
        int64_t x{0};
        int64_t lx{0}, mx{0};
        std::cin >> x >> lx >> mx;
        int64_t y{0};
        int64_t ly{0}, my{0};
        std::cin >> y >> ly >> my;
        int64_t k{0};
        int64_t lk{0}, mk{0};
        std::cin >> k >> lk >> mk;

        auto i = std::min(x, y);
        auto j = std::max(x, y);

        sum += statistics.find_statistic(k, i, j);
        for(uint32_t query=1; query < G; ++query){
            x = (((i - 1) * lx + mx) % N) + 1;
            y = (((j - 1) * ly + my) % N) + 1;
            i = std::min(x, y);
            j = std::max(x, y);
            k = (((k - 1) * lk + mk) % (j - i + 1)) + 1;
            sum += statistics.find_statistic(k, i, j);
        }
    }
    std::cout << sum;

    return 0;
}
