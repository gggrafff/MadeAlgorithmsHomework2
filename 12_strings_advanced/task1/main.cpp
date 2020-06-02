/*
 * A. Суффиксное дерево
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Постройте суффиксное дерево для заданной строки s.
 *
 * Входные данные
 * Первая строка входного файла содержит строку s (1 ≤ |s| ≤ 100 000). Строка состоит из строчных латинских букв.
 *
 * Выходные данные
 * В первой строке выходного файла выведите два натуральных числа n и m, разделенных пробелом — число вершин и ребер
 * в суффиксном дереве соответственно.
 * В следующих m строках выведите описания ребер в формате <родитель> <потомок> < l> < r>.
 * Эта запись означает, что на ребре написана строка s[l..r], при этом значение l должно быть минимально возможным.
 * Корнем дерева должна быть вершина с номером 1.
 * Вершины должны быть занумерованы натуральными числами, не превышающими n.
 *
 * Пример
 * Входные данные
 * ababb
 *
 * Выходные данные
 * 7 6
 * 1 4 1 2
 * 1 6 2 2
 * 4 2 3 5
 * 4 5 5 5
 * 6 3 3 5
 * 6 7 5 5
 */

/*
 * Подробнее о суффиксном дереве:
 * https://ru.wikipedia.org/wiki/Суффиксное_дерево
 * https://neerc.ifmo.ru/wiki/index.php?title=Сжатое_суффиксное_дерево
 * https://codeforces.com/blog/entry/11337
 * https://codeforces.com/blog/entry/16780
 * https://stackoverflow.com/questions/9452701/ukkonens-suffix-tree-algorithm-in-plain-english
 * http://neerc.ifmo.ru/wiki/index.php?title=Алгоритм_Укконена
 * http://yury.name/internet/01ianote.pdf
 */


#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>
#include <utility>
#include <map>

/**
 * @brief Класс для построения и использования суффиксных деревьев.
 * @details Суффиксным деревом будем называть суффиксный бор, сжатый таким образом,
 * @details что вершина присутствует в дереве в том и только том случае,
 * @details если в суффиксном боре у неё было больше одного сына или если она отвечает одному из суффиксов строки.
 */
class SuffixTree {
public:
    /**
     * @brief Конструцирует суффиксное дерево для заданной строки.
     * @param original Строка, для которой строим дерево.
     */
    explicit SuffixTree(const std::string &original) {
        nodes_.emplace_back();
        for (char c : original) {
            add_letter(c);
        }
    }

    /**
     * @brief Печать информаии о дереве в поток вывода.
     * @param stream Поток вывода.
     */
    void print_tree(std::ostream &stream = std::cout) {
        stream << get_nodes_count() << " " << get_edges_count() << "\n";
        for (size_t i = 0; i < nodes_.size(); ++i) {
            for (auto &edge: nodes_[i].edges) {
                auto right = std::min(edge.second.first_position + edge.second.length,
                                      static_cast<uint64_t>(original_.size()));
                stream <<
                       i + 1 << " " <<
                       edge.second.to + 1 << " " <<
                       edge.second.first_position + 1 << " " <<
                       right <<
                       "\n";
            }
        }
    }

    /**
     * @brief Получить количество вершин дерева.
     * @return Количество вершин.
     */
    size_t get_nodes_count() {
        return nodes_.size();
    }

    /**
     * @brief Получить количество рёбер дерева.
     * @return Количество рёбер.
     */
    size_t get_edges_count() {
        return nodes_.size() - 1;
    }

    /**
     * @brief Подсчёт количества различных подстрок в оригинальной строке.
     * @return Количество различных подстрок.
     */
    uint64_t count_different_substrings() {
        uint64_t result = 0;
        for (auto &node : nodes_) {
            for (auto &edge: node.edges) {
                auto count = std::min(static_cast<uint64_t>(original_.size() - edge.second.first_position),
                                      edge.second.length);
                result += count;
            }
        }
        return result;
    }

private:
    /**
     * @brief Ребро дерева.
     */
    struct Edge {
        size_t to{0};  // Индекс вершины, в которую ведёт ребро.
        size_t first_position{0};  // Первая позиция вхождения ребра в строку.
        /**
         * @brief Длина ребра.
         * @details Длину рёбер, ведущих в листья будем по-умолчанию считать равной inf.
         * @details Таким образом мы можем быть уверены, что в любой момент времени рёбра в листья идут до самого конца строки.
         */
        uint64_t length{std::numeric_limits<uint32_t>::max()};
    };

    /**
     * @brief Узел дерева.
     */
    struct Node {
        /**
         * @brief Суффиксная ссылка.
         * @details Она определена для внутренних вершин дерева.
         * @details Переход по суффиксной ссылке будет вести в вершину, соответствующую той же строке, но без первого символа.
         * @details Для корня суффиксная ссылка не определена.
         */
        size_t link{0};
        /**
         * @brief Рёбра, которые выходят из данной вершины.
         * @details ToDo: write here
         */
        std::map<char, Edge> edges;
    };

    /**
     * @brief Суффикс строки.
     */
    struct Suffix {
        size_t node_index{0};  // Индекс вершины в суффиксном дереве, которая соответствует суффиксу.
        size_t length{0};  // Число символов, которое необходимо пройти от вершины вниз для получения суффикса.
    };

    /**
     * @brief Добавление символа в дерево.
     * @param c Символ.
     */
    void add_letter(char c) {
        original_ += c;
        longest_suffix.length++;
        size_t last = 0;
        /*
         * Если pos = 0, то все суффиксы обработаны, завершаемся. Иначе найдём вершину, после которой находится новый суффикс.
         */
        while (longest_suffix.length > 0) {
            go_edge();
            int symbol_edge = original_[original_.size() - longest_suffix.length];
            auto &edge = nodes_[longest_suffix.node_index].edges[symbol_edge];
            int t = original_[edge.first_position + longest_suffix.length - 1];
            // Пытаемся добавить очередной суффикс. Здесь нас ожидает три варианта.
            if (edge.to == 0) {
                /* Если у нас нет исходящего ребра по интересующему нас символу,
                 * то мы просто создаём новую вершину и подвешиваем её к текущей.
                 */
                nodes_.emplace_back();
                edge = Edge{nodes_.size() - 1, original_.size() - longest_suffix.length};

                nodes_[last].link = longest_suffix.node_index;
                last = 0;
            } else if (t == c) {
                /*
                 * Если ребро есть и суффикс, который мы хотим добавить целиком лежит на нём,
                 * завершаем свою работу — этот и дальнейшие суффиксы не являются уникальными.
                 */
                nodes_[last].link = longest_suffix.node_index;
                return;
            } else {
                /*
                 * Если ребро есть и суффикс не лежит на нём целиком, это значит,
                 * что нам нужно создать новую вершину посередине этого ребра, к которой подвесить старую
                 * вершину с конца ребра и новую вершину, соответствующую суффиксу.
                 * Стоит заметить, что ребро к новому листу в данный момент будет иметь длину, равную единице.
                 */
                nodes_.emplace_back();
                auto new_edge_1 = Edge{nodes_.size() - 1, edge.first_position, uint64_t(longest_suffix.length - 1)};

                nodes_.emplace_back();
                auto new_edge_2 = Edge{nodes_.size() - 1, original_.size() - 1};

                nodes_[new_edge_1.to].edges[c] = new_edge_2;
                nodes_[new_edge_1.to].edges[t] = edge;
                nodes_[new_edge_1.to].edges[t].first_position += longest_suffix.length - 1;
                nodes_[new_edge_1.to].edges[t].length -= longest_suffix.length - 1;

                edge = new_edge_1;

                /*
                 * На i-ом этапе мы устанавливаем суффиксную ссылку для внутренней вершины, созданной на (i - 1)-ом.
                 * Если мы создаём новую внутреннюю вершину, то суффиксная ссылка будет вести в неё.
                 * В двух остальных случаях суффиксная ссылка ведёт в node самого длинного неуникального суффикса строки.
                 */
                nodes_[last].link = new_edge_1.to;

                last = new_edge_1.to;
            }
            /*
             * Если мы не завершили работу на прошлом шаге, переходим к следующему суффиксу.
             * Если мы находимся в корне, то мы уменьшаем pos на единицу,
             * иначе же мы просто переходим по суффиксной ссылке node = link(node).
             * После этого производим следующую итерацию.
             */
            if (longest_suffix.node_index == 0) {
                longest_suffix.length--;
            } else {
                longest_suffix.node_index = nodes_[longest_suffix.node_index].link;
            }
        }
    }

    /**
     * @brief Перейти в потомка.
     */
    void go_edge() {
        auto &node = longest_suffix.node_index;
        auto &pos = longest_suffix.length;
        // пока pos больше длины ребра, исходящего из данной вершины, будем идти по ребру и вычитать его длину из pos
        while (pos > nodes_[node].edges[original_[original_.size() - pos]].length) {
            auto length_copy = nodes_[node].edges[original_[original_.size() - pos]].length;
            node = nodes_[node].edges[original_[original_.size() - pos]].to;
            pos -= length_copy;
        }
    }

    std::string original_;
    std::vector<Node> nodes_;
    /**
     * @brief На каждом шаге алгоритма будем хранить самый длинный неуникальный суффикс строки.
     * @details При дописывании нового символа мы увеличим length на 1 и добавим все уникальные суффиксы строки,
     * @details которые возникли после добавления нового символа.
     */
    Suffix longest_suffix;
};

// Начало тестов

void test_from_task() {
    std::string original = "ababb";
    SuffixTree suffix_tree(original);
    assert(suffix_tree.get_nodes_count() == 7);
    assert(suffix_tree.get_edges_count() == 6);
    suffix_tree.print_tree();
}

void test_count_substrings_ababb() {
    const std::string original = "ababb";
    SuffixTree suffix_tree(original);
    auto count_substrings = suffix_tree.count_different_substrings();
    assert(count_substrings == 11);
}

void test_count_substrings_aaba() {
    const std::string original = "aaba";
    SuffixTree suffix_tree(original);
    auto count_substrings = suffix_tree.count_different_substrings();
    assert(count_substrings == 8);
}

void test_count_substrings_one_symbol() {
    const std::string original = "a";
    SuffixTree suffix_tree(original);
    auto count_substrings = suffix_tree.count_different_substrings();
    assert(count_substrings == 1);
}

void test_count_substrings_ababba() {
    const std::string original = "ababba";
    SuffixTree suffix_tree(original);
    auto count_substrings = suffix_tree.count_different_substrings();
    assert(count_substrings == 15);
}

void test_count_substrings_mmuc() {
    const std::string original = "mmuc";
    SuffixTree suffix_tree(original);
    auto count_substrings = suffix_tree.count_different_substrings();
    assert(count_substrings == 9);
}

void test_count_substrings_xmnnnuu() {
    const std::string original = "xmnnnuu";
    SuffixTree suffix_tree(original);
    auto count_substrings = suffix_tree.count_different_substrings();
    assert(count_substrings == 24);
}

void test_count_substrings_nnnn() {
    const std::string original = "nnnn";
    SuffixTree suffix_tree(original);
    auto count_substrings = suffix_tree.count_different_substrings();
    assert(count_substrings == 4);
}

void test_count_substrings_banana() {
    const std::string original = "banana";
    SuffixTree suffix_tree(original);
    auto count_substrings = suffix_tree.count_different_substrings();
    assert(count_substrings == 15);
}

void test_count_substrings_abcde() {
    const std::string original = "abcde";
    SuffixTree suffix_tree(original);
    auto count_substrings = suffix_tree.count_different_substrings();
    assert(count_substrings == 15);
}

void test_count_substrings_aaaaa() {
    const std::string original = "aaaaa";
    SuffixTree suffix_tree(original);
    auto count_substrings = suffix_tree.count_different_substrings();
    assert(count_substrings == 5);
}

void test_count_substrings_abacabadabacaba() {
    const std::string original = "abacabadabacaba";
    SuffixTree suffix_tree(original);
    auto count_substrings = suffix_tree.count_different_substrings();
    assert(count_substrings == 85);
}

void run_all_tests() {
    test_from_task();

    // тесты на количество различных подстрок
    test_count_substrings_ababb();
    test_count_substrings_aaba();
    test_count_substrings_one_symbol();
    test_count_substrings_ababba();
    test_count_substrings_mmuc();
    test_count_substrings_xmnnnuu();
    test_count_substrings_nnnn();
    test_count_substrings_banana();
    test_count_substrings_abcde();
    test_count_substrings_aaaaa();
    test_count_substrings_abacabadabacaba();
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
    std::string original;
    std::cin >> original;
    SuffixTree suffix_tree(original);
    suffix_tree.print_tree();

    return 0;
}
