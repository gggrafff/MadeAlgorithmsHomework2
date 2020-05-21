/*
 * B. Задача о назначениях
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дана целочисленная матрица C размера n×n. Требуется выбрать n ячеек так, чтобы в каждой строке и каждом столбце была
 * выбрана ровно одна ячейка, а сумма значений в выбранных ячейках была минимальна.
 *
 * Входные данные
 * Первая строка входного файла содержит n (2≤n≤300).
 * Каждая из последующих n строк содержит по n чисел: Cij
 * Все значения во входном файле неотрицательны и не превосходят 10^6.
 *
 * Выходные данные
 * В первую строку выходного файла выведите одно число — искомая минимизируемая величина.
 * Далее выведите n строк по два числа в каждой — номер строки и столбца клетки, участвующей в оптимальном назначении.
 * Пары чисел можно выводить в произвольном порядке.
 *
 * Пример
 * Входные данные
 * 3
 * 3 2 1
 * 1 3 2
 * 2 1 3
 * Выходные данные
 * 3
 * 2 1
 * 3 2
 * 1 3
 */


#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>
#include <optional>
#include <queue>
#include <random>
#include <chrono>
#include <map>

/**
 * Структура, описывающая сеть.
 */
class FlowNetwork {
public:
    FlowNetwork() = default;

    ~FlowNetwork() = default;

    /**
     * @brief Добавляет ориентированное ребро в сеть.
     * @details Добавляет два ориентированных ребра, направленных в разные стороны (обратное нулевой ёмкости).
     * @param from Индекс одной из вершин, соединённых ребром.
     * @param to Индекс второй из вершин, соединённых ребром.
     * @param capacity Пропускная способность ребра.
     */
    void add_directed_edge(size_t from, size_t to, int64_t capacity, int64_t cost, int64_t index = 0) {
        if (from == to) {
            return;
        }
        if (capacity == 0) {
            return;
        }
        if (index == 0) {
            last_index_edges += 1;
            index = last_index_edges;
        }
        nodes[from].edges.push_back({to, 0, capacity, cost, 0, index});
        nodes[to].edges.push_back({from, 0, 0, -cost, 0, -index});
        nodes[from].edges.back().reverse_edge_index = nodes[to].edges.size() - 1;
        nodes[to].edges.back().reverse_edge_index = nodes[from].edges.size() - 1;
    }

    /**
     * @brief Добавляет узел в сеть.
     */
    void add_node() {
        nodes.emplace_back();
    }

    /**
     * @brief Добавляет в сеть заданное количество узлов.
     * @param count Количество добавляемых узлов.
     */
    void add_nodes(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            add_node();
        }
    }

    /**
     * @brief Поиск максимального потока алгоритмом Диница.
     * @param network Сеть, в которой ищем поток.
     * @return Значение максимального потока.
     */
    int64_t find_max_flow_dinic() {
        int64_t maxFlow = 0;
        while (measure_depth_vertices()) {  // пересчитываем d[i], заодно проверяем достижима ли t из s
            for (auto &node: this->nodes) {
                node.p = 0;  // заполняем p нулями
            }
            auto flow = find_block_flow(this->source_index, std::numeric_limits<int64_t>::max());
            while (flow != 0) {
                maxFlow += flow;
                assert(flow >= 0);
                flow = find_block_flow(this->source_index, std::numeric_limits<int64_t>::max());
            }
        }

        // просто проверка корректности, надеюсь, компилятор это соптимизирует в release сборке
        for (auto &node: this->nodes) {
            for (auto &edge: node.edges) {
                assert(edge.flow == -this->nodes[edge.to].edges[edge.reverse_edge_index].flow);
            }
        }

        return maxFlow;
    }

    /**
     * @brief Понижает стоимость найденного ранее потока.
     * @details Нужно найти любой максимальный поток, после чего избавиться от всех циклов отрицательной стоимости в остаточном графе.
     * @details Чтобы избавиться от цикла, надо пустить по нему максимально возможный поток.
     * @details Циклы ищутся алгоритмом Беллмана - Форда.
     * @return Минимальная стоимость максимального потока.
     */
    int64_t reduce_cost_max_flow() {
        while(true) {
            // Беллмана-Форда алгоритм в остаточной сети. Методом динамического программирования.
            const int64_t inf = std::numeric_limits<int64_t>::max();
            /**
             * Искать отрицательные циклы с помощью Б-Ф нужно немного иначе, по сравнению с обычным поиском кратчайшего пути.
             * Если запустить как обычно из стартовой вершины, Б-Ф найдет только отрицательный цикл, достижимый из стартовой.
             *
             * Для того, чтобы это исправить, обычно делают так:
             * добавляют фиктивную вершину, из которой есть ребра веса 0 во все остальные,
             * и потом запускают алгоритм поиска кратчайших путей.
             *
             * Для реализации это просто означает,
             * что мы начинаем не с массива расстояний [0, +inf, +inf, ..., +inf], а с [0, 0, 0, ..., 0].
             */
            std::vector<int64_t> distance(nodes.size(), 0);  // расстояние от истока до вершины i
            std::vector<std::pair<size_t, size_t>> parents(nodes.size(),
                                                           {0,
                                                            0});  // запомним родителей и индексы рёбер для поиска цикла
            std::optional<size_t> last_node_index = std::nullopt;  // запомним номер вершины, в которой произошла релаксация на последней фазе

            for (size_t i = 0; i < nodes.size(); ++i) {
                /**
                 * Часто ответ находится уже за несколько фаз, а оставшиеся фазы никакой полезной работы не происходит,
                 * лишь впустую просматриваются все рёбра.
                 * Поэтому будем хранить флаг того, изменилось что-то на текущей фазе или нет,
                 * и если на какой-то фазе ничего не произошло, то алгоритм можно останавливать.
                 */
                bool changed = false;
                last_node_index = std::nullopt;

                for (size_t ni = 0; ni < nodes.size(); ++ni) {
                    for (size_t ei = 0; ei < nodes[ni].edges.size(); ++ei) {
                        auto &edge = nodes[ni].edges[ei];
                        if (edge.capacity - edge.flow > 0) {
                            if (distance[edge.to] > distance[ni] + edge.cost) {
                                /**
                                 * При наличии отрицательного цикла за n итераций алгоритма расстояния могли
                                 * уйти далеко в минус (по всей видимости, до отрицательных чисел порядка -2^n).
                                 * Поэтому напишем так:
                                 */
                                distance[edge.to] = std::max(-inf, distance[ni] + edge.cost);
                                parents[edge.to] = {ni, ei};
                                changed = true;
                                last_node_index = edge.to;
                            }
                        }
                    }
                }
                if (!changed) {
                    break;
                }
            }

            if (!last_node_index.has_value()) {
                // Считаем стоимость
                return compute_cost_flow();
            }

            // Найдём отрицательный цикл в остаточной сети и поток, который можно по нему запустить.
            /**
             * Чтобы найти вершину, которая лежит на цикле, можно |V|−1 раз пройти назад по предкам из вершины v.
             * Так как наибольшая длина пути в графе из |V| вершин равна |V|−1,
             * то полученная вершина u будет гарантированно лежать на отрицательном цикле.
             */
            size_t in_cycle = last_node_index.value();
            for (int i = 0; i < nodes.size() - 1; ++i) {
                in_cycle = parents[in_cycle].first;
            }

            /**
             * Зная, что вершина u лежит на цикле отрицательного веса,
             * можно восстанавливать путь по сохраненным вершинам до тех пор, пока не встретится та же вершина u.
             * Это обязательно произойдет, так как в цикле отрицательного веса релаксации происходят по кругу.
             */
            int64_t additional_flow = inf;
            for (size_t current = in_cycle;
                 ((current != in_cycle) || (additional_flow == inf));
                 current = parents[current].first) {
                auto &edge = nodes[parents[current].first].edges[parents[current].second];
                additional_flow = std::min(additional_flow, edge.capacity - edge.flow);
            }

            assert(additional_flow != 0);

            // Запустим по циклу дополнительный поток
            bool is_begin = true;
            for (size_t current = in_cycle; ((current != in_cycle) || is_begin); current = parents[current].first) {
                auto &edge = nodes[parents[current].first].edges[parents[current].second];
                auto &reverse_edge = nodes[edge.to].edges[edge.reverse_edge_index];
                edge.flow += additional_flow;
                reverse_edge.flow -= additional_flow;
                is_begin = false;
            }

            // Продолжим удешевлять поток
        }
    }

    /**
     * @brief Ищет максимальный поток минимальной стоимости.
     * @details Модификация алгоритма Форда—Фалкерсона.
     * @details На каждом шаге выбирается увеличивающий путь минимальной цены.
     * @details Для выбора пути используется алгоритм Беллмана-Форда.
     * @return Минимальная стоимость максимального потока.
     */
    int64_t find_max_flow_min_cost_bf() {
        while(true) {
            for (auto &node: nodes) {
                node.visited = false;
            }
            if (!find_path_residual(source_index, sink_index)) {
                // Считаем стоимость
                return compute_cost_flow();
            }

            // Беллмана-Форда алгоритм. Методом динамического программирования.
            const int64_t inf = std::numeric_limits<int64_t>::max();

            std::vector<int64_t> distance(nodes.size(), inf);  // расстояние от истока до вершины i
            distance[source_index] = 0;
            // запомним родителей и индексы рёбер для поиска пути
            std::vector<std::pair<size_t, size_t>> parents(nodes.size(), {0, 0});

            for (size_t i = 0; i < nodes.size() - 1; ++i) {
                /**
                 * Часто ответ находится уже за несколько фаз, а оставшиеся фазы никакой полезной работы не происходит,
                 * лишь впустую просматриваются все рёбра.
                 * Поэтому будем хранить флаг того, изменилось что-то на текущей фазе или нет,
                 * и если на какой-то фазе ничего не произошло, то алгоритм можно останавливать.
                 */
                bool changed = false;

                for (size_t ni = 0; ni < nodes.size(); ++ni) {
                    for (size_t ei = 0; ei < nodes[ni].edges.size(); ++ei) {
                        auto &edge = nodes[ni].edges[ei];
                        if ((distance[ni] < inf) && (edge.capacity - edge.flow > 0)) {
                            if (distance[edge.to] > distance[ni] + edge.cost) {
                                distance[edge.to] = distance[ni] + edge.cost;
                                parents[edge.to] = {ni, ei};
                                changed = true;
                            }
                        }
                    }
                }
                if (!changed) {
                    break;
                }
            }

            assert (distance[sink_index] != inf);

            // Найдём путь и поток, который можно по нему запустить.
            // Запустим по пути дополнительный поток
            add_flow(distance, parents);

            // Продолжим искать поток
        }
    }

    /**
     * @brief Ищет максимальный поток минимальной стоимости.
     * @details Модификация алгоритма Форда—Фалкерсона.
     * @details На каждом шаге выбирается увеличивающий путь минимальной цены.
     * @details Для выбора пути используется алгоритм Дейкстры. Подходит только для случая неотрицательных весов рёбер.
     * @return Минимальная стоимость максимального потока.
     */
    int64_t find_max_flow_min_cost_dijkstra_johnson() {
        while (true) {
            for (auto &node: nodes) {
                node.visited = false;
            }
            if (!find_path_residual(source_index, sink_index)) {
                // Считаем стоимость
                return compute_cost_flow();
            }

            // Дейкстры алгоритм.
            const int64_t inf = std::numeric_limits<int64_t>::max();

            std::vector<int64_t> distance(nodes.size(), inf);  // расстояние от истока до вершины i
            distance[source_index] = 0;
            // запомним родителей и индексы рёбер для поиска пути
            std::vector<std::pair<size_t, size_t>> parents(nodes.size(), {0, 0});
            for (auto &node: nodes) {
                node.visited = false;
            }

            // Выполняем цикл до тех пор, пока будет получаться находить не посещённые, но достижимые вершины
            while(true){
                std::optional<size_t> v = std::nullopt;
                for (size_t j = 0; j < nodes.size(); ++j) {
                    // найдём вершину с минимальным расстоянием
                    if (!nodes[j].visited && (!v.has_value() || (distance[j] < distance[v.value()]))) {
                        v = j;
                    }
                }

                if (!v.has_value() || distance[v.value()] == inf) {
                    break;
                }
                nodes[v.value()].visited = true;
                // произведём релаксацию по всем рёбрам, исходящим из v
                for (size_t ei = 0; ei < nodes[v.value()].edges.size(); ++ei) {
                    auto &edge = nodes[v.value()].edges[ei];
                    if (edge.capacity - edge.flow > 0) {
                        if (distance[edge.to] > distance[v.value()] + edge.cost) {
                            distance[edge.to] = distance[v.value()] + edge.cost;

                            /**
                             * Почему-то об этом не пишут в интернете.
                             * Но без этой строки у меня работало не на всех примерах корректно.
                             * Может, думают, что это очевидно.
                             * А может я что-то неправильно делаю.
                             * Но так работает.
                             */
                            nodes[edge.to].visited = false;

                            parents[edge.to] = {v.value(), ei};
                        }
                    }
                }
            }

            assert (distance[sink_index] != inf);

            // Найдём путь и поток, который можно по нему запустить.
            // Запустим по пути дополнительный поток
            add_flow(distance, parents);

            // Продолжим искать поток
        }
    }

    /**
     * @brief Получить индекс источника в сети.
     * @return Индекс источника.
     */
    size_t get_source_index() const {
        return source_index;
    }

    /**
     * Установить индекс источника в сети.
     * @param source_index Индекс источника.
     */
    void set_source_index(size_t source_index) {
        FlowNetwork::source_index = source_index;
    }

    /**
     * @brief Получить индекс стока в сети.
     * @return Индекс стока.
     */
    size_t get_sink_index() const {
        return sink_index;
    }

    /**
     * Установить индекс стока в сети.
     * @param source_index Индекс стока.
     */
    void set_sink_index(size_t sink_index) {
        FlowNetwork::sink_index = sink_index;
    }

    /**
     * @brief Находит индекс какой-то вершины, в которую идёт поток из указанной.
     * @details Игнорирует исток и сток.
     * @param from Откуда ищем поток.
     * @return Индекс вершины, в которую есть поток из указанной.
     */
    size_t find_flow(size_t from) {
        for (auto &edge: nodes[from].edges) {
            if ((edge.flow != 0) && (edge.to != source_index) && (edge.to != sink_index)) {
                return edge.to;
            }
        }
    }

private:
    /**
     * @brief Структура, описывающая ребро сети.
     */
    struct Edge {
        size_t to{0};  // куда ведёт ребро
        int64_t reverse_edge_index{-1};  // индекс обратного ребра в другой вершине
        int64_t capacity{0};  // пропускная способность ребра
        int64_t cost{0};  // стоимость потока по ребру
        int64_t flow{0};  // поток через ребро
        int64_t index{0};  // индекс ребра
    };

    /**
     * Структура, описывающая узел (вершину) сети.
     */
    struct NodeFlowNetwork {
        size_t depth{std::numeric_limits<size_t>::max()};  // удалённость от истока
        size_t p{0};  // индекс первого неудалённого ребра, идущего из узла
        std::vector<Edge> edges;  // массив рёбер, инцидентных вершине
        bool visited{false};  // пометка о посещении при обходе
    };


    /**
     * @brief Поиском в ширину определяет удаление вершин от истока.
     * @details Часть алгоритма Диница.
     * @details Вместо построения слоистой сети просто измерим удаление вершин от истока и учтём их при дальнейшем обходе.
     * @param network Сеть.
     * @return true, если сток достижим из истока, иначе - false.
     */
    bool measure_depth_vertices() {
        //заполняем массив d значениями, равными бесконечности
        for (auto &node: this->nodes) {
            node.depth = std::numeric_limits<size_t>::max();
        }

        this->nodes[this->source_index].depth = 0;

        std::queue<size_t> Q;
        Q.push(this->source_index);
        while (!Q.empty()) {
            auto u = Q.front();
            Q.pop();
            for (auto &edge: this->nodes[u].edges) {
                if ((edge.flow < edge.capacity) && (this->nodes[edge.to].depth == std::numeric_limits<size_t>::max())) {
                    this->nodes[edge.to].depth = this->nodes[u].depth + 1;
                    Q.push(edge.to);
                }
            }
        }
        return (this->nodes[this->sink_index].depth != std::numeric_limits<size_t>::max());
    }

    /**
     * @brief Поиск блокирующего потока
     * @details Часть алгоритма Диница.
     * @param network Сеть.
     * @param u Номер вершины, из которой производим поиск.
     * @param minC Минимальная пропускная способность дополняющей сети на пройденном пути.
     * @return Сколько удалось пропустить по этому пути.
     */
    int64_t find_block_flow(size_t u, int64_t minC) {
        assert(minC >= 0);
        if ((u == this->sink_index) || (minC == 0)) {
            return minC;
        }
        for (auto ei = this->nodes[u].p; ei < this->nodes[u].edges.size(); ++ei) {
            auto &edge = this->nodes[u].edges[ei];

            if (this->nodes[edge.to].depth == this->nodes[u].depth + 1) {
                // это условие эквивалентно поиску во вспомогательной слоистой сети
                auto delta = find_block_flow(edge.to, std::min(minC, edge.capacity - edge.flow));
                assert(delta >= 0);
                if (delta != 0) {
                    edge.flow += delta; // насыщаем рёбра по пути
                    assert(edge.flow <= edge.capacity);
                    assert(edge.flow >= -edge.capacity);
                    auto &reverse_edge = this->nodes[edge.to].edges[edge.reverse_edge_index];
                    assert(reverse_edge.reverse_edge_index == ei);
                    reverse_edge.flow -= delta;
                    return delta;
                }
            }
            this->nodes[u].p++;
        }
        return 0;
    }

    /**
     * @brief Подсчитать количество ориентированных рёбер в сети.
     * @return Количество ориентированных рёбер в сети.
     */
    size_t compute_count_directed_edges() {
        size_t result = 0;
        for (auto &node: nodes) {
            result += node.edges.size();
        }
        return result;
    }

    /**
     * @brief Вычисление стоимости потока.
     * @return Стоимость потока.
     */
    int64_t compute_cost_flow() {
        int64_t result = 0;
        for (auto &node: nodes) {
            for (auto &edge: node.edges) {
                if ((edge.capacity > 0) && edge.flow != 0) {
                    result += edge.cost * edge.flow;
                }
            }
        }
        return result;
    }

    /**
     * @brief Поиск пути между вершинами.
     * @details DFS.
     * @details Перед вызовом у всех вершин поле visited нужно установить в false.
     * @param from Откуда ищем путь.
     * @param to Куда ищем путь.
     * @return true, если путь есть.
     */
    bool find_path_residual(size_t from, size_t to) {
        nodes[from].visited = true;
        if (from == to) {
            return true;
        }
        for (auto &edge: nodes[from].edges) {
            if (!nodes[edge.to].visited) {
                if (edge.capacity - edge.flow > 0) {
                    if (find_path_residual(edge.to, to)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /**
     * @brief Добавляет поток вдоль найденного пути от истока к стоку.
     * @param distance Массив расстояний от истока до остальных вершин.
     * @param parents Массив родителей при поиске расстояний для восстановления пути.
     */
    void add_flow(const std::vector<int64_t>& distance, const std::vector<std::pair<size_t, size_t>>& parents) {
        const int64_t inf = std::numeric_limits<int64_t>::max();

        assert (distance[sink_index] != inf);

        // Найдём путь и поток, который можно по нему запустить.
        int64_t additional_flow = inf;
        for (size_t current = sink_index; current != source_index; current = parents[current].first) {
            auto &edge = nodes[parents[current].first].edges[parents[current].second];
            additional_flow = std::min(additional_flow, edge.capacity - edge.flow);
        }

        assert(additional_flow != 0);

        // Запустим по пути дополнительный поток
        for (size_t current = sink_index; current != source_index; current = parents[current].first) {
            auto &edge = nodes[parents[current].first].edges[parents[current].second];
            auto &reverse_edge = nodes[edge.to].edges[edge.reverse_edge_index];
            edge.flow += additional_flow;
            reverse_edge.flow -= additional_flow;
        }
    }


    std::vector<NodeFlowNetwork> nodes;  // массив вершин
    size_t source_index{0};  // индекс вершины, являющейся истоком
    size_t sink_index{0};  // индекс вершины, являющейся стоком
    int64_t last_index_edges{0};  // индекс последнего добавленного ребра (см. Edge::index)
};

/**
 * @brief Структура, в которую сохраним ответ на задачу.
 */
struct Schedule {
    bool operator==(const Schedule &rhs) const {
        return total_cost == rhs.total_cost &&
               assignments == rhs.assignments;
    }

    bool operator!=(const Schedule &rhs) const {
        return !(rhs == *this);
    }

    int64_t total_cost{0};
    std::vector<std::pair<size_t, size_t>> assignments;  // номер работника, номер задачи
};

/**
 * Класс-обёртка, помогающий свести задачу о назначениях к задаче поиска макс. потока мин.стоимости.
 */
class TaskScheduler {
public:
    /**
     * @brief Создаёт сеть нужной конфигурации для сведения задачи.
     * @param count_tasks_and_workers Количество работников и количество задач.
     */
    explicit TaskScheduler(size_t count_tasks_and_workers) : count_tasks_and_workers_(count_tasks_and_workers) {
        net_.add_nodes(count_tasks_and_workers_ * 2 + 2);
        net_.set_source_index(0);
        net_.set_sink_index(1);
        for (size_t i = 0; i < count_tasks_and_workers_; ++i) {
            net_.add_directed_edge(net_.get_source_index(), i + 2, 1, 0);
        }
        for (size_t i = 0; i < count_tasks_and_workers_; ++i) {
            net_.add_directed_edge(i + 2 + count_tasks_and_workers_, net_.get_sink_index(), 1, 0);
        }
    }

    ~TaskScheduler() = default;

    /**
     * @brief Установить стоимость работы для работника.
     * @param worker_index Индекс работника.
     * @param task_index Индекс задачи.
     * @param cost Стоимость выполнения задачи работником.
     */
    void add_cost_work(size_t worker_index, size_t task_index, int64_t cost) {
        net_.add_directed_edge(2 + worker_index, 2 + count_tasks_and_workers_ + task_index, 1, cost);
    }

    /**
     * @brief Решает задачу о назначениях с использованием алгоритма Дейкстры.
     * @details Для неотрицательных стоимостей.
     * @return Распределение задач между работниками и общая стоимость в структуре Schedule.
     */
    Schedule find_minimum_cost_schedule_dijkstra() {
        Schedule result;
        result.total_cost = net_.find_max_flow_min_cost_dijkstra_johnson();
        for (size_t i = 0; i < count_tasks_and_workers_; ++i) {
            result.assignments.emplace_back(i, get_schedule_for_worker(i));
        }
        return result;
    }

    /**
     * @brief Решает задачу о назначениях с использованием алгоритма Беллмана-Форда.
     * @details Могут быть рёбра с отрицательным весом, но без циклов отрицательной стоимости.
     * @return Распределение задач между работниками и общая стоимость в структуре Schedule.
     */
    Schedule find_minimum_cost_schedule_bf() {
        Schedule result;
        result.total_cost = net_.find_max_flow_min_cost_bf();
        for (size_t i = 0; i < count_tasks_and_workers_; ++i) {
            result.assignments.emplace_back(i, get_schedule_for_worker(i));
        }
        return result;
    }

    /**
     * @brief Решает задачу о назначениях.
     * @details Ищет максимальный поток алгоритмом Диница.
     * @details Минимизирует стоимость потока, устраняя отрицательные циклы в остаточной сети.
     * @details Для поиска циклов в остаточной сети использует алгоритм Беллмана-Форда.
     * @return Распределение задач между работниками и общая стоимость в структуре Schedule.
     */
    Schedule find_minimum_cost_schedule_dinic_bf() {
        net_.find_max_flow_dinic();
        Schedule result;
        result.total_cost = net_.reduce_cost_max_flow();
        for (size_t i = 0; i < count_tasks_and_workers_; ++i) {
            result.assignments.emplace_back(i, get_schedule_for_worker(i));
        }
        return result;
    }

private:
    /**
     * @brief Узнать, на какую задачу назначен работник.
     * @param worker_index Индекс работника.
     * @return Индекс задачи.
     */
    size_t get_schedule_for_worker(size_t worker_index) {
        auto task_index = net_.find_flow(2 + worker_index) - (2 + count_tasks_and_workers_);
        return task_index;
    }

    FlowNetwork net_;
    size_t count_tasks_and_workers_{0};
};


// Начало тестов


void test_from_task() {
    size_t N = 3;
    TaskScheduler scheduler(N);

    scheduler.add_cost_work(0, 0, 3);
    scheduler.add_cost_work(0, 1, 2);
    scheduler.add_cost_work(0, 2, 1);

    scheduler.add_cost_work(1, 0, 1);
    scheduler.add_cost_work(1, 1, 3);
    scheduler.add_cost_work(1, 2, 2);

    scheduler.add_cost_work(2, 0, 2);
    scheduler.add_cost_work(2, 1, 1);
    scheduler.add_cost_work(2, 2, 3);

    auto plan = scheduler.find_minimum_cost_schedule_dijkstra();
    assert(plan.total_cost == 3);
    assert((plan.assignments[0] == std::pair<size_t, size_t>(0, 2)));
    assert((plan.assignments[1] == std::pair<size_t, size_t>(1, 0)));
    assert((plan.assignments[2] == std::pair<size_t, size_t>(2, 1)));
}

void test_slow_dijkstra() {
    auto begin = std::chrono::steady_clock::now();
    size_t N = 150;
    TaskScheduler scheduler(N);

    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            scheduler.add_cost_work(i, j, i * j);
        }
    }

    auto plan = scheduler.find_minimum_cost_schedule_dijkstra();
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "The time of slow test for Dijkstra with N = " << N << ": " << elapsed_ms.count() << " ms\n";
}

void test_slow_bf() {
    auto begin = std::chrono::steady_clock::now();
    size_t N = 150;
    TaskScheduler scheduler(N);

    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            scheduler.add_cost_work(i, j, i * j);
        }
    }

    auto plan = scheduler.find_minimum_cost_schedule_bf();
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "The time of slow test for B-F with N = " << N << ": " << elapsed_ms.count() << " ms\n";
}

void test_slow_dinic_bf() {
    auto begin = std::chrono::steady_clock::now();
    size_t N = 150;
    TaskScheduler scheduler(N);

    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            scheduler.add_cost_work(i, j, i * j);
        }
    }

    auto plan = scheduler.find_minimum_cost_schedule_bf();
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "The time of slow test for Dinic + B-F with N = " << N << ": " << elapsed_ms.count() << " ms\n";
}

void test_random_big() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<size_t> small_numbers_generator(0, 10);
    std::uniform_int_distribution<size_t> big_numbers_generator(0, 1000);

    for (auto experimentNo = 0; experimentNo < 100; ++experimentNo) {
        size_t N = small_numbers_generator(rd) + 2;

        TaskScheduler scheduler1(N);
        TaskScheduler scheduler2(N);
        TaskScheduler scheduler3(N);

        std::map<std::pair<size_t, size_t>, int64_t> costs;

        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                auto cost = big_numbers_generator(rd);
                scheduler1.add_cost_work(i, j, cost);
                scheduler2.add_cost_work(i, j, cost);
                scheduler3.add_cost_work(i, j, cost);
                costs[std::pair<size_t, size_t>(i, j)] = cost;
            }
        }

        auto plan1 = scheduler1.find_minimum_cost_schedule_dijkstra();
        auto plan2 = scheduler2.find_minimum_cost_schedule_bf();
        auto plan3 = scheduler3.find_minimum_cost_schedule_dinic_bf();

        int64_t sum1 = 0;
        for(auto& assignment: plan1.assignments)
        {
            sum1 += costs[assignment];
        }
        assert(sum1 == plan1.total_cost);

        int64_t sum2 = 0;
        for(auto& assignment: plan2.assignments)
        {
            sum2 += costs[assignment];
        }
        assert(sum2 == plan2.total_cost);

        int64_t sum3 = 0;
        for(auto& assignment: plan3.assignments)
        {
            sum3 += costs[assignment];
        }
        assert(sum3 == plan3.total_cost);

        assert(plan2.total_cost == plan3.total_cost);
        assert(plan1.total_cost == plan2.total_cost);
        assert(plan1.total_cost == plan3.total_cost);
    }
}

void test_5x5(){
    size_t N = 5;
    TaskScheduler scheduler1(N);
    TaskScheduler scheduler2(N);
    std::map<std::pair<size_t, size_t>, int64_t> costs {
            {{0, 0}, 3},
            {{0, 1}, 6},
            {{0, 2}, 2},
            {{0, 3}, 3},
            {{0, 4}, 7},

            {{1, 0}, 6},
            {{1, 1}, 4},
            {{1, 2}, 3},
            {{1, 3}, 6},
            {{1, 4}, 5},

            {{2, 0}, 4},
            {{2, 1}, 2},
            {{2, 2}, 4},
            {{2, 3}, 0},
            {{2, 4}, 6},

            {{3, 0}, 1},
            {{3, 1}, 4},
            {{3, 2}, 2},
            {{3, 3}, 3},
            {{3, 4}, 2},

            {{4, 0}, 2},
            {{4, 1}, 3},
            {{4, 2}, 6},
            {{4, 3}, 3},
            {{4, 4}, 5},
    };
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            scheduler1.add_cost_work(i, j, costs[std::pair<size_t, size_t>(i, j)]);
            scheduler2.add_cost_work(i, j, costs[std::pair<size_t, size_t>(i, j)]);
        }
    }

    auto plan1 = scheduler1.find_minimum_cost_schedule_bf();
    auto plan2 = scheduler2.find_minimum_cost_schedule_dijkstra();

    assert(plan1.total_cost == plan2.total_cost);
}

void test_3x3(){
    size_t N = 3;
    TaskScheduler scheduler1(N);
    TaskScheduler scheduler2(N);
    std::map<std::pair<size_t, size_t>, int64_t> costs {
            {{0, 0}, 6},
            {{0, 1}, 6},
            {{0, 2}, 9},

            {{1, 0}, 0},
            {{1, 1}, 5},
            {{1, 2}, 2},

            {{2, 0}, 0},
            {{2, 1}, 2},
            {{2, 2}, 7},
    };
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            scheduler1.add_cost_work(i, j, costs[std::pair<size_t, size_t>(i, j)]);
            scheduler2.add_cost_work(i, j, costs[std::pair<size_t, size_t>(i, j)]);
        }
    }

    auto plan1 = scheduler1.find_minimum_cost_schedule_bf();
    auto plan2 = scheduler2.find_minimum_cost_schedule_dijkstra();

    assert(plan1.total_cost == plan2.total_cost);
}

void run_all_tests() {
    test_3x3();
    test_5x5();
    test_random_big();
    test_from_task();
    test_slow_dijkstra();
    test_slow_bf();
    test_slow_dinic_bf();
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
    TaskScheduler scheduler(N);
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            int64_t cost{0};
            std::cin >> cost;
            scheduler.add_cost_work(i, j, cost);
        }
    }

    auto plan = scheduler.find_minimum_cost_schedule_dijkstra();
    std::cout << plan.total_cost << "\n";
    for (auto &assignment: plan.assignments) {
        std::cout << assignment.first + 1 << " " << assignment.second + 1 << "\n";
    }

    return 0;
}