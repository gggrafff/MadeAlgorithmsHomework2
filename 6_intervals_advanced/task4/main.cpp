/*
 * D. LCA
 * ограничение по времени на тест 5 секунд
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дано подвешенное дерево с корнем в первой вершине.
 * Вам нужно ответить на m запросов вида "найти LCA двух вершин".
 * LCA вершин u и v в подвешенном дереве — это наиболее удалённая от корня дерева вершина,
 * лежащая на обоих путях от u и v до корня.
 *
 * Входные данные
 * В первой строке задано целое число n — число вершин в дереве (1≤n≤2⋅10^5).
 * В следующих n−1 строках записано одно целое число x. Число x на строке i означает, что x — предок вершины i(x<i).
 * Затем дано число m.
 * Далее заданы m (0≤m≤5⋅105) запросов вида (u,v) — найти LCA двух вершин u и v (1≤u,v≤n; u≠v).
 *
 * Выходные данные
 * Для каждого запроса выведите LCA двух вершин на отдельной строке.
 *
 * Примеры
 * Входные данные
 * 5
 * 1
 * 1
 * 2
 * 3
 * 2
 * 2 3
 * 4 5
 * Выходные данные
 * 1
 * 1
 *
 * Входные данные
 * 5
 * 1
 * 1
 * 2
 * 2
 * 3
 * 4 5
 * 4 2
 * 3 5
 * Выходные данные
 * 2
 * 2
 * 1
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

/**
 * @brief Представим граф списками смежности вершин.
 */
using Graph = std::vector<std::vector<size_t>>;  // списки смежности

/**
 * @brief Ищет lca в дереве.
 */
class LcaFinder {
public:
    /**
     * @brief Производит необходимую предобработку для дальнейшего поиска lca. Первая в списке вершина считается корнем.
     * @param graph Списки смежности графа.
     */
    explicit LcaFinder(const Graph &graph) : graph_(graph), parents_(graph.size(), 0), depth_(graph.size(), 0) {
        std::vector<bool> visited_(graph.size(), false);
        dfs(0, visited_);  // Подсчёт глубин вершин и запоминание родителей

        // Заполним массив jmp методом ДП
        jmp_.resize(graph.size(), std::vector<size_t>(round_log2(graph.size()), 0));
        for (size_t v = 0; v < jmp_.size(); ++v) {
            jmp_[v][0] = parents_[v];
        }
        for (size_t k = 1; k < jmp_.front().size(); ++k) {
            for (size_t v = 0; v < jmp_.size(); ++v) {
                jmp_[v][k] = jmp_[jmp_[v][k - 1]][k - 1];
            }
        }
    }

    /**
     * @brief Запрос поиска lca двух вершин.
     * @param u Индекс первой вершины.
     * @param v Индекс второй вершины.
     * @return Индекс вершины, являющейся lca двух вершин.
     */
    size_t lca(size_t u, size_t v) {
        if (u == v)  // если индексы равны
        {
            return u;  // любая вершина является lca с самой собой
        }
        if (depth_[u] < depth_[v]) {
            std::swap(u, v);
        }
        // Выравниваем высоту вершин
        auto delta_depth = depth_[u] - depth_[v];
        for (int64_t k = jmp_.front().size() - 1; k >= 0; --k) {
            if (delta_depth >= (1 << k)) {
                u = jmp_[u][k];
                delta_depth -= (1 << k);
                assert(delta_depth >= 0);
            }
        }
        if (u == v)  // если после выравнивания высоты оказались в одной вершине
        {
            return u;
        }
        // Поднимаемся от обеих вершин вверх, пока не дойдём до lca
        for (int64_t k = jmp_.front().size() - 1; k >= 0; --k) {
            auto u_top = jmp_[u][k];
            auto v_top = jmp_[v][k];
            if (u_top != v_top)  // делаем прыжок только если после прыжка окажемся ниже lca
            {
                u = u_top;
                v = v_top;
            }
        }
        return jmp_[u][0];  // возвращаем lca
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
            if (!visited[v]) {
                parents_[v] = u;
                depth_[v] = depth_[u] + 1;
                dfs(v, visited);
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

    Graph graph_;
    std::vector<size_t> parents_;  // родители вершин
    std::vector<size_t> depth_;  // глубины вершин
    std::vector<std::vector<size_t>> jmp_;  // jmp[u][k] - куда мы попадём, прыгнув вверх из вершины v на 2^k рёбер.
};

// Начало тестов

void test_from_task_1() {
    Graph graph{
            {1, 2},
            {3},
            {4},
            {},
            {}
    };
    LcaFinder lcaFinder(graph);
    assert(lcaFinder.lca(1, 2) == 0);
    assert(lcaFinder.lca(3, 4) == 0);
}

void test_from_task_2() {
    Graph graph{
            {1, 2},
            {3, 4},
            {},
            {},
            {}
    };
    LcaFinder lcaFinder(graph);
    assert(lcaFinder.lca(3, 4) == 1);
    assert(lcaFinder.lca(3, 1) == 1);
    assert(lcaFinder.lca(2, 4) == 0);
}

void run_all_tests() {
    test_from_task_1();
    test_from_task_2();
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
    size_t n{0};
    std::cin >> n;
    Graph graph(n);
    for (size_t i = 1; i < n; ++i) {
        size_t p{0};
        std::cin >> p;
        graph[p - 1].push_back(i);
    }
    LcaFinder lcaFinder(graph);
    size_t m{0};
    std::cin >> m;
    for (size_t i = 0; i < m; ++i) {
        size_t u{0}, v{0};
        std::cin >> u >> v;
        std::cout << lcaFinder.lca(u - 1, v - 1) + 1 << "\n";
    }

    return 0;
}