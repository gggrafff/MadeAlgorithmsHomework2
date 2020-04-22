/*
 * B. Разрез
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 1024 мегабайта
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Найдите минимальный разрез между вершинами 1 и n в заданном неориентированном графе.
 *
 * Входные данные
 * На первой строке входного файла содержится n (2≤n≤100) — число вершин в графе и m (0≤m≤400) — количество ребер.
 * На следующих m строках входного файла содержится описание ребер. Ребро описывается номерами вершин,
 * которые оно соединяет, и его пропускной способностью (положительное целое число, не превосходящее 10000000),
 * при этом никакие две вершины не соединяются более чем одним ребром.
 *
 * Выходные данные
 * На первой строке выходного файла должны содержаться количество ребер в минимальном разрезе и
 * их суммарная пропускная способность.
 * На следующей строке выведите возрастающую последовательность номеров ребер
 * (ребра нумеруются в том порядке, в каком они были заданы во входном файле).
 *
 * Примеры
 * Входные данные
 * 3 3
 * 1 2 3
 * 1 3 5
 * 3 2 7
 *
 * Выходные данные
 * 2 8
 * 1 2
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>
#include <queue>
#include <random>
#include <tuple>

struct Edge {
    size_t to{0};  // куда ведёт ребро
    size_t reverse_edge_index{0};  // индекс обратного ребра
    int64_t capacity{0};  // пропускная способность ребра
    int64_t flow{0};  // поток через ребро
    bool is_min_cut{false};  // входит ли ребро в минимальный разрез
    int64_t index{0};
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
    int64_t last_index_edges{0};

    void add_undirected_edge(size_t from, size_t to, int64_t capacity, int64_t index = 0) {
        if (index == 0) {
            last_index_edges += 1;
            index = last_index_edges;
        }
        nodes[from].edges.push_back({to, 0, capacity, 0, false, index});
        nodes[to].edges.push_back({from, 0, capacity, 0, false, -index});
        nodes[from].edges.back().reverse_edge_index = nodes[to].edges.size() - 1;
        nodes[to].edges.back().reverse_edge_index = nodes[from].edges.size() - 1;
    }


    size_t compute_count_directed_edges() {
        size_t result = 0;
        for (auto &node: nodes) {
            result += node.edges.size();
        }
        return result;
    }

    size_t compute_count_undirected_edges() {
        return compute_count_directed_edges() / 2;
    }

    int64_t compute_summary_capacity_directed_edges() {
        int64_t result = 0;
        for (auto &node: nodes) {
            for (auto &edge: node.edges) {
                result += edge.capacity;
            }
        }
        return result;
    }

    int64_t compute_summary_capacity_undirected_edges() {
        return compute_summary_capacity_directed_edges() / 2;
    }

    Edge get_random_edge() {
        static std::random_device rd;  //Will be used to obtain a seed for the random number engine
        static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        static std::uniform_int_distribution<size_t> numbers_generator(0, std::max(compute_count_directed_edges(),
                                                                                   nodes.size()));

        auto node_index = numbers_generator(rd) % nodes.size();
        while(nodes[node_index].edges.size() == 0)
        {
            node_index = numbers_generator(rd) % nodes.size();
        }
        auto edge_index = numbers_generator(rd) % nodes[node_index].edges.size();

        while (((node_index == source_index) && (nodes[node_index].edges[edge_index].to == sink_index))||
                ((node_index == sink_index) && (nodes[node_index].edges[edge_index].to == source_index)))
        {
            node_index = numbers_generator(rd) % nodes.size();
            while(nodes[node_index].edges.size() == 0)
            {
                node_index = numbers_generator(rd) % nodes.size();
            }
            edge_index = numbers_generator(rd) % nodes[node_index].edges.size();
        }

        /*while (nodes[node_index].edges[edge_index].capacity == 0)
        {
            node_index = numbers_generator(rd) % nodes.size();
            edge_index = numbers_generator(rd) % nodes[node_index].edges.size();
        }*/

        return nodes[node_index].edges[edge_index];
    }

    void remove_node(size_t index)
    {
        assert(sink_index!=index);
        assert(source_index!=index);
        nodes.erase(nodes.begin() + index);
        if (sink_index > index){
            sink_index -=1;
        }
        if (source_index > index){
            source_index -=1;
        }
        for (auto &node: nodes) {
            for (auto &edge: node.edges) {
                assert(edge.to != index);
                if (edge.to > index) {
                    edge.to -= 1;
                }
            }
        }
    }

    void contract_undirected_edge(Edge &contract_edge) {
        auto u = nodes[contract_edge.to].edges[contract_edge.reverse_edge_index].to;
        auto v = contract_edge.to;
        assert (!((u == sink_index) && (v == source_index)));
        assert (!((u == source_index) && (v == sink_index)));
        for (auto i = 0; i < nodes[v].edges.size(); ++i) {
            auto &e = nodes[v].edges[i];
            if (e.to != u) {
                add_undirected_edge(u, e.to, e.capacity, e.index);
            }
            nodes[e.to].edges.erase(nodes[e.to].edges.begin() + e.reverse_edge_index);
            for (auto j = e.reverse_edge_index; j < nodes[e.to].edges.size(); ++j) {
                auto &re = nodes[e.to].edges[j];
                nodes[re.to].edges[re.reverse_edge_index].reverse_edge_index -= 1;
            }
        }
        if (v == sink_index) {
            sink_index = u;
        }
        else if (v == source_index) {
            source_index = u;
        }
        remove_node(v);
        if (u>v)
        {
            u-=1;
        }
        if (nodes[u].edges.empty())
        {
            if ((u == source_index) || (u ==sink_index)){
                for (size_t i =0; i<nodes.size(); ++i)
                {
                    if ((i!=sink_index) && (i!=source_index))
                    {
                        remove_node(i);
                    }
                }
            }
            else {
                remove_node(u);
            }
        }
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
        for (auto &edge: node.edges) {
            assert(edge.flow == -network.nodes[edge.to].edges[edge.reverse_edge_index].flow);
        }
    }
    return maxFlow;
}

size_t find_min_cut_bfs(FlowNetwork &network) {
    //заполняем массив d значениями, равными бесконечности
    for (auto &node: network.nodes) {
        node.depth = std::numeric_limits<size_t>::max();
    }
    network.nodes[network.source_index].depth = 0;

    size_t count = 0;
    std::queue<size_t> Q;
    Q.push(network.source_index);
    while (!Q.empty()) {
        auto u = Q.front();
        Q.pop();
        for (auto &edge: network.nodes[u].edges) {
            if ((network.nodes[edge.to].depth == std::numeric_limits<size_t>::max()) || (network.nodes[edge.to].depth > network.nodes[u].depth)) {
                network.nodes[edge.to].depth = network.nodes[u].depth + 1;
                if ((edge.flow >= edge.capacity) || (edge.flow <= -edge.capacity)) {
                    edge.is_min_cut = true;
                    count += 1;
                } else {
                    edge.is_min_cut = false;
                    Q.push(edge.to);
                }
            }
        }
    }
    return count;
}

std::tuple<int64_t, size_t, std::vector<size_t>> getCut(FlowNetwork network) {
    auto vertex_count = network.nodes.size();
    std::vector<size_t> edges_idx;
    while (vertex_count > 2) {
        auto edge = network.get_random_edge();
        network.contract_undirected_edge(edge); //стягиваем ребро edge
        vertex_count--;
    }
    for (auto &edge: network.nodes[0].edges) {
        edges_idx.push_back(std::abs(edge.index));
    }
    auto capacity = network.compute_summary_capacity_undirected_edges();
    auto edges_count = network.compute_count_undirected_edges();
    return {capacity,
            edges_count,
            edges_idx};
}


std::tuple<int64_t, size_t, std::vector<size_t>> find_min_cut_karger(FlowNetwork network) {
    if (network.nodes[network.source_index].edges.empty() || network.nodes[network.sink_index].edges.empty())
    {
        return {0, 0, {}};
    }

    for (size_t v = 0; v<network.nodes.size(); ++v)
    {
        if (network.nodes[v].edges.empty())
        {
            network.remove_node(v);
            --v;
        }
    }
    double p = 0.002;
    double n = sqrt(1.0/p);
    auto count = static_cast<size_t>(1000 + std::pow(n, 2) * log(n));
    auto answer = getCut(network);
    for (auto i = 1; i < count; ++i) {
        auto curCut = getCut(network);
        if (std::get<0>(curCut) < std::get<0>(answer)) {
            answer = curCut;
        }
    }
    return answer;
}

// Начало тестов


void test_from_task() {
    size_t N = 3;
    size_t M = 3;
    FlowNetwork network;
    network.source_index = 0;
    network.sink_index = N - 1;
    for (size_t i = 0; i < N; ++i) {
        network.nodes.emplace_back();
    }
    std::vector<std::pair<size_t, size_t>> edges_idx;
    network.add_undirected_edge(0, 1, 3);
    edges_idx.emplace_back(0, network.nodes[0].edges.size() - 1);
    network.add_undirected_edge(0, 2, 5);
    edges_idx.emplace_back(0, network.nodes[0].edges.size() - 1);
    network.add_undirected_edge(2, 1, 7);
    edges_idx.emplace_back(2, network.nodes[2].edges.size() - 1);
    auto max_flow = find_max_flow_dinic(network);
    assert(max_flow == 8);
    auto count_min_cut = find_min_cut_bfs(network);
    assert(count_min_cut == 2);
    assert(network.nodes[edges_idx[0].first].edges[edges_idx[0].second].is_min_cut);
    assert(network.nodes[edges_idx[1].first].edges[edges_idx[1].second].is_min_cut);
    assert(!network.nodes[edges_idx[2].first].edges[edges_idx[2].second].is_min_cut);

    auto[karger_flow, karger_count, edges] = find_min_cut_karger(network);
    std::sort(edges.begin(), edges.end());
    assert(karger_flow == max_flow);
    assert(karger_count == count_min_cut);
    assert(edges.size() == karger_count);
    assert(edges[0] == 1);
    assert(edges[1] == 2);

    for (size_t i = 0; i < edges_idx.size(); ++i) {
        auto& edge_idx = edges_idx[i];
        if (network.nodes[edge_idx.first].edges[edge_idx.second].is_min_cut)
        {
            assert(edges[i] == i + 1);
        }
    }
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
    auto count_min_cut = find_min_cut_bfs(network);
    assert(max_flow == 17);
    auto[karger_flow, karger_count, edges] = find_min_cut_karger(network);
    std::sort(edges.begin(), edges.end());
    assert(karger_flow == max_flow);
    assert(count_min_cut == karger_count);
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
    auto count_min_cut = find_min_cut_bfs(network);
    assert(max_flow == 3);
    assert(network.nodes[edges_idx[0].first].edges[edges_idx[0].second].flow == 2);
    assert(network.nodes[edges_idx[1].first].edges[edges_idx[1].second].flow == 1);
    assert(network.nodes[edges_idx[2].first].edges[edges_idx[2].second].flow == 2);
    assert(network.nodes[edges_idx[3].first].edges[edges_idx[3].second].flow == -1);
    assert(network.nodes[edges_idx[4].first].edges[edges_idx[4].second].flow == 2);
    assert(network.nodes[edges_idx[5].first].edges[edges_idx[5].second].flow == -1);

    auto[karger_flow, karger_count, edges] = find_min_cut_karger(network);
    std::sort(edges.begin(), edges.end());
    assert(karger_flow == max_flow);
    assert(count_min_cut == karger_count);
}

void test_random_sparse() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<size_t> small_numbers_generator(0, 10);
    std::uniform_int_distribution<size_t> big_numbers_generator(0, 1000);

    for(auto experimentNo = 0; experimentNo < 100; ++experimentNo){
        size_t N = small_numbers_generator(rd) + 2;
        size_t M = small_numbers_generator(rd) + 1;
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
            while(t == f){
                t = big_numbers_generator(rd) % N;
            }
            int64_t c = small_numbers_generator(rd) + 1;
            network.add_undirected_edge(f, t, c);
            edges_idx.emplace_back(f, network.nodes[f].edges.size() - 1);
        }
        auto max_flow = find_max_flow_dinic(network);
        int64_t count_min_cut = 0;
        if (max_flow!=0){
            count_min_cut = find_min_cut_bfs(network);
        }
        auto [karger_flow, karger_count, edges] = find_min_cut_karger(network);
        assert(karger_flow == max_flow);
        assert(count_min_cut == karger_count);
    }
}

void test_random_dense() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<size_t> small_numbers_generator(0, 10);
    std::uniform_int_distribution<size_t> big_numbers_generator(0, 1000);

    for(auto experimentNo = 0; experimentNo < 100; ++experimentNo){
        size_t N = small_numbers_generator(rd) + 2;
        size_t M = small_numbers_generator(rd) * small_numbers_generator(rd) + 1;
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
            while(t == f){
                t = big_numbers_generator(rd) % N;
            }
            int64_t c = small_numbers_generator(rd) + 1;
            network.add_undirected_edge(f, t, c);
            edges_idx.emplace_back(f, network.nodes[f].edges.size() - 1);
        }
        auto max_flow = find_max_flow_dinic(network);
        int64_t count_min_cut = 0;
        if (max_flow!=0){
            count_min_cut = find_min_cut_bfs(network);
        }
        auto [karger_flow, karger_count, edges] = find_min_cut_karger(network);
        assert(karger_flow == max_flow);
        assert(count_min_cut == karger_count);
    }
}

void run_all_tests() {
    test_from_task();
    test_by_tus();
    test_five();
    test_random_sparse();
    test_random_dense();
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
    size_t M{0};
    std::cin >> N >> M;
    FlowNetwork network;
    network.source_index = 0;
    network.sink_index = N - 1;
    for (size_t i = 0; i < N; ++i) {
        network.nodes.emplace_back();
    }
    //std::vector<std::pair<size_t, size_t>> edges_idx;
    for (size_t i = 0; i < M; ++i) {
        size_t f{0}, t{0};
        int64_t c{0};
        std::cin >> f >> t >> c;
        assert(f != t);
        assert((f >= 1) && (f <= N));
        assert((t >= 1) && (t <= N));
        assert(c >= 0);
        network.add_undirected_edge(f - 1, t - 1, c);
        //edges_idx.emplace_back(f - 1, network.nodes[f - 1].edges.size() - 1);
    }
    /*auto max_flow = find_max_flow_dinic(network);
    assert(max_flow >= 0);
    auto count_min_cut = find_min_cut_bfs(network);*/
    auto[max_flow, count_min_cut, edges_idx] = find_min_cut_karger(network);
    if (max_flow == 0) {
        std::cout << 0 << " " << 0 << std::endl;
    } else {
        std::sort(edges_idx.begin(), edges_idx.end());
        std::cout << count_min_cut << " " << max_flow << std::endl;
        for (size_t i = 0; i < edges_idx.size(); ++i) {
            /*auto& edge_idx = edges_idx[i];
            if (network.nodes[edge_idx.first].edges[edge_idx.second].is_min_cut)
            {
                std::cout << i + 1 << " " ;
            }*/
            std::cout << edges_idx[i] << " ";
        }
    }


    return 0;
}