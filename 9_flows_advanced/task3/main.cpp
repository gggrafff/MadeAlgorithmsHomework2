/*
 * C. План эвакуации
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * В городе есть муниципальные здания и бомбоубежища, которые были специально построены для эвакуации служащих
 * в случае ядерной войны. Каждое бомбоубежище имеет ограниченную вместительность по количеству людей,
 * которые могут в нем находиться. В идеале все работники из одного муниципального здания должны были бы бежать
 * к ближайшему бомбоубежищу. Однако, в таком случае, некоторые бомбоубежища могли бы переполниться,
 * в то время как остальные остались бы наполовину пустыми.
 *
 * Чтобы разрешить эту проблему Городской Совет разработал специальный план эвакуации. Вместо того,
 * чтобы каждому служащему индивидуально приписать, в какое бомбоубежище он должен бежать, для каждого
 * муниципального здания определили, сколько служащих из него в какое бомбоубежище должны бежать.
 * Задача индивидуального распределения была переложена на внутреннее управление муниципальных зданий.
 *
 * План эвакуации учитывает количество служащих в каждом здании — каждый служащий должен быть учтен в плане и
 * в каждое бомбоубежище может быть направлено количество служащих, не превосходящее вместимости бомбоубежища.
 *
 * Городской Совет заявляет, что их план эвакуации оптимален в том смысле,
 * что суммарное время эвакуации всех служащих города минимально.
 *
 * Мэр города, находящийся в постоянной конфронтации с Городским Советом, не слишком то верит этому заявлению.
 * Поэтому он нанял Вас в качестве независимого эксперта для проверки плана эвакуации. Ваша задача состоит в том,
 * чтобы либо убедиться в оптимальности плана Городского Совета, либо доказать обратное,
 * представив в качестве доказательства другой план эвакуации с меньшим суммарным временем для эвакуации всех служащих.
 *
 * Карта города может быть представлена в виде квадратной сетки. Расположение муниципальных зданий и бомбоубежищ
 * задается парой целых чисел, а время эвакуации из муниципального здания с координатами (Xi, Yi) в бомбоубежище
 * с координатами (Pj, Qj) составляет Dij = |Xi - Pj| + |Yi - Qj| + 1 минут.
 *
 * Входные данные
 * Входной файл содержит описание карты города и плана эвакуации, предложенного Городским Советом.
 * Первая строка входного файла содержит два целых числа N (1 ≤ N ≤ 100) и M (1 ≤ M ≤ 100), разделенных пробелом.
 * N — число муниципальных зданий в городе (все они занумерованы числами от 1 до N),
 * M — число бомбоубежищ (все они занумерованы числами от 1 до M).
 * Последующие N строк содержат описания муниципальных зданий.
 * Каждая строка содержит целые числа Xi, Yi и Bi, разделенные пробелами,
 *      где Xi, Yi (-1000 ≤ Xi, Yi ≤ 1000) — координаты здания, а Bi (1 ≤ Bi ≤ 1000) — число служащих в здании.
 * Описание бомбоубежищ содержится в последующих M строках.
 * Каждая строка содержит целые числа Pj, Qj и Cj, разделенные пробелами,
 *      где Pj, Qj (-1000 ≤ Pj, Qj ≤ 1000) — координаты бомбоубежища, а Cj (1 ≤ Cj ≤ 1000) — вместимость бомбоубежища.
 * В последующих N строках содержится описание плана эвакуации.
 * Каждая строка представляет собой описание плана эвакуации для отдельного здания.
 * План эвакуации из i-го здания состоит из M целых чисел Eij, разделенных пробелами.
 * Eij (0 ≤ Eij ≤ 10000) — количество служащих, которые должны эвакуироваться из i-го здания в j-е бомбоубежище.
 * Гарантируется, что план, заданный во входном файле, корректен.
 *
 * Выходные данные
 * Если план эвакуации Городского Совета оптимален, то выведите одно слово OPTIMAL.
 * В противном случае выведите на первой строке слово SUBOPTIMAL, а в последующих N строках выведите
 * Ваш план эвакуации (более оптимальный) в том же формате, что и во входном файле.
 * Ваш план не обязан быть оптимальным, но должен быть лучше плана Городского Совета.
 *
 * Примеры
 * Входные данные
 * 3 4
 * -3 3 5
 * -2 -2 6
 * 2 2 5
 * -1 1 3
 * 1 1 4
 * -2 -2 7
 * 0 -1 3
 * 3 1 1 0
 * 0 0 6 0
 * 0 3 0 2
 * Выходные данные
 * SUBOPTIMAL
 * 3 0 1 1
 * 0 0 6 0
 * 0 4 0 1
 *
 * Входные данные
 * 3 4
 * -3 3 5
 * -2 -2 6
 * 2 2 5
 * -1 1 3
 * 1 1 4
 * -2 -2 7
 * 0 -1 3
 * 3 0 1 1
 * 0 0 6 0
 * 0 4 0 1
 * Выходные данные
 * OPTIMAL
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
 * Структура описывающая затраты ресурсов каждого работника на каждую задачу.
 */
using ComplicatedSchedule = std::vector<std::vector<int64_t>>;  // работник; его усилия на каждую задачу

class TaskScheduler;

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
    void add_directed_edge(size_t from, size_t to, int64_t capacity, int64_t cost, int64_t flow = 0, int64_t index = 0) {
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
        nodes[from].edges.push_back({to, 0, capacity, cost, flow, index});
        nodes[to].edges.push_back({from, 0, 0, -cost, -flow, -index});
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
    int64_t reduce_cost_max_flow(int64_t iteration_count = 0) {
        while (true) {
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
            // запомним родителей и индексы рёбер для поиска цикла
            std::vector<std::pair<size_t, size_t>> parents(nodes.size(), {0, 0});
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

            // Продолжим удешевлять поток, если не исчерпано количество итераций
            iteration_count -= 1;
            if (iteration_count == 0) {
                return compute_cost_flow();
            }
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
        while (true) {
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
            while (true) {
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
    std::optional<size_t> find_flow(size_t from) {
        for (auto &edge: nodes[from].edges) {
            if ((edge.flow != 0) && (edge.to != source_index) && (edge.to != sink_index)) {
                return edge.to;
            }
        }
        return std::nullopt;
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

    friend class TaskScheduler;

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
    void add_flow(const std::vector<int64_t> &distance, const std::vector<std::pair<size_t, size_t>> &parents) {
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
 * Класс-обёртка, помогающий свести задачу о назначениях к задаче поиска макс. потока мин.стоимости.
 */
class TaskScheduler {
public:
    /**
     * @brief Создаёт сеть нужной конфигурации.
     * @param count_tasks_and_workers Количество работников и количество задач.
     */
    explicit TaskScheduler(size_t count_tasks_and_workers) :
            count_workers_(count_tasks_and_workers),
            count_tasks_(count_tasks_and_workers) {
        net_.add_nodes(count_workers_ * 2 + 2);
        net_.set_source_index(0);
        net_.set_sink_index(1);
        for (size_t i = 0; i < count_workers_; ++i) {
            net_.add_directed_edge(net_.get_source_index(), i + 2, 1, 0);
        }
        for (size_t i = 0; i < count_workers_; ++i) {
            net_.add_directed_edge(i + 2 + count_workers_, net_.get_sink_index(), 1, 0);
        }
    }

    /**
     * @brief Создаёт сеть нужной конфигурации.
     * @param count_workers Количество работников.
     * @param count_tasks Количество задач.
     */
    TaskScheduler(size_t count_workers, size_t count_tasks) :
            count_workers_(count_workers),
            count_tasks_(count_tasks) {
        net_.add_nodes(count_workers_ + count_tasks + 2);
        net_.set_source_index(0);
        net_.set_sink_index(1);
    }

    ~TaskScheduler() = default;

    /**
     * @brief Задать ресурс работника.
     * @details Работник может тратить свой ресурс на разные задачи одновременно.
     * @param worker_index Индекс работника.
     * @param power Ресурс работника.
     */
    void set_worker_power(size_t worker_index, int64_t power) {
        net_.add_directed_edge(net_.get_source_index(), 2 + worker_index, power, 0);
    }

    /**
     * @brief Задать сложность задачи.
     * @details Задача может выполняться разными работниками.
     * @details Если общий затрачиваемый ресурс работников равен сложности, то задача выполнена.
     * @details Если задача выполнена то, больше никто не может к ней присоединиться.
     * @param task_index Индекс задачи.
     * @param difficulty Сложность задачи.
     */
    void set_task_difficulty(size_t task_index, int64_t difficulty) {
        net_.add_directed_edge(2 + count_workers_ + task_index, net_.get_sink_index(), difficulty, 0);
    }

    /**
     * @brief Установить стоимость работы для работника.
     * @param worker_index Индекс работника.
     * @param task_index Индекс задачи.
     * @param cost Стоимость выполнения задачи работником.
     */
    void add_cost_work(size_t worker_index, size_t task_index, int64_t cost) {
        net_.add_directed_edge(
                2 + worker_index,
                2 + count_workers_ + task_index,
                std::numeric_limits<int64_t>::max(),
                cost);
    }

    /**
     * @brief Установить стоимость работы для работника и усилия, затрачиваемые им на работу.
     * @param worker_index Индекс работника.
     * @param task_index Индекс задачи.
     * @param cost Стоимость выполнения задачи работником.
     * @param power_consumption Усилия, затрачиваемые на работу.
     */
    void add_cost_work(size_t worker_index, size_t task_index, int64_t cost, int64_t power_consumption) {
        net_.add_directed_edge(
                2 + worker_index,
                2 + count_workers_ + task_index,
                std::numeric_limits<int64_t>::max(),
                cost,
                power_consumption);

        auto &edge_worker_to_source = net_.nodes[2 + worker_index].edges[0];
        auto &edge_source_to_worker = net_.nodes[net_.get_source_index()].edges[edge_worker_to_source.reverse_edge_index];
        edge_source_to_worker.flow += power_consumption;
        edge_worker_to_source.flow -= power_consumption;

        auto &edge_task_to_sink = net_.nodes[2 + count_workers_ + task_index].edges[0];
        auto &edge_sink_to_task = net_.nodes[net_.get_sink_index()].edges[edge_task_to_sink.reverse_edge_index];
        edge_task_to_sink.flow += power_consumption;
        edge_sink_to_task.flow -= power_consumption;
    }


    /**
     * @brief Решает задачу о назначениях с использованием алгоритма Дейкстры.
     * @details Для неотрицательных стоимостей.
     * @return Распределение задач между работниками и общая стоимость в структуре Schedule.
     */
    Schedule find_minimum_cost_schedule_dijkstra() {
        Schedule result;
        result.total_cost = net_.find_max_flow_min_cost_dijkstra_johnson();
        for (size_t i = 0; i < count_workers_; ++i) {
            auto task_index = get_schedule_for_worker(i);
            if (!task_index.has_value()) {
                continue;
            }
            result.assignments.emplace_back(i, task_index.value());
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
        for (size_t i = 0; i < count_workers_; ++i) {
            auto task_index = get_schedule_for_worker(i);
            if (!task_index.has_value()) {
                continue;
            }
            result.assignments.emplace_back(i, task_index.value());
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
    Schedule find_minimum_cost_schedule_dinic_bf(size_t iterations = 0) {
        net_.find_max_flow_dinic();
        Schedule result;
        result.total_cost = net_.reduce_cost_max_flow(iterations);
        for (size_t i = 0; i < count_workers_; ++i) {
            auto task_index = get_schedule_for_worker(i);
            if (!task_index.has_value()) {
                continue;
            }
            result.assignments.emplace_back(i, task_index.value());
        }
        return result;
    }

    /**
     * @brief Пробует улучшить распределение нагрузки на работников между задачами.
     * @return Если получилось улучшить, то возвращает распределение нагрузки работников по задачам.
     */
    std::optional<ComplicatedSchedule> improve_schedule() {
        auto current_total_cost = net_.compute_cost_flow();
        auto new_total_cost = net_.reduce_cost_max_flow(1);
        assert(new_total_cost <= current_total_cost);
        if (current_total_cost == new_total_cost) {
            return std::nullopt;
        }
        ComplicatedSchedule schedule(count_workers_, std::vector<int64_t>(count_tasks_, 0));
        for (size_t i = 0; i < count_workers_; ++i) {
            auto node_index = 2 + i;
            for (auto &edge : net_.nodes[node_index].edges) {
                if (edge.flow > 0) {
                    auto task_index = edge.to - 2 - count_workers_;
                    schedule[i][task_index] = edge.flow;
                }
            }
        }
        return schedule;
    }

    /**
     * @brief Узнать общую стоимость выполнения задач.
     * @details Используется для тестов.
     * @return Стоимость выполнения задач.
     */
    int64_t compute_total_cost() {
        return net_.compute_cost_flow();
    }

private:
    /**
     * @brief Узнать, на какую задачу назначен работник.
     * @param worker_index Индекс работника.
     * @return Индекс задачи, если задача работнику назначена.
     */
    std::optional<size_t> get_schedule_for_worker(size_t worker_index) {
        auto task_index_internal = net_.find_flow(2 + worker_index);
        if (!task_index_internal.has_value()) {
            return std::nullopt;
        }
        auto task_index = task_index_internal.value() - (2 + count_workers_);
        return task_index;
    }

    FlowNetwork net_;
    size_t count_tasks_{0};
    size_t count_workers_{0};
};

/**
 * @brief Структура, описывающая строение
 */
struct Building {
    int64_t x{0};  // x-координата
    int64_t y{0};  // y-координата
    int64_t size{0};  // количество людей в здании / вместимость убежища
};
using MunicipalBuilding  = Building;
using Shelter = Building;

/**
 * @brief Рассчитать время эвакуации из муниципального здания в убежище.
 * @param municipal Муниципальное здание.
 * @param shelter Убежище.
 * @return Время эвакуации
 */
int64_t calc_evacuation_time(const MunicipalBuilding &municipal, const Shelter &shelter) {
    return std::abs(municipal.x - shelter.x) + std::abs(municipal.y - shelter.y) + 1;
}

// Начало тестов

void test_simple_from_taskB() {
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

void test_simple_slow_dijkstra() {
    auto begin = std::chrono::steady_clock::now();
    size_t N = 70;
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

void test_simple_slow_bf() {
    auto begin = std::chrono::steady_clock::now();
    size_t N = 70;
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

void test_simple_slow_dinic_bf() {
    auto begin = std::chrono::steady_clock::now();
    size_t N = 70;
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

void test_simple_random() {
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
        for (auto &assignment: plan1.assignments) {
            sum1 += costs[assignment];
        }
        assert(sum1 == plan1.total_cost);

        int64_t sum2 = 0;
        for (auto &assignment: plan2.assignments) {
            sum2 += costs[assignment];
        }
        assert(sum2 == plan2.total_cost);

        int64_t sum3 = 0;
        for (auto &assignment: plan3.assignments) {
            sum3 += costs[assignment];
        }
        assert(sum3 == plan3.total_cost);

        assert(plan2.total_cost == plan3.total_cost);
        assert(plan1.total_cost == plan2.total_cost);
        assert(plan1.total_cost == plan3.total_cost);
    }
}

void test_simple_5x5() {
    size_t N = 5;
    TaskScheduler scheduler1(N);
    TaskScheduler scheduler2(N);
    std::map<std::pair<size_t, size_t>, int64_t> costs{
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

void test_simple_3x3() {
    size_t N = 3;
    TaskScheduler scheduler1(N);
    TaskScheduler scheduler2(N);
    std::map<std::pair<size_t, size_t>, int64_t> costs{
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

void test_from_task_1() {
    size_t N = 3;
    size_t M = 4;
    TaskScheduler scheduler(N, M);
    std::vector<MunicipalBuilding> municipals{
            {-3, 3,  5},
            {-2, -2, 6},
            {2,  2,  5},
    };
    std::vector<Shelter> shelters{
            {-1, 1,  3},
            {1,  1,  4},
            {-2, -2, 7},
            {0,  -1, 3},
    };
    for (size_t i = 0; i < municipals.size(); ++i) {
        scheduler.set_worker_power(i, municipals[i].size);
    }
    for (size_t i = 0; i < shelters.size(); ++i) {
        scheduler.set_task_difficulty(i, shelters[i].size);
    }
    ComplicatedSchedule schedule{
            {3, 1, 1, 0},
            {0, 0, 6, 0},
            {0, 3, 0, 2},
    };
    for (size_t i = 0; i < municipals.size(); ++i) {
        for (size_t j = 0; j < shelters.size(); ++j) {
            const auto cost = calc_evacuation_time(municipals[i], shelters[j]);
            scheduler.add_cost_work(i, j, cost, schedule[i][j]);
        }
    }
    auto new_schedule = scheduler.improve_schedule();
    assert(new_schedule.has_value());
    ComplicatedSchedule true_new_schedule{
            {3, 0, 1, 1},
            {0, 0, 6, 0},
            {0, 4, 0, 1},
    };
    assert(new_schedule == true_new_schedule);
}

void test_from_task_2() {
    size_t N = 3;
    size_t M = 4;
    TaskScheduler scheduler(N, M);
    std::vector<MunicipalBuilding> municipals{
            {-3, 3,  5},
            {-2, -2, 6},
            {2,  2,  5},
    };
    std::vector<Shelter> shelters{
            {-1, 1,  3},
            {1,  1,  4},
            {-2, -2, 7},
            {0,  -1, 3},
    };
    for (size_t i = 0; i < municipals.size(); ++i) {
        scheduler.set_worker_power(i, municipals[i].size);
    }
    for (size_t i = 0; i < shelters.size(); ++i) {
        scheduler.set_task_difficulty(i, shelters[i].size);
    }
    ComplicatedSchedule schedule{
            {3, 0, 1, 1},
            {0, 0, 6, 0},
            {0, 4, 0, 1},
    };
    for (size_t i = 0; i < municipals.size(); ++i) {
        for (size_t j = 0; j < shelters.size(); ++j) {
            const auto cost = calc_evacuation_time(municipals[i], shelters[j]);
            scheduler.add_cost_work(i, j, cost, schedule[i][j]);
        }
    }
    auto new_schedule = scheduler.improve_schedule();
    assert(!new_schedule.has_value());;
}

void test_random_optimal() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<size_t> small_numbers_generator(0, 50);
    std::uniform_int_distribution<size_t> big_numbers_generator(0, 1000);

    for (auto experimentNo = 0; experimentNo < 100; ++experimentNo) {
        size_t N = small_numbers_generator(rd) + 1;
        size_t M = small_numbers_generator(rd) + 1;

        TaskScheduler scheduler(N, M);

        std::vector<MunicipalBuilding> municipals;
        for (size_t i = 0; i < N; ++i) {
            int64_t x{0}, y{0}, size{0};
            x = static_cast<int64_t>(big_numbers_generator(rd)) - 500;
            y = static_cast<int64_t>(big_numbers_generator(rd)) - 500;
            size = big_numbers_generator(rd) + 1;
            municipals.push_back({x, y, size});
            scheduler.set_worker_power(i, municipals[i].size);
        }

        std::vector<Shelter> shelters;
        for (size_t i = 0; i < M; ++i) {
            int64_t x{0}, y{0}, size{0};
            x = static_cast<int64_t>(big_numbers_generator(rd)) - 500;
            y = static_cast<int64_t>(big_numbers_generator(rd)) - 500;
            size = big_numbers_generator(rd) + 1;
            shelters.push_back({x, y, size});
            scheduler.set_task_difficulty(i, shelters[i].size);
        }

        for (size_t i = 0; i < municipals.size(); ++i) {
            for (size_t j = 0; j < shelters.size(); ++j) {
                const auto cost = calc_evacuation_time(municipals[i], shelters[j]);
                scheduler.add_cost_work(i, j, cost);
            }
        }

        scheduler.find_minimum_cost_schedule_dijkstra();

        auto result = scheduler.improve_schedule();
        assert(!result.has_value());
    }
}

void test_random_big() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<size_t> small_numbers_generator(0, 100);
    std::uniform_int_distribution<size_t> big_numbers_generator(0, 1000);

    for (auto experimentNo = 0; experimentNo < 100; ++experimentNo) {
        size_t N = small_numbers_generator(rd) + 1;
        size_t M = small_numbers_generator(rd) + 1;

        TaskScheduler scheduler1(N, M);
        TaskScheduler scheduler2(N, M);

        std::vector<MunicipalBuilding> municipals;
        int64_t total_size_municipals = 0;
        for (size_t i = 0; i < N; ++i) {
            int64_t x{0}, y{0}, size{0};
            x = static_cast<int64_t>(big_numbers_generator(rd)) - 500;
            y = static_cast<int64_t>(big_numbers_generator(rd)) - 500;
            size = big_numbers_generator(rd) + 1;
            total_size_municipals += size;
            municipals.push_back({x, y, size});
            scheduler1.set_worker_power(i, municipals[i].size);
            scheduler2.set_worker_power(i, municipals[i].size);
        }

        int64_t total_size_shelters = 0;
        std::vector<Shelter> shelters;
        for (size_t i = 0; i < M - 1; ++i) {
            int64_t x{0}, y{0}, size{0};
            x = static_cast<int64_t>(big_numbers_generator(rd)) - 500;
            y = static_cast<int64_t>(big_numbers_generator(rd)) - 500;
            size = big_numbers_generator(rd) + 1;
            total_size_shelters += size;
            shelters.push_back({x, y, size});
            scheduler1.set_task_difficulty(i, shelters[i].size);
            scheduler2.set_task_difficulty(i, shelters[i].size);
        }
        int64_t x{0}, y{0}, size{0};
        x = static_cast<int64_t>(big_numbers_generator(rd)) - 500;
        y = static_cast<int64_t>(big_numbers_generator(rd)) - 500;
        size = big_numbers_generator(rd) + 1;
        if (total_size_municipals - total_size_shelters > 0) {
            size = total_size_municipals - total_size_shelters;
        }
        total_size_shelters += size;
        shelters.push_back({x, y, size});
        scheduler1.set_task_difficulty(M - 1, shelters[M - 1].size);
        scheduler2.set_task_difficulty(M - 1, shelters[M - 1].size);

        std::map<std::pair<size_t, size_t>, int64_t> costs;
        for (size_t i = 0; i < municipals.size(); ++i) {
            for (size_t j = 0; j < shelters.size(); ++j) {
                const auto cost = calc_evacuation_time(municipals[i], shelters[j]);
                scheduler1.add_cost_work(i, j, cost);
                scheduler2.add_cost_work(i, j, cost);
                costs[std::pair<size_t, size_t>(i, j)] = cost;
            }
        }

        scheduler1.find_minimum_cost_schedule_dijkstra();
        auto min_cost = scheduler1.compute_total_cost();

        scheduler2.find_minimum_cost_schedule_dinic_bf(1);
        auto start_cost = scheduler2.compute_total_cost();
        auto result = scheduler2.improve_schedule();

        if (!result.has_value()) {
            assert(min_cost == start_cost);
        } else {
            auto finish_cost = scheduler2.compute_total_cost();
            assert(finish_cost < start_cost);
            assert(finish_cost >= min_cost);
            int64_t check_finish_cost = 0;
            for (size_t i = 0; i < N; ++i) {
                auto sum = std::accumulate(result.value()[i].begin(), result.value()[i].end(), 0);
                assert(sum == municipals[i].size);
                for (size_t j = 0; j < M; ++j) {
                    check_finish_cost += result.value()[i][j] * costs[{i, j}];
                    assert(result.value()[i][j] >= 0);
                }
            }
            assert(finish_cost == check_finish_cost);
        }
    }
}

void run_all_tests() {
    test_from_task_1();
    test_from_task_2();
    test_simple_3x3();
    test_simple_5x5();
    test_simple_random();
    test_simple_from_taskB();
    test_simple_slow_dijkstra();
    test_simple_slow_bf();
    test_simple_slow_dinic_bf();
    test_random_optimal();
    test_random_big();
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
     * Для решения задачи сведем её к задаче о назначении следующего вида:
     * Есть рабочие и задачи.
     * У рабочего есть ограниченный ресурс, который он может тратить на решение разных задач.
     * У задач есть сложность. Задача может выполняться не полностью.
     * Но если задача выполнена полностью, то больше никто не может её выполнять.
     * Задача выполнена полностью, если сумма затраченных на неё работниками ресурсов равна её сложности.
     * Каждый работник должен полностью истратить свои ресурсы.
     * За то, что рабочий тратит ресурсы на задачи, нужно платить. Стоимость разная для разных рабочих и разных задач.
     *
     * Сведение выполним так:
     * Муниципальные здания - рабочие. Их численность - это ресурс рабочего.
     * Убежище - задачи. Их сложность - вместимость убежища.
     * Стоимость работы рабочего - это время эвакуации из здания в убежище.
     * Решим задачу о назначениях, указанную выше.
     *
     * Сведем задачу о назначениях к задаче нахождения потока минимальной стоимости.
     * Построим ориентированный граф, состоящий из двух частей G следующим образом:
     * Имеется исток S и сток T.
     * В первой части находятся N вершин, соответствующие рабочим.
     * Во второй N вершин, соответствующие задачам.
     * Между каждой вершиной i первой части и каждой вершиной j второй части проведём ребро
     *      с бесконечной пропускной способностью и стоимостью, за которую данный рабочий работает над этой задачей.
     * От истока S проведём рёбра ко всем вершинам i первой части
     *      с пропускной способностью равной ресурсу рабочего и стоимостью 0.
     * От каждой вершины второй части j к стоку T проведём ребро
     *      с пропускной способностью равной сложности задачи и стоимостью 0.
     * Найдём в полученном графе G максимальный поток минимальной стоимости.
     *
     * Но в данной задаче нам не нужно искать решение, а нужно лишь улучшить имеющееся.
     * Для этого сразу заполним в сети потоки.
     * А затем попробуем удешевить поток, найдя циклы отрицательной стоимости в остаточной сети.
     */

    // Решение задачи
    size_t N{0};
    size_t M{0};
    std::cin >> N >> M;
    TaskScheduler scheduler(N, M);
    std::vector<MunicipalBuilding> municipals;
    for (size_t i = 0; i < N; ++i) {
        int64_t x{0}, y{0}, size{0};
        std::cin >> x >> y >> size;
        municipals.push_back({x, y, size});
        scheduler.set_worker_power(i, municipals[i].size);
    }
    std::vector<Shelter> shelters;
    for (size_t i = 0; i < M; ++i) {
        int64_t x{0}, y{0}, size{0};
        std::cin >> x >> y >> size;
        shelters.push_back({x, y, size});
        scheduler.set_task_difficulty(i, shelters[i].size);
    }
    for (size_t i = 0; i < municipals.size(); ++i) {
        for (size_t j = 0; j < shelters.size(); ++j) {
            const auto cost = calc_evacuation_time(municipals[i], shelters[j]);
            int64_t flow{0};
            std::cin >> flow;
            scheduler.add_cost_work(i, j, cost, flow);
        }
    }
    auto new_schedule = scheduler.improve_schedule();
    if (!new_schedule.has_value()) {
        std::cout << "OPTIMAL";
    } else {
        std::cout << "SUBOPTIMAL" << std::endl;
        for (size_t i = 0; i < municipals.size(); ++i) {
            for (size_t j = 0; j < shelters.size(); ++j) {
                std::cout << new_schedule.value()[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    return 0;
}