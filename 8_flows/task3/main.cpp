/*
 * C. Улиточки
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 1024 мегабайта
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Две улиточки Маша и Петя сейчас находятся на лужайке с абрикосами и хотят добраться до своего домика.
 * Лужайки пронумерованы числами от 1 до n и соединены дорожками (может быть несколько дорожек соединяющих две лужайки,
 * могут быть дорожки, соединяющие лужайку с собой же). В виду соображений гигиены, если по дорожке проползла улиточка,
 * то вторая по той же дорожке уже ползти не может.
 * Помогите Пете и Маше добраться до домика.
 *
 * Входные данные
 * В первой строке файла записаны четыре целых числа — n, m, s и t
 * (количество лужаек, количество дорог, номер лужайки с абрикосами и номер домика).
 * В следующих m строках записаны пары чисел. Пара чисел (x, y) означает, что есть дорожка с лужайки x до лужайки y
 * (из-за особенностей улиток и местности дорожки односторонние).
 * Ограничения: 2<=n<=10^5, 0<=m<=10^5, s != t.
 *
 * Выходные данные
 * Если существует решение, то выведите YES и на двух отдельных строчках сначала последовательность лужаек для Машеньки
 * (дам нужно пропускать вперед), затем путь для Пети.
 * Если решения не существует, выведите NO.
 * Если решений несколько, выведите любое.
 *
 * Примеры
 * Входные данные
 * 3 3 1 3
 * 1 2
 * 1 3
 * 2 3
 *
 * Выходные данные
 * YES
 * 1 3
 * 1 2 3
 *
 * Примечание
 * Дан орграф, найти два непересекающихся по ребрам пути из s в t, вывести вершины найденных путей.
 */

/*
 * https://compendium.su/informatics/algorithm/51.html
 * https://neerc.ifmo.ru/wiki/index.php?title=Теорема_о_декомпозиции
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
#include <set>

/**
 * @brief Структура, описывающая ребро сети.
 */
struct Edge {
    size_t to{0};  // куда ведёт ребро
    int64_t reverse_edge_index{-1};  // индекс обратного ребра в другой вершине
    int64_t capacity{0};  // пропускная способность ребра
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
 * Структура, описывающая сеть.
 */
struct FlowNetwork {
    std::vector<NodeFlowNetwork> nodes;  // массив вершин
    size_t source_index{0};  // индекс вершины, являющейся истоком
    size_t sink_index{0};  // индекс вершины, являющейся стоком
    int64_t last_index_edges{0};  // индекс последнего добавленного ребра (см. Edge::index)

    /**
     * @brief Добавляет неориентированное ребро в сеть.
     * @details Добавляет два ориентированных ребра, направленных в разные стороны.
     * @param from Индекс одной из вершин, соединённых ребром.
     * @param to Индекс второй из вершин, соединённых ребром.
     * @param capacity Пропускная способность ребра.
     */
    void add_undirected_edge(size_t from, size_t to, int64_t capacity, int64_t index = 0) {
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
        nodes[from].edges.push_back({to, 0, capacity, 0, index});
        nodes[to].edges.push_back({from, 0, capacity, 0, -index});
        nodes[from].edges.back().reverse_edge_index = nodes[to].edges.size() - 1;
        nodes[to].edges.back().reverse_edge_index = nodes[from].edges.size() - 1;
    }

    /**
     * @brief Добавляет ориентированное ребро в сеть.
     * @details Добавляет два ориентированных ребра, направленных в разные стороны (обратное нулевой ёмкости).
     * @param from Индекс одной из вершин, соединённых ребром.
     * @param to Индекс второй из вершин, соединённых ребром.
     * @param capacity Пропускная способность ребра.
     */
    void add_directed_edge(size_t from, size_t to, int64_t capacity, int64_t index = 0) {
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
        nodes[from].edges.push_back({to, 0, capacity, 0, index});
        nodes[to].edges.push_back({from, 0, 0, 0, -index});
        nodes[from].edges.back().reverse_edge_index = nodes[to].edges.size() - 1;
        nodes[to].edges.back().reverse_edge_index = nodes[from].edges.size() - 1;
    }
};


/**
 * @brief Поиском в ширину определяет удаление вершин от истока.
 * @details Часть алгоритма Диница.
 * @details Вместо построения слоистой сети просто измерим удаление вершин от истока и учтём их при дальнейшем обходе.
 * @param network Сеть.
 * @return true, если сток достижим из истока, иначе - false.
 */
bool measure_depth_vertices(FlowNetwork &network) {
    //заполняем массив d значениями, равными бесконечности
    for (auto &node: network.nodes) {
        node.depth = std::numeric_limits<size_t>::max();
    }

    network.nodes[network.source_index].depth = 0;

    std::queue<size_t> Q;
    Q.push(network.source_index);
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        for (auto &edge: network.nodes[u].edges) {
            if ((edge.flow < edge.capacity) && (network.nodes[edge.to].depth == std::numeric_limits<size_t>::max())) {
                network.nodes[edge.to].depth = network.nodes[u].depth + 1;
                Q.push(edge.to);
            }
        }
    }
    return (network.nodes[network.sink_index].depth != std::numeric_limits<size_t>::max());
}

/**
 * @brief Поиск блокирующего потока
 * @details Часть алгоритма Диница.
 * @param network Сеть.
 * @param u Номер вершины, из которой производим поиск.
 * @param minC Минимальная пропускная способность дополняющей сети на пройденном пути.
 * @return Сколько удалось пропустить по этому пути.
 */
int64_t find_block_flow(FlowNetwork &network, size_t u, int64_t minC) {
    assert(minC >= 0);
    if ((u == network.sink_index) || (minC == 0)) {
        return minC;
    }
    for (auto ei = network.nodes[u].p; ei < network.nodes[u].edges.size(); ++ei) {
        auto &edge = network.nodes[u].edges[ei];

        if (network.nodes[edge.to].depth == network.nodes[u].depth + 1) {
            // это условие эквивалентно поиску во вспомогательной слоистой сети
            auto delta = find_block_flow(network, edge.to, std::min(minC, edge.capacity - edge.flow));
            assert(delta >= 0);
            if (delta != 0) {
                edge.flow += delta; // насыщаем рёбра по пути
                assert(edge.flow <= edge.capacity);
                assert(edge.flow >= -edge.capacity);
                auto &reverse_edge = network.nodes[edge.to].edges[edge.reverse_edge_index];
                assert(reverse_edge.reverse_edge_index == ei);
                reverse_edge.flow -= delta;
                return delta;
            }
        }
        network.nodes[u].p++;
    }
    return 0;
}

/**
 * @brief Поиск максимального потока алгоритмом Диница.
 * @param network Сеть, в которой ищем поток.
 * @return Значение максимального потока.
 */
int64_t find_max_flow_dinic(FlowNetwork &network) {
    int64_t maxFlow = 0;
    while (measure_depth_vertices(network)) {  // пересчитываем d[i], заодно проверяем достижима ли t из s
        for (auto &node: network.nodes) {
            node.p = 0;  // заполняем p нулями
        }
        auto flow = find_block_flow(network, network.source_index, std::numeric_limits<int64_t>::max());
        while (flow != 0) {
            maxFlow += flow;
            assert(flow >= 0);
            flow = find_block_flow(network, network.source_index, std::numeric_limits<int64_t>::max());
        }
    }

    // просто проверка корректности, надеюсь, компилятор это соптимизирует в release сборке
    for (auto &node: network.nodes) {
        for (auto &edge: node.edges) {
            assert(edge.flow == -network.nodes[edge.to].edges[edge.reverse_edge_index].flow);
        }
    }

    return maxFlow;
}


/**
 * @brief Ищет элементарный поток в сети.
 * @details Применять в сети, в которой расставлены потоки по рёбрам.
 * @details Зануляет поток вдоль найденного пути.
 * @param network Сеть.
 * @param from Откуда ищем путь.
 * @param to Куда ищем путь.
 * @param edges Массив, в который сохранятся индексы рёбер, составляющих путь.
 * @return Вершины, через которые проходит путь, либо nullopt, если пути нет.
 */
std::optional<std::vector<size_t>>
find_path(FlowNetwork &network, size_t from, size_t to, std::vector<int64_t> &edges) {
    network.nodes[from].visited = true;
    if (from == to) {
        return std::vector<size_t>{from};
    }
    for (auto &edge: network.nodes[from].edges) {
        if ((edge.flow != 0) && (!network.nodes[edge.to].visited)) {
            auto result = find_path(network, edge.to, to, edges);
            if (result.has_value()) {
                result.value().push_back(from);
                edge.flow = 0;
                edges.push_back(edge.index);
                return result;
            }
        }
    }
    return std::nullopt;
}

/**
 * @brief Ищет нужное количество элементраных потоков (путей) в сети с потоком.
 * @param network Сеть (с расставлеными потоками по рёбрам).
 * @param count Количество искомых путей.
 * @return Массив путей. Путь - массив вершин, через которые он проходит.
 */
std::vector<std::vector<size_t>> find_pathes(FlowNetwork &network, size_t count) {
    std::vector<std::vector<size_t>> result;
    std::vector<std::vector<int64_t>> edges_all;
    while (result.size() < count) {
        for (auto &node: network.nodes) {
            node.visited = false;
        }
        std::vector<int64_t> edges;
        auto path = find_path(network, network.source_index, network.sink_index, edges);
        if (!path.has_value()) {
            return result;
        }
        edges_all.push_back(edges);
        result.push_back(path.value());
        std::reverse(result.back().begin(), result.back().end());
        assert(result.back().front() == network.source_index);
        assert(result.back().back() == network.sink_index);
    }

    // просто проверка корректности, надеюсь, компилятор это соптимизирует в release сборке
    std::set<int64_t> check_edges;
    size_t count_edges = 0;
    for (auto &edges: edges_all) {
        for (auto edge_idx: edges) {
            check_edges.insert(edge_idx);
        }
        count_edges += edges.size();
    }
    assert(check_edges.size() == count_edges);

    return result;
}

// Начало тестов


void test_from_task() {
    size_t N = 3;
    size_t M = 3;
    FlowNetwork network;
    network.source_index = 0;
    network.sink_index = 2;
    for (size_t i = 0; i < N; ++i) {
        network.nodes.emplace_back();
    }
    network.add_directed_edge(0, 1, 1);
    network.add_directed_edge(0, 2, 1);
    network.add_directed_edge(1, 2, 1);
    auto max_flow = find_max_flow_dinic(network);
    assert(max_flow >= 2);
    auto result = find_pathes(network, 2);
    assert(result.size() == 2);
}

void test_rhombus_directed_undirected() {
    size_t N = 4;
    size_t M = 11;

    FlowNetwork network1;
    network1.source_index = 0;
    network1.sink_index = 3;
    FlowNetwork network2;
    network2.source_index = 0;
    network2.sink_index = 3;

    for (size_t i = 0; i < N; ++i) {
        network1.nodes.emplace_back();
        network2.nodes.emplace_back();
    }

    network1.add_directed_edge(0, 2, 1);
    network1.add_directed_edge(0, 3, 1);
    network1.add_directed_edge(1, 0, 1);
    network1.add_directed_edge(1, 2, 1);
    network1.add_directed_edge(1, 2, 1);
    network1.add_directed_edge(1, 2, 1);
    network1.add_directed_edge(1, 3, 1);
    network1.add_directed_edge(2, 1, 1);
    network1.add_directed_edge(3, 2, 1);
    network1.add_directed_edge(3, 2, 1);
    network1.add_directed_edge(3, 2, 1);

    network2.add_undirected_edge(0, 2, 1);
    network2.add_undirected_edge(0, 3, 1);
    network2.add_undirected_edge(1, 0, 1);
    network2.add_undirected_edge(1, 2, 1);
    network2.add_undirected_edge(1, 2, 1);
    network2.add_undirected_edge(1, 2, 1);
    network2.add_undirected_edge(1, 3, 1);
    network2.add_undirected_edge(2, 1, 1);
    network2.add_undirected_edge(3, 2, 1);
    network2.add_undirected_edge(3, 2, 1);
    network2.add_undirected_edge(3, 2, 1);

    auto max_flow1 = find_max_flow_dinic(network1);
    assert(max_flow1 == 2);
    auto max_flow2 = find_max_flow_dinic(network2);
    assert(max_flow2 == 3);

    auto pathes1 = find_pathes(network1, 2);
    auto pathes2 = find_pathes(network2, 2);
    //assert(pathes1 == pathes2);
}

void test_random_big() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<size_t> small_numbers_generator(0, 10);
    std::uniform_int_distribution<size_t> big_numbers_generator(0, 1000);

    for (auto experimentNo = 0; experimentNo < 100; ++experimentNo) {
        size_t N = small_numbers_generator(rd) + 2;
        size_t M = 2 * small_numbers_generator(rd) + 1;
        FlowNetwork network;
        network.source_index = 0;
        network.sink_index = N - 1;
        for (size_t i = 0; i < N; ++i) {
            network.nodes.emplace_back();
        }
        std::vector<std::pair<size_t, size_t>> edges_idx;
        for (size_t i = 0; i < M; ++i) {
            size_t f = big_numbers_generator(rd) % N;
            size_t t = big_numbers_generator(rd) % N;
            while (t == f) {
                t = big_numbers_generator(rd) % N;
            }
            network.add_directed_edge(f, t, 1);
            edges_idx.emplace_back(f, network.nodes[f].edges.size() - 1);
        }
        auto max_flow = find_max_flow_dinic(network);
        if (max_flow >= 2) {
            auto pathes = find_pathes(network, 2);
            assert(pathes.size() == 2);
        }
    }
}

void test_random_directed_undirected() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<size_t> small_numbers_generator(0, 10);
    std::uniform_int_distribution<size_t> big_numbers_generator(0, 1000);

    for (auto experimentNo = 0; experimentNo < 100; ++experimentNo) {
        size_t N = small_numbers_generator(rd) + 2;
        size_t M = 2 * small_numbers_generator(rd) + 1;

        FlowNetwork network1;
        network1.source_index = 0;
        network1.sink_index = N - 1;

        FlowNetwork network2;
        network2.source_index = 0;
        network2.sink_index = N - 1;

        for (size_t i = 0; i < N; ++i) {
            network1.nodes.emplace_back();
            network2.nodes.emplace_back();
        }

        for (size_t i = 0; i < M; ++i) {
            size_t f = big_numbers_generator(rd) % N;
            size_t t = big_numbers_generator(rd) % N;
            while (t == f) {
                t = big_numbers_generator(rd) % N;
            }
            network1.add_directed_edge(f, t, 1);
            network2.add_undirected_edge(f, t, 1);
        }
        auto max_flow1 = find_max_flow_dinic(network1);
        auto max_flow2 = find_max_flow_dinic(network2);
        auto pathes1 = find_pathes(network1, 2);
        auto pathes2 = find_pathes(network2, 2);
        //assert(pathes1.size() == pathes2.size());
        //assert(pathes1 == pathes2);
    }
}

void test_random_dense() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<size_t> big_numbers_generator(0, 1000);

    for (auto experimentNo = 0; experimentNo < 30; ++experimentNo) {
        size_t N = big_numbers_generator(rd) + 2;
        size_t M = 2 * big_numbers_generator(rd) * big_numbers_generator(rd) + 1;
        FlowNetwork network;
        network.source_index = 0;
        network.sink_index = N - 1;
        for (size_t i = 0; i < N; ++i) {
            network.nodes.emplace_back();
        }
        std::vector<std::pair<size_t, size_t>> edges_idx;
        for (size_t i = 0; i < M; ++i) {
            size_t f = big_numbers_generator(rd) % N;
            size_t t = big_numbers_generator(rd) % N;
            while (t == f) {
                t = big_numbers_generator(rd) % N;
            }
            network.add_directed_edge(f, t, 1);
            edges_idx.emplace_back(f, network.nodes[f].edges.size() - 1);
        }
        auto max_flow = find_max_flow_dinic(network);
        if (max_flow >= 2) {
            auto pathes = find_pathes(network, 2);
            assert(pathes.size() == 2);
        }
    }
}

void test_empty() {
    size_t N = 3;
    size_t M = 0;
    FlowNetwork network;
    network.source_index = 0;
    network.sink_index = 2;
    for (size_t i = 0; i < N; ++i) {
        network.nodes.emplace_back();
    }
    auto max_flow = find_max_flow_dinic(network);
    assert(max_flow == 0);
    auto result = find_pathes(network, 2);
    assert(result.size() == 0);
}

void run_all_tests() {
    test_from_task();
    test_rhombus_directed_undirected();
    test_empty();
    test_random_big();
    test_random_dense();
    test_random_directed_undirected();
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
    size_t M{0};
    size_t S{0};
    size_t T{0};
    std::cin >> N >> M >> S >> T;
    FlowNetwork network;
    network.source_index = S - 1;
    network.sink_index = T - 1;
    for (size_t i = 0; i < N; ++i) {
        network.nodes.emplace_back();
    }
    for (size_t i = 0; i < M; ++i) {
        size_t from{0}, to{0};
        std::cin >> from >> to;
        network.add_directed_edge(from - 1, to - 1, 1);
    }
    const auto max_flow = find_max_flow_dinic(network);
    if (max_flow >= 2) {
        std::cout << "YES" << std::endl;
        const auto pathes = find_pathes(network, 2);
        assert(pathes.size() == 2);
        for (auto &path: pathes) {
            for (auto &u: path) {
                std::cout << u + 1 << " ";
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "NO";
    }

    return 0;
}