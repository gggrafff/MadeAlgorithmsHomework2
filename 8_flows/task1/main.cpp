/*
 * A. Просто поток
 * ограничение по времени на тест 5 секунд
 * ограничение по памяти на тест 1024 мегабайта
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дана система из узлов и труб, по которым может течь вода.
 * Для каждой трубы известна наибольшая скорость, с которой вода может протекать через нее.
 * Известно, что вода течет по трубам таким образом, что за единицу времени в каждый узел
 * (за исключением двух — источника и стока) втекает ровно столько воды, сколько из него вытекает.
 *
 * Ваша задача — найти наибольшее количество воды, которое за единицу времени может протекать между источником и
 * стоком,а также скорость течения воды по каждой из труб.
 *
 * Трубы являются двусторонними, то есть вода в них может течь в любом направлении.
 * Между любой парой узлов может быть более одной трубы.
 *
 * Входные данные
 * В первой строке записано натуральное число N — количество узлов в системе (2≤N≤100).
 * Известно, что источник имеет номер 1, а сток номер N.
 * Во второй строке записано натуральное M (1≤M≤5000) — количество труб в системе.
 * Далее в M строках идет описание труб.
 * Каждая труба задается тройкой целых чисел Ai, Bi, Ci,
 * где Ai, Bi — номера узлов,которые соединяет данная труба (Ai≠Bi),
 * а Ci (0≤Ci≤104) — наибольшая допустимая скорость течения воды через данную трубу.
 *
 * Выходные данные
 * В первой строке выведите наибольшее количество воды, которое протекает между источником и стоком за единицу времени.
 * Далее выведите M строк, в каждой из которых выведите скорость течения воды по соответствующей трубе.
 * Если направление не совпадает с порядком узлов, заданным во входных данных, то выводите скорость со знаком минус.
 * Числа выводите с точностью 10−3.
 *
 * Примеры
 * Входные данные
 * 2
 * 2
 * 1 2 1
 * 2 1 3
 *
 * Выходные данные
 * 4
 * 1
 * -3
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>
#include <queue>

struct Edge {
    size_t to{0};  // куда ведёт ребро
    size_t reverse_edge_index{0};  // индекс обратного ребра
    int64_t capacity{0};  // пропускная способность ребра
    int64_t flow{0};  // поток через ребро
};
struct NodeFlowNetwork {
    size_t depth{std::numeric_limits<size_t>::max()};  // удалённость от истока
    size_t p{0};  // индекс первого неудалённого ребра, идущего из узла
    std::vector<Edge> edges;
};

struct FlowNetwork {
    std::vector<NodeFlowNetwork> nodes;
    size_t source_index{0};
    size_t sink_index{0};

    void add_undirected_edge(size_t from, size_t to, int64_t capacity) {
        nodes[from].edges.push_back({to, 0, capacity, 0});
        nodes[to].edges.push_back({from, 0, capacity, 0});
        nodes[from].edges.back().reverse_edge_index = nodes[to].edges.size() - 1;
        nodes[to].edges.back().reverse_edge_index = nodes[from].edges.size() - 1;
    }
};


bool bfs(FlowNetwork &network) {
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


// поиск блокирующего потока
// u — номер вершины
// minC — минимальная пропускная способность дополняющей сети на пройденном dfs пути
int64_t dfs(FlowNetwork &network, size_t u, int64_t minC) {
    assert(minC >= 0);
    if ((u == network.sink_index) || (minC == 0)) {
        return minC;
    }
    for (auto ei = network.nodes[u].p; ei < network.nodes[u].edges.size(); ++ei) {
        auto &edge = network.nodes[u].edges[ei];
        auto &reverse_edge = network.nodes[edge.to].edges[edge.reverse_edge_index];
        assert(reverse_edge.reverse_edge_index == ei);

        if (network.nodes[edge.to].depth == network.nodes[u].depth + 1) {
            // это условие эквивалентно поиску во вспомогательной слоистой сети
            auto delta = dfs(network, edge.to, std::min(minC, edge.capacity - edge.flow));
            assert(delta >= 0);
            if (delta != 0) {
                edge.flow += delta; // насыщаем рёбра по пути dfs
                assert(edge.flow <= edge.capacity);
                assert(edge.flow >= -edge.capacity);
                reverse_edge.flow -= delta;
                assert(reverse_edge.flow <= reverse_edge.capacity);
                assert(reverse_edge.flow >= -reverse_edge.capacity);
                return delta;
            }
        }
        network.nodes[u].p++;
    }
    return 0;
}

int64_t find_max_flow_dinic(FlowNetwork &network) {
    int64_t maxFlow = 0;
    while (bfs(network)) {  // пересчитываем d[i], заодно проверяем достижима ли t из s
        for (auto &node: network.nodes) {
            node.p = 0;  // заполняем p нулями
        }
        auto flow = dfs(network, network.source_index, std::numeric_limits<int64_t>::max());
        while (flow != 0) {
            maxFlow += flow;
            assert(flow >= 0);
            flow = dfs(network, network.source_index, std::numeric_limits<int64_t>::max());
        }
    }
    for (auto &node: network.nodes) {
        for (auto& edge: node.edges)
        {
            assert(edge.flow == -network.nodes[edge.to].edges[edge.reverse_edge_index].flow);
        }
    }
    return maxFlow;
}



// Начало тестов


void test_from_task() {
    size_t N = 2;
    size_t M = 2;
    FlowNetwork network;
    network.source_index = 0;
    network.sink_index = N - 1;
    for (size_t i = 0; i < N; ++i) {
        network.nodes.emplace_back();
    }
    std::vector<std::pair<size_t, size_t>> edges_idx;
    network.add_undirected_edge(0, 1, 1);
    edges_idx.emplace_back(0, network.nodes[0].edges.size() - 1);
    network.add_undirected_edge(1, 0, 3);
    edges_idx.emplace_back(1, network.nodes[1].edges.size() - 1);
    auto max_flow = find_max_flow_dinic(network);
    assert(max_flow == 4);
    assert(network.nodes[edges_idx[0].first].edges[edges_idx[0].second].flow == 1);
    assert(network.nodes[edges_idx[1].first].edges[edges_idx[1].second].flow == -3);
}

void test_five() {
    size_t N = 5;
    size_t M = 6;
    FlowNetwork network;
    network.source_index = 0;
    network.sink_index = N - 1;
    for (size_t i = 0; i < N; ++i) {
        network.nodes.emplace_back();
    }
    std::vector<std::pair<size_t, size_t>> edges_idx;
    network.add_undirected_edge(0, 1, 2);
    edges_idx.emplace_back(0, network.nodes[0].edges.size() - 1);
    network.add_undirected_edge(0, 2, 1);
    edges_idx.emplace_back(0, network.nodes[0].edges.size() - 1);
    network.add_undirected_edge(1, 3, 3);
    edges_idx.emplace_back(1, network.nodes[1].edges.size() - 1);
    network.add_undirected_edge(2, 3, 5);
    edges_idx.emplace_back(2, network.nodes[2].edges.size() - 1);
    network.add_undirected_edge(2, 4, 2);
    edges_idx.emplace_back(2, network.nodes[2].edges.size() - 1);
    network.add_undirected_edge(4, 3, 1);
    edges_idx.emplace_back(4, network.nodes[4].edges.size() - 1);
    auto max_flow = find_max_flow_dinic(network);
    assert(max_flow == 3);
    assert(network.nodes[edges_idx[0].first].edges[edges_idx[0].second].flow == 2);
    assert(network.nodes[edges_idx[1].first].edges[edges_idx[1].second].flow == 1);
    assert(network.nodes[edges_idx[2].first].edges[edges_idx[2].second].flow == 2);
    assert(network.nodes[edges_idx[3].first].edges[edges_idx[3].second].flow == -1);
    assert(network.nodes[edges_idx[4].first].edges[edges_idx[4].second].flow == 2);
    assert(network.nodes[edges_idx[5].first].edges[edges_idx[5].second].flow == -1);
}

void test_by_tus() {
    size_t N = 6;
    size_t M = 7;
    FlowNetwork network;
    network.source_index = 0;
    network.sink_index = N - 1;
    for (size_t i = 0; i < N; ++i) {
        network.nodes.emplace_back();
    }
    std::vector<std::pair<size_t, size_t>> edges_idx;
    network.add_undirected_edge(0, 1, 10);
    edges_idx.emplace_back(0, network.nodes[0].edges.size() - 1);
    network.add_undirected_edge(1, 2, 30);
    edges_idx.emplace_back(1, network.nodes[1].edges.size() - 1);
    network.add_undirected_edge(2, 4, 20);
    edges_idx.emplace_back(2, network.nodes[2].edges.size() - 1);
    network.add_undirected_edge(4, 3, 20);
    edges_idx.emplace_back(4, network.nodes[4].edges.size() - 1);
    network.add_undirected_edge(3, 1, 20);
    edges_idx.emplace_back(3, network.nodes[3].edges.size() - 1);
    network.add_undirected_edge(2, 5, 10);
    edges_idx.emplace_back(2, network.nodes[2].edges.size() - 1);
    network.add_undirected_edge(0, 5, 7);
    edges_idx.emplace_back(2, network.nodes[2].edges.size() - 1);
    auto max_flow = find_max_flow_dinic(network);
    assert(max_flow == 17);
    /*assert(network.nodes[edges_idx[0].first].edges[edges_idx[0].second].flow == 10);
    assert(network.nodes[edges_idx[1].first].edges[edges_idx[1].second].flow == 30);
    assert(network.nodes[edges_idx[2].first].edges[edges_idx[2].second].flow == 20);
    assert(network.nodes[edges_idx[3].first].edges[edges_idx[3].second].flow == 20);
    assert(network.nodes[edges_idx[4].first].edges[edges_idx[4].second].flow == 20);
    assert(network.nodes[edges_idx[5].first].edges[edges_idx[5].second].flow == 10);
    assert(network.nodes[edges_idx[5].first].edges[edges_idx[5].second].flow == 7);*/
}

void run_all_tests() {
    test_from_task();
    test_five();
    test_by_tus();
}

// Конец тестов

int main(int argc, char *argv[]) {
    //run_all_tests();
    //return 0;

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
    network.source_index = 0;
    network.sink_index = N - 1;
    for (size_t i = 0; i < N; ++i) {
        network.nodes.emplace_back();
    }
    std::vector<std::pair<size_t, size_t>> edges_idx;
    for (size_t i = 0; i < M; ++i) {
        size_t f{0}, t{0};
        int64_t c{0};
        std::cin >> f >> t >> c;
        assert(f != t);
        assert((f >= 1) && (f <= N));
        assert((t >= 1) && (t <= N));
        assert(c >= 0);
        network.add_undirected_edge(f - 1, t - 1, c);
        edges_idx.emplace_back(f - 1, network.nodes[f - 1].edges.size() - 1);
    }
    auto max_flow = find_max_flow_dinic(network);
    assert(max_flow >= 0);
    std::cout << max_flow << std::endl;
    if (max_flow == 0) {
        for (auto &edge_idx: edges_idx) {
            std::cout << 0 << std::endl;
        }
    } else {
        for (auto &edge_idx: edges_idx) {
            std::cout << network.nodes[edge_idx.first].edges[edge_idx.second].flow << std::endl;
        }
    }

    return 0;
}