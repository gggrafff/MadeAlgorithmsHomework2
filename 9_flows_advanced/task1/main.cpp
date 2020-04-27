/*
 * A. Максимальный поток минимальной стоимости
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Задан ориентированный граф, каждое ребро которого обладает пропускной способностью и стоимостью.
 * Найдите максимальный поток минимальной стоимости из вершины с номером 1 в вершину с номером n.
 *
 * Входные данные
 * Первая строка входного файла содержит n и m — количество вершин и количество ребер графа (2≤n≤100, 0≤m≤1000).
 * Следующие m строк содержат по четыре целых числа числа:
 * номера вершин, которые соединяет соответствующее ребро графа, его пропускную способность и его стоимость.
 * Пропускные способности и стоимости не превосходят 10^5.
 *
 * Выходные данные
 * В выходной файл выведите одно число — цену максимального потока минимальной стоимости
 * из вершины с номером 1 в вершину с номером n. Ответ не превышает 2^63 − 1.
 *
 * Гарантируется, что в графе нет циклов отрицательной стоимости.
 *
 * Пример
 * Входные данные
 * 4 5
 * 1 2 1 2
 * 1 3 2 2
 * 3 2 1 1
 * 2 4 2 1
 * 3 4 2 3
 * Выходные данные
 * 12
 *
 * Примечание
 * В этой задаче достаточно несколько раз пустить Форд-Беллмана...
 */

/**
 * Для поиска потока мин. стоимости, есть 2 подхода:
 * Найти просто макс. поток, а потом отменять отрицательные циклы
 * Строить итеративно, находя кратчайшие пути в остаточной сети с помощью Беллмана-Форда или Дейкстры.
 *
 * Если мы нашли сначала какой-то макс. поток, то надо искать отрицательные циклы.
 * Если же использовать второй подход, то нужно сразу начинать искать кратчайшие пути.
 */

/**
 * В этом файле реализованы оба подхода.
 * Первый:
 *      Реализация: функция find_max_flow_dinic() + функция reduce_cost_max_flow()
 *      Пример использования: функция test_from_task_dinic_bf()
 * Второй:
 *      Реализация: функция find_max_flow_min_cost()
 *      Пример использования: функция test_from_task_bf_iteration()
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


/**
 * Структура, описывающая сеть.
 */
class FlowNetwork {
public:
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
        std::vector<std::pair<size_t, size_t>> parents(nodes.size(), {0, 0});  // запомним родителей и индексы рёбер для поиска цикла
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
        return reduce_cost_max_flow();
    }

    /**
     * @brief Ищет максимальный поток минимальной стоимости.
     * @details Модификация алгоритма Форда—Фалкерсона.
     * @details На каждом шаге выбирается увеличивающий путь минимальной цены.
     * @details Для выбора пути используется алгоритм Беллмана-Форда.
     * @return Минимальная стоимость максимального потока.
     */
    int64_t find_max_flow_min_cost() {
        // Беллмана-Форда алгоритм. Методом динамического программирования.
        const int64_t inf = std::numeric_limits<int64_t>::max();

        std::vector<int64_t> distance(nodes.size(), inf);  // расстояние от истока до вершины i
        distance[source_index] = 0;
        std::vector<std::pair<size_t, size_t>> parents(nodes.size(), {0, 0});  // запомним родителей и индексы рёбер для поиска пути

        for (auto& node: nodes)
        {
            node.visited = false;
        }
        if (!find_path_residual(source_index, sink_index)){
            // Считаем стоимость
            return compute_cost_flow();
        }

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
        int64_t additional_flow = inf;
        for (size_t current = sink_index;current!=source_index; current = parents[current].first) {
            auto &edge = nodes[parents[current].first].edges[parents[current].second];
            additional_flow = std::min(additional_flow, edge.capacity - edge.flow);
        }

        assert(additional_flow != 0);

        // Запустим по пути дополнительный поток
        for (size_t current = sink_index;current!=source_index; current = parents[current].first) {
            auto &edge = nodes[parents[current].first].edges[parents[current].second];
            auto &reverse_edge = nodes[edge.to].edges[edge.reverse_edge_index];
            edge.flow += additional_flow;
            reverse_edge.flow -= additional_flow;
        }

        // Продолжим искать поток
        return find_max_flow_min_cost();
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
        if (from == to)
        {
            return true;
        }
        for (auto &edge: nodes[from].edges) {
            if (!nodes[edge.to].visited) {
                if (edge.capacity - edge.flow > 0){
                    if (find_path_residual(edge.to, to)){
                        return true;
                    }
                }
            }
        }
        return false;
    }


    std::vector<NodeFlowNetwork> nodes;  // массив вершин
    size_t source_index{0};  // индекс вершины, являющейся истоком
    size_t sink_index{0};  // индекс вершины, являющейся стоком
    int64_t last_index_edges{0};  // индекс последнего добавленного ребра (см. Edge::index)
};


// Начало тестов


void test_from_task_dinic_bf() {
    size_t N = 4;
    size_t M = 5;
    FlowNetwork network;
    network.set_source_index(0);
    network.set_sink_index(N - 1);
    for (size_t i = 0; i < N; ++i) {
        network.add_node();
    }
    network.add_directed_edge(0, 1, 1, 2);
    network.add_directed_edge(0, 2, 2, 2);
    network.add_directed_edge(2, 1, 1, 1);
    network.add_directed_edge(1, 3, 2, 1);
    network.add_directed_edge(2, 3, 2, 3);
    auto max_flow = network.find_max_flow_dinic();
    assert(max_flow > 0);
    auto min_cost = network.reduce_cost_max_flow();
    assert(min_cost == 12);
}

void test_from_task_bf_iteration() {
    size_t N = 4;
    size_t M = 5;
    FlowNetwork network;
    network.set_source_index(0);
    network.set_sink_index(N - 1);
    for (size_t i = 0; i < N; ++i) {
        network.add_node();
    }
    network.add_directed_edge(0, 1, 1, 2);
    network.add_directed_edge(0, 2, 2, 2);
    network.add_directed_edge(2, 1, 1, 1);
    network.add_directed_edge(1, 3, 2, 1);
    network.add_directed_edge(2, 3, 2, 3);
    auto min_cost = network.find_max_flow_min_cost();
    assert(min_cost == 12);
}

void test_random_sparse() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<size_t> small_numbers_generator(0, 10);
    std::uniform_int_distribution<size_t> big_numbers_generator(0, 1000);

    for (auto experimentNo = 0; experimentNo < 100; ++experimentNo) {
        size_t N = small_numbers_generator(rd) + 2;
        size_t M = 2 * small_numbers_generator(rd) + 1;

        FlowNetwork network1;
        network1.set_source_index(0);
        network1.set_sink_index(N - 1);
        network1.add_nodes(N);

        FlowNetwork network2;
        network2.set_source_index(0);
        network2.set_sink_index(N - 1);
        network2.add_nodes(N);

        std::vector<std::pair<size_t, size_t>> edges_idx;
        for (size_t i = 0; i < M; ++i) {
            size_t f = big_numbers_generator(rd) % N;
            size_t t = big_numbers_generator(rd) % N;
            while (t == f) {
                t = big_numbers_generator(rd) % N;
            }
            int64_t cap = big_numbers_generator(rd);
            int64_t cost = big_numbers_generator(rd);

            network1.add_directed_edge(f, t, cap, cost);
            network2.add_directed_edge(f, t, cap, cost);
        }
        auto max_flow = network1.find_max_flow_dinic();
        if(max_flow > 0){
            auto min_cost1 = network1.reduce_cost_max_flow();
            auto min_cost2 = network2.find_max_flow_min_cost();
            assert(min_cost1 == min_cost2);
        }
    }
}

void test_rake(){
    size_t N = 4;
    size_t M = 5;
    FlowNetwork network;
    network.set_source_index(0);
    network.set_sink_index(N - 1);
    network.add_nodes(N);
    network.add_directed_edge(0, 1, 3, 3);
    network.add_directed_edge(0, 3, 2, 1);
    network.add_directed_edge(0, 2, 3, 3);
    network.add_directed_edge(2, 3, 2, 3);
    network.add_directed_edge(2, 0, 1, 2);
    auto max_flow = network.find_max_flow_dinic();
    assert(max_flow == 4);
    auto min_cost = network.reduce_cost_max_flow();
    assert(min_cost == 14);
}

void test_triangle(){
    size_t N = 3;
    size_t M = 4;
    FlowNetwork network;
    network.set_source_index(0);
    network.set_sink_index(N - 1);
    network.add_nodes(N);
    network.add_directed_edge(0, 1, 2, 2);
    network.add_directed_edge(0, 1, 2, 1);
    network.add_directed_edge(1, 2, 1, 2);
    network.add_directed_edge(2, 0, 2, 2);
    auto max_flow = network.find_max_flow_dinic();
    assert(max_flow == 1);
    auto min_cost = network.reduce_cost_max_flow();
    assert(min_cost == 3);
}

void run_all_tests() {
    test_triangle();
    test_rake();
    test_from_task_dinic_bf();
    test_from_task_bf_iteration();
    test_random_sparse();
}

// Конец тестов

int main(int argc, char *argv[]) {
    run_all_tests();
    return 0;

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
    size_t M{0};
    std::cin >> N >> M;
    FlowNetwork network;
    network.set_source_index(0);
    network.set_sink_index(N - 1);
    network.add_nodes(N);
    for (size_t i = 0; i < M; ++i) {
        size_t from{0}, to{0};
        int64_t capacity{0}, cost{0};
        std::cin >> from >> to >> capacity >> cost;
        network.add_directed_edge(from - 1, to - 1, capacity, cost);
    }
    auto max_flow = network.find_max_flow_dinic();
    if (max_flow == 0) {
        std::cout << 0;
    } else {
        auto min_cost = network.reduce_cost_max_flow();
        std::cout << min_cost;
    }
    /*auto min_cost = network.find_max_flow_min_cost();
    std::cout << min_cost;*/

    return 0;
}