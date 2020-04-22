/*
 * E. Самое дешевое ребро
 * ограничение по времени на тест 4 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод minonpath.in
 * вывод minonpath.out
 *
 * Дано подвешенное дерево с корнем в первой вершине. Все ребра имеют веса (стоимости).
 * Вам нужно ответить на M запросов вида "найти у двух вершин минимум среди стоимостей ребер пути между ними".
 *
 * Входные данные
 * В первой строке задано целое число n — число вершин в дереве (1 ≤ n ≤ 2·10^5).
 * В следующих n - 1 строках записаны два целых числа x и y.
 * Число x на строке i означает, что x — предок вершины i, y задает стоимость ребра (x < i; |y| ≤ 10^6).
 * Далее заданы m (0 ≤ m ≤ 5·10^5) запросов вида (x, y) — найти минимум на пути из x в y (1 ≤ x, y ≤ n; x ≠ y).
 *
 * Выходные данные
 * Выведите ответы на запросы.
 *
 * Примеры
 * Входные данные
 * 5
 * 1 2
 * 1 3
 * 2 5
 * 3 2
 * 2
 * 2 3
 * 4 5
 * Выходные данные
 * 2
 * 2
 *
 * Входные данные
 * 5
 * 1 1
 * 1 2
 * 2 3
 * 3 4
 * 2
 * 1 4
 * 3 2
 * Выходные данные
 * 1
 * 1
 */

#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>



/**
 * @brief Представим взвешенный граф списками смежности вершин (вершина, вес ребра).
 */
using WeightedGraph = std::vector<std::vector<std::pair<size_t, int64_t>>>;  // списки смежности (вершина, вес ребра)

/**
 * Делает запросы на путях во взвешенном дереве.
 */
class TreeSparseTable {
public:
    /**
     * @brief Производит необходимую предобработку для дальнейших запросов в дереве.
     * @details Первая в списке вершина считается корнем.
     * @param graph Взвешенный граф (списки смежности с весами рёбер).
     * @param query_operation Операция, для которой будем делать запросы.
     * @param query_neutral_element Нейтральный элемент для операции запроса.
     */
    explicit TreeSparseTable(
            const WeightedGraph &graph,
            const std::function<int64_t(const int64_t &lhs, const int64_t &rhs)> &query_operation,
            const int64_t query_neutral_element) :
            graph_(graph),
            parents_(graph.size(), {0, query_neutral_element}),
            depth_(graph.size(), 0),
            query_operation_(query_operation),
            query_neutral_element_(query_neutral_element) {
        std::vector<bool> visited_(graph.size(), false);
        dfs(0, visited_);  // Подсчёт глубин вершин и запоминание родителей

        // Заполним массив перемещений по дереву jmp и разреженную таблицу методом ДП
        jmp_.resize(graph.size(), std::vector<size_t>(round_log2(graph.size()), 0));
        sparse_table_.resize(graph.size(), std::vector<int64_t>(round_log2(graph.size()), query_neutral_element_));
        for (size_t v = 0; v < jmp_.size(); ++v) {
            jmp_[v][0] = parents_[v].first;
            sparse_table_[v][0] = parents_[v].second;
        }
        for (size_t k = 1; k < jmp_.front().size(); ++k) {
            for (size_t v = 0; v < jmp_.size(); ++v) {
                jmp_[v][k] = jmp_[jmp_[v][k - 1]][k - 1];
                sparse_table_[v][k] = query_operation_(sparse_table_[v][k - 1], sparse_table_[jmp_[v][k - 1]][k - 1]);
            }
        }
    }

    /**
     * @brief Производит запрос на пути между двумя вершинами в дереве.
     * @param u Первая вершина (начало пути).
     * @param v Вторая вершина (конец пути).
     * @return Результат запроса.
     */
    int64_t query(size_t u, size_t v) {
        if (u == v) {
            return 0;
        }
        if (depth_[u] < depth_[v]) {
            std::swap(u, v);
        }
        // Выравниваем высоту вершин
        auto delta_depth = depth_[u] - depth_[v];
        auto result_u = query_neutral_element_;
        auto result_v = query_neutral_element_;
        for (int64_t k = jmp_.front().size() - 1; k >= 0; --k) {
            if (delta_depth >= (1 << k)) {
                result_u = query_operation_(result_u, sparse_table_[u][k]);
                u = jmp_[u][k];
                delta_depth -= (1 << k);
                assert(delta_depth >= 0);
            }
        }
        if (u == v) {
            return result_u;
        }
        // Поднимаемся от обеих вершин вверх, пока не дойдём до lca
        for (int64_t k = jmp_.front().size() - 1; k >= 0; --k) {
            auto u_top = jmp_[u][k];
            auto v_top = jmp_[v][k];
            if (u_top != v_top) {
                result_u = query_operation_(result_u, sparse_table_[u][k]);
                u = u_top;
                result_v = query_operation_(result_v, sparse_table_[v][k]);
                v = v_top;
            }
        }
        result_u = query_operation_(result_u, sparse_table_[u][0]);
        result_v = query_operation_(result_v, sparse_table_[v][0]);
        return query_operation_(result_u, result_v);
    }

private:
    /**
     * @brief Обход дерева в глубину для подсчёта глубин вершин и запоминания родителей.
     * @details Фукция рекурсивная. Предназначена для запуска на неглубоких деревьях.
     * @param u Вершина, в которой начинаем dfs.
     * @param visited Массив с отметками о посещении.
     */
    void dfs(size_t u, std::vector<bool> &visited) {
        visited[u] = true;
        for (auto v: graph_[u]) {
            if (!visited[v.first]) {
                parents_[v.first] = {u, v.second};
                depth_[v.first] = depth_[u] + 1;
                dfs(v.first, visited);
            }
        }
    }

    /**
     * @brief Округлённый в большую сторону до целого числа логарифм по основанию 2.
     * @details Используется знание о представлении чисел с плавающей точкой в памяти.
     * @details Подробнее: https://ru.wikipedia.org/wiki/Число_двойной_точности
     * @param number Число, от которого нужно взять логарифм по основанию 2.
     * @return Значение логарифма, округлённое вверх до целого.
     */
    static size_t round_log2(size_t number) {
        double x = static_cast<double>(number -
                                       1);  // если убрать "- 1", то "не меньше" в описании нужно заменить на "больше"
        auto answer = reinterpret_cast<unsigned int *>(&x);
        ++answer;
        return static_cast<size_t>(((*answer & 0x7FF00000) >> 20) - 1022);
    }

    WeightedGraph graph_;
    std::vector<std::pair<size_t, int64_t>> parents_;  // родители вершин
    std::vector<size_t> depth_;  // глубины вершин
    std::vector<std::vector<size_t>> jmp_;  // jmp[u][k] - куда мы попадём, прыгнув вверх из вершины v на 2^k рёбер.
    std::vector<std::vector<int64_t>> sparse_table_;  // разреженная таблица для запросов на маршрутах в дереве
    std::function<int64_t(const int64_t &lhs, const int64_t &rhs)> query_operation_;
    int64_t query_neutral_element_;
};

// Начало тестов

void test_from_task_1() {
    WeightedGraph graph{
            {{1, 2}, {2, 3}},
            {{3, 5}},
            {{4, 2}},
            {},
            {}
    };
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    TreeSparseTable sparseTable(graph, operation, std::numeric_limits<int64_t>::max());
    assert(sparseTable.query(1, 2) == 2);
    assert(sparseTable.query(3, 4) == 2);
}

void test_from_task_2() {
    WeightedGraph graph{
            {{1, 1}, {2, 2}},
            {{3, 3}},
            {{4, 4}},
            {},
            {}
    };
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    TreeSparseTable sparseTable(graph, operation, std::numeric_limits<int64_t>::max());
    assert(sparseTable.query(0, 3) == 1);
    assert(sparseTable.query(2, 1) == 1);
}

void run_all_tests() {
    test_from_task_1();
    test_from_task_2();
}

// Конец тестов

int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);

    if (argc > 1) {
        if (std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        }
    }

    // Решение задачи
    std::ifstream fin("minonpath.in");
    size_t n{0};
    fin >> n;
    WeightedGraph graph(n);
    for (size_t i = 1; i < n; ++i) {
        size_t p{0};
        int64_t w{0};
        fin >> p >> w;
        graph[p - 1].emplace_back(i, w);
    }
    const int64_t &(*operation)(const int64_t &, const int64_t &) = std::min<int64_t>;
    TreeSparseTable sparseTable(graph, operation, std::numeric_limits<int64_t>::max());
    size_t m{0};
    fin >> m;
    std::ofstream fout("minonpath.out");
    for (size_t i = 0; i < m; ++i) {
        size_t u{0}, v{0};
        fin >> u >> v;
        fout << sparseTable.query(u - 1, v - 1) << "\n";
    }
    fin.close();
    fout.close();

    return 0;
}