/*
 * D. Автоматное программирование
 * ограничение по времени на тест 5 секунд
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * В один замечательный день в компанию «X» завезли k автоматов. И не простых автоматов, а автоматов-программистов!
 * Это был последний неудачный шаг перед переходом на андроидов-программистов, но это уже совсем другая история.
 *
 * В компании сейчас n задач, для каждой из которых известно
 * время начала ее выполнения si, длительность ее выполнения ti и прибыль компании от ее завершения ci.
 * Любой автомат может выполнять любую задачу, ровно одну в один момент времени.
 * Если автомат начал выполнять задачу, то он занят все моменты времени с si по si+ti−1 включительно и
 * не может переключиться на другую задачу.
 *
 * Вам требуется выбрать набор задач, которые можно выполнить с помощью этих k автоматов и
 * который принесет максимальную суммарную прибыль.
 *
 * Входные данные
 * В первой строке записаны два целых числа n и k (1≤n≤1000, 1≤k≤50) — количество задач и количество автоматов, соответственно.
 * В следующих n строках через пробелы записаны тройки целых чисел si,ti,ci (1≤si,ti≤109, 1≤ci≤106),
 * si — время начала выполнения i-го задания, ti — длительность i-го задания, а ci — прибыль от его выполнения.
 *
 * Выходные данные
 * Выведите n целых чисел x1,x2,…,xn. Число xi должно быть равно 1, если задачу i следует выполнить, и 0 в противном случае.
 * Если оптимальных решений несколько, то выведите любое из них.
 *
 * Примеры
 * Входные данные
 * 3 1
 * 2 7 5
 * 1 3 3
 * 4 1 3
 * Выходные данные
 * 0 1 1
 *
 * Входные данные
 * 5 2
 * 1 5 4
 * 1 4 5
 * 1 3 2
 * 4 1 2
 * 5 6 1
 * Выходные данные
 * 1 1 0 0 1
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


using Schedule = std::vector<bool>;  // нужно ли выполнять i-ю задачу

struct Task {
    size_t index{0};  // порядковый номер для восстановления порядка в ответе
    uint64_t start{0};  // время начала
    uint64_t duration{0};  // длительность задачи
    uint64_t profit{0};  // прибыль от выполнения
};

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
    void
    add_directed_edge(size_t from, size_t to, int64_t capacity, int64_t cost, int64_t flow = 0, int64_t index = 0) {
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
     * @brief Ищет максимальный поток минимальной стоимости.
     * @details Модификация алгоритма Форда—Фалкерсона.
     * @details На каждом шаге выбирается увеличивающий путь минимальной цены.
     * @details Для выбора пути используется алгоритм Дейкстры с использованием потенциалов Джонсона.
     * @details https://neerc.ifmo.ru/wiki/index.php?title=Использование_потенциалов_Джонсона_при_поиске_потока_минимальной_стоимости
     * @return Минимальная стоимость максимального потока.
     */
    int64_t find_max_flow_min_cost_dijkstra_johnson() {
        // Найдём потенциалы Джонсона
        // Беллмана-Форда алгоритм. Методом динамического программирования.
        const int64_t inf = std::numeric_limits<int64_t>::max();

        std::vector<int64_t> potentials(nodes.size(), inf);  // расстояние от истока до вершины i
        potentials[source_index] = 0;
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
                    if ((potentials[ni] < inf) && (edge.capacity - edge.flow > 0)) {
                        if (potentials[edge.to] > potentials[ni] + edge.cost) {
                            potentials[edge.to] = potentials[ni] + edge.cost;
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

        assert (potentials[sink_index] != inf);

        while (true) {
            for (auto &node: nodes) {
                node.visited = false;
            }
            if (!find_path_residual(source_index, sink_index)) {
                // Считаем стоимость
                return compute_cost_flow();
            }

            // Найдём путь и поток, который можно по нему запустить.
            // Запустим по пути дополнительный поток
            add_flow(parents);

            // Дейкстры алгоритм.

            std::vector<int64_t> distance(nodes.size(), inf);  // расстояние от истока до вершины i
            distance[source_index] = 0;
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
                    auto weight = edge.cost + potentials[v.value()] - potentials[edge.to];
                    if (edge.capacity - edge.flow > 0) {
                        if (distance[edge.to] > distance[v.value()] + weight) {
                            distance[edge.to] = distance[v.value()] + weight;
                            nodes[edge.to].visited = false;
                            parents[edge.to] = {v.value(), ei};
                        }
                    }
                }
            }

            for (size_t i = 0; i < distance.size(); ++i) {
                potentials[i] += distance[i];  // обновим потенциалы
            }

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
     * @param parents Массив родителей при поиске расстояний для восстановления пути.
     */
    void add_flow(const std::vector<std::pair<size_t, size_t>> &parents) {
        const int64_t inf = std::numeric_limits<int64_t>::max();

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
 * Класс-обёртка, помогающий свести данную задачу к задаче поиска макс. потока мин.стоимости.
 */
class TaskScheduler {
public:
    /**
     * @brief Создаёт сеть нужной конфигурации.
     * @param count_workers Количество работников.
     * @param tasks Массив задач.
     */
    TaskScheduler(size_t count_workers, std::vector<Task> &tasks) :
            count_workers_(count_workers),
            count_tasks_(tasks.size()) {
        std::sort(tasks.begin(), tasks.end(), [](auto &lhs, auto &rhs) {
            return lhs.start < rhs.start;
        });
        net_.add_nodes(count_tasks_ * 2 + 2);

        net_.set_source_index(0);
        net_.set_sink_index(1);

        for (size_t i = 0; i < tasks.size(); ++i) {
            // раздваивающие рёбра со стоимостью
            net_.add_directed_edge(2 + 2 * i, 2 + 2 * i + 1, 1, -static_cast<int64_t>(tasks[i].profit), 0,
                                   tasks[i].index + 1);
        }

        const int64_t inf = std::numeric_limits<int64_t>::max();
        for (size_t i = 0; i < tasks.size() - 1; ++i) {
            net_.add_directed_edge(2 + 2 * i, 2 + 2 * (i + 1), inf, 0);  // пропустить задачу
            auto next_task = std::lower_bound(
                    std::next(tasks.begin(), i),
                    tasks.end(),
                    tasks[i].start + tasks[i].duration,
                    [](auto &lhs, auto &rhs) {
                        return lhs.start < rhs;
                    });
            if (next_task != tasks.end()) {
                auto next_task_index = std::distance(tasks.begin(), next_task);
                assert(next_task_index > 0);
                net_.add_directed_edge(2 + 2 * i + 1, 2 + 2 * static_cast<size_t>(next_task_index), 1,
                                       0);  // следующая задача после выполнения текущей
            } else {
                net_.add_directed_edge(2 + 2 * i + 1, 1, 1,
                                       0);  // если после задачи нельзя начать новую, то уходим в сток
            }
        }

        net_.add_directed_edge(0, 2, count_workers, 0);  // из истока в первую задачу
        net_.add_directed_edge(2 + (count_tasks_ - 1) * 2, 1, count_workers,
                               0);  // последнюю задачу можно пропустить и уйти в сток
        net_.add_directed_edge(2 + (count_tasks_ - 1) * 2 + 1, 1, count_workers,
                               0);  // последнюю задачу можно выполнить и уйти в сток
    }

    ~TaskScheduler() = default;


    /**
     * @brief Найти, какие задачи стоит выполнить.
     * @return Массив, где i-й элемент true, если i-ю задачу выполняем.
     */
    Schedule find_schedule() {
        Schedule result(count_tasks_, false);
        net_.find_max_flow_min_cost_dijkstra_johnson();
        for (size_t i = 0; i < count_tasks_; ++i) {
            auto task_edge = net_.nodes[2 + 2 * i].edges[0];
            if (task_edge.flow > 0) {
                result[task_edge.index - 1] = true;
            }
        }
        return result;
    }

private:
    FlowNetwork net_;
    size_t count_tasks_{0};
    size_t count_workers_{0};
};

// Начало тестов

void test_from_task_1() {
    size_t n = 3;
    size_t k = 1;
    std::vector<Task> tasks{
            {0, 2, 7, 5},
            {1, 1, 3, 3},
            {2, 4, 1, 3}
    };
    TaskScheduler scheduler(k, tasks);
    auto result = scheduler.find_schedule();
    assert((result == Schedule{0, 1, 1}));
}

void test_from_task_2() {
    size_t n = 5;
    size_t k = 2;
    std::vector<Task> tasks{
            {0, 1, 5, 4},
            {1, 1, 4, 5},
            {2, 1, 3, 2},
            {3, 4, 1, 2},
            {4, 5, 6, 1},
    };
    TaskScheduler scheduler(k, tasks);
    auto result = scheduler.find_schedule();
    assert((result == Schedule{1, 1, 0, 0, 1}));
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

    /**
     * Для решения задачи построим граф следующим образом:
     * Вершины графа - задачи, которые нужно выполнить.
     * Для удобства отсортируем задачи по возрастанию времени начала.
     * Из каждой задачи есть два рёбра:
     *      К следущющей по порядку задаче (пропуск текущего задания).
     *      К следующей по времени начала задаче после конца текущей (выполнение текущего задания).
     * Единичный поток в графе - это план работ для одного исполнителя.
     * Работы исполнителей не должны пересекаться, для этого раздвоим вершины,
     *      а раздваивающему ребру присвоим стоимость -ci и ёмкость 1.
     * Рёбра переходов от одной задачи к другой имеют нулевую стоимость и бесконечную ёмкость.
     * Есть сток и исток (отедльные вершины), которые соединены рёбрами с первой и последней задачами соответственно.
     * Рёбра от истока и к стоку имеют емкость равную количеству работников.
     * Находим максимальный поток минимальной стоимости с помощью Дейкстры с перевзвешиванием рёбер (потенциалы Джонсона).
     */

    // Решение задачи
    size_t n{0};
    size_t k{0};
    std::cin >> n >> k;
    std::vector<Task> tasks;
    for (size_t i = 0; i < n; ++i) {
        uint64_t s, t, c;
        std::cin >> s >> t >> c;
        tasks.push_back({i, s, t, c});
    }
    TaskScheduler scheduler(k, tasks);
    auto result = scheduler.find_schedule();
    for (auto task: result) {
        std::cout << static_cast<int>(task) << " ";
    }

    return 0;
}