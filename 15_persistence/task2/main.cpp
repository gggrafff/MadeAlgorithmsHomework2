/*
 * B. Персистентный массив
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дан массив (вернее, первая, начальная его версия).
 * Нужно уметь отвечать на два запроса:
 *      ai[j]=x — создать из i-й версии новую, в которой j-й элемент равен x, а остальные элементы такие же, как в i-й версии.
 *      get ai[j] — сказать, чему равен j-й элемент в i-й версии.
 *
 * Входные данные
 * Количество чисел в массиве N (1≤N≤10^5) и N элементов массива.
 * Далее количество запросов M (1≤M≤10^5) и M запросов.
 * Формат описания запросов можно посмотреть в примере.
 * Если уже существует K версий, новая версия получает номер K+1.
 * И исходные, и новые элементы массива — целые числа от 0 до 10^9.
 * Элементы в массиве нумеруются числами от 1 до N.
 *
 * Выходные данные
 * На каждый запрос типа get вывести соответствующий элемент нужного массива.
 *
 * Пример
 * входные данные
 * 6
 * 1 2 3 4 5 6
 * 11
 * create 1 6 10
 * create 2 5 8
 * create 1 5 30
 * get 1 6
 * get 1 5
 * get 2 6
 * get 2 5
 * get 3 6
 * get 3 5
 * get 4 6
 * get 4 5
 *
 * выходные данные
 * 6
 * 5
 * 10
 * 5
 * 10
 * 8
 * 6
 * 30
 */

/*
 * Подробнее о персистентном массиве:
 * https://neerc.ifmo.ru/wiki/index.php?title=Персистентный_массив
 * https://site.ada.edu.az/~medv/acm/Docs%20e-olimp/Volume%2030/2955.htm
 */

#include <iostream>
#include <vector>
#include <cassert>
#include <array>
#include <optional>
#include <string>

/**
 * @brief Пространство имён, объединяющее персистентные структуры данных.
 */
namespace persistence {
    /**
     * @brief Персистентный массив.
     * @details Версии нумеруются с 0. Версия 0 создаётся коснтруктором.
     * @details Для хранения используется дерево, по структуре похожее на дерево отрезков, но без операций на отрезках.
     * @tparam T Тип данных, хранящихся в массиве.
     */
    template<typename T>
    class Array {
    public:
        /**
         * @brief Конструирует массив и инициализирует его первую версию переданными значениями.
         * @tparam CollectionType Тип данных коллекции, в которой передано начальное заполнение массива.
         * @param initial_value Массив, в котором содержится начальное заполнение первой версии массива.
         */
        template<typename CollectionType>
        explicit Array(const CollectionType &initial_value): SIZE(round_pow2(initial_value.size())) {
            for (size_t i = 0; i < SIZE - 1; ++i) {
                nodes_.push_back({i * 2 + 1, i * 2 + 2, std::nullopt});
            }
            for (size_t i = 0; i < SIZE; ++i) {
                if (i < initial_value.size()) {
                    nodes_.push_back({std::nullopt, std::nullopt, initial_value[i]});
                } else {
                    nodes_.push_back({std::nullopt, std::nullopt, std::nullopt});
                }
            }
            roots_.push_back(0);
        }

        ~Array() = default;

        /**
         * @brief Устанавливает значение элемента в нужной версии массива.
         * @param version Версия массива, которую нужно изменить.
         * @param element_index Индекс изменяемого элемента в массиве.
         * @param value Новое значение элемента.
         * @return Номер новой версии массива.
         */
        size_t set(size_t version, size_t element_index, const T &value) {
            /*
             * Для эффективного выполнения операций нам нужно было сбалансированное дерево
             * (без большой разницы глубин листьев). Поэтому было взято дерево, подобное по структуре дереву отрезков.
             * Корень отвечает за все элементы дерева и создаётся заново при изменении любого элемента.
             * Его левый ребёнок отвечает за первую половину элементов родителя и меняет только при изменении
             * элементов этой половины, а правый ребёнок - за вторую половину элементов родителя.
             */
            assert(version < roots_.size());
            roots_.push_back(nodes_.size());  // Новый корень, новая версия дерева

            // Спукскаемся по дереву от корня к листьям
            size_t current_node = roots_[version];
            size_t border = SIZE / 2;  // граница, левее которой ответственен левый ребёнок, а начиная с неё - правый
            size_t border_difference = border / 2;
            while (!nodes_[current_node].value.has_value()) {
                // Если изменён элемент в левом поддереве, то создаём нового левого ребёнка
                if (element_index < border) {
                    size_t left = nodes_.size() + 1;
                    auto right = nodes_[current_node].right.value();  // правого ребнка не меняем, ссылаемся на имеющегося
                    nodes_.push_back({left, right, std::nullopt});
                    border -= border_difference;
                    current_node = nodes_[current_node].left.value();
                } else { // Если изменён элемент в правом поддереве, то создаём нового правого ребёнка
                    auto left = nodes_[current_node].left.value();  // левого ребнка не меняем, ссылаемся на имеющегося
                    size_t right = nodes_.size() + 1;
                    nodes_.push_back({left, right, std::nullopt});
                    border += border_difference;
                    current_node = nodes_[current_node].right.value();
                }
                border_difference /= 2;
            }
            nodes_.push_back({std::nullopt, std::nullopt, value});
            return roots_.size() - 1;
        }

        /**
         * @brief Получить значение элемента массива.
         * @param version Версия массива.
         * @param element_index Индекс элемента.
         * @return Значение элемента в указанной версии массива.
         */
        [[nodiscard]] const T &get(size_t version, size_t element_index) const {
            assert(version < roots_.size());
            // Спукскаемся по дереву от корня к листьям
            size_t current_node = roots_[version];
            size_t border = SIZE / 2;  // граница, левее которой ответственен левый ребёнок, а начиная с неё - правый
            size_t border_difference = border / 2;
            while (!nodes_[current_node].value.has_value()) {
                if (element_index < border) {
                    border -= border_difference;
                    current_node = nodes_[current_node].left.value();
                } else {
                    border += border_difference;
                    current_node = nodes_[current_node].right.value();
                }
                border_difference /= 2;
            }
            assert(nodes_[current_node].value.has_value());
            return nodes_[current_node].value.value();
        }

    private:
        /**
         * @brief Структура, описывающая узел дерева. Листья хранят значение, другие узлы - просто поддерживают связи.
         */
        struct Node {
            const std::optional<size_t> left{std::nullopt};
            const std::optional<size_t> right{std::nullopt};
            const std::optional<T> value{std::nullopt};
        };

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

        /*
         * Можно было бы сделать дерево на сырых указателях. Скорее всего, оно работало бы быстрее.
         * Но такое персистентное дерево на сырых указателях не очевидно, как удалить.
         * Наличие нескольких родителей у одного узла создаст сложности при очистке.
         * Поэтому было решено хранить узлы в массиве, а ссылки представить индексами.
         */
        std::vector<size_t> roots_;
        std::vector<Node> nodes_;
        const size_t SIZE{0};
    };
}

// Начало тестов

void test_from_task() {
    std::vector<uint32_t> initial{1, 2, 3, 4, 5, 6};
    persistence::Array<uint32_t> array(initial);
    array.set(0, 5, 10);
    array.set(1, 4, 8);
    array.set(0, 4, 30);
    assert(array.get(0, 5) == 6);
    assert(array.get(0, 4) == 5);
    assert(array.get(1, 5) == 10);
    assert(array.get(1, 4) == 5);
    assert(array.get(2, 5) == 10);
    assert(array.get(2, 4) == 8);
    assert(array.get(3, 5) == 6);
    assert(array.get(3, 4) == 30);
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
    size_t N{0};
    std::cin >> N;
    std::vector<uint32_t> initial(N);
    for (size_t i = 0; i < N; ++i) {
        std::cin >> initial[i];
    }
    persistence::Array<uint32_t> array(initial);
    size_t M{0};
    std::cin >> M;
    std::string operation;
    for (size_t i = 0; i < M; ++i) {
        std::cin >> operation;
        size_t version{0}, element_index{0};
        std::cin >> version >> element_index;
        if (operation == "create") {
            uint32_t value{0};
            std::cin >> value;
            array.set(version - 1, element_index - 1, value);
        } else {
            std::cout << array.get(version - 1, element_index - 1) << "\n";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return 0;
}
