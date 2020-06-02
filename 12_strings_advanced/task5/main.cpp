/*
 * E. Суффиксный массив
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 512 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Постройте суффиксный массив для заданной строки s, для каждых двух соседних суффиксов найдите длину максимального общего префикса.
 * Входные данные
 * Первая строка входного файла содержит строку s (1 ≤ |s| ≤ 400 000). Строка состоит из строчных латинских букв.
 *
 * Выходные данные
 * В первой строке выведите |s| различных чисел — номера первых символов суффиксов строки s так,
 * чтобы соответствующие суффиксы были упорядочены в лексикографически возрастающем порядке.
 * Во второй строке выведите |s| - 1 чисел — длины наибольших общих префиксов.
 *
 * Пример
 * Входные данные
 * ababb
 * Выходные данные
 * 1 3 5 2 4
 * 2 0 1 1
 */

/*
 * Подробнее про построение суффиксного массива из суффиксного дерева:
 * https://neerc.ifmo.ru/wiki/index.php?title=Сжатое_суффиксное_дерево
 */

/*
 * Взял класс суффиксного массива из прошлой домашней работы и класс суффиксного дерева из предыдущих задач текущей
 * домашней работы. Добавил к классу массива конструктор, принимающий дерево и запускающий его обход.
 * Остальные методы не менял, но и удалять не стал.
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
#include <string>

class SuffixArray;

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
     * @param with_terminator True, если в конец строки нужно добавить служебный символ. Нужно для подсчёта подстрок.
     */
    explicit SuffixTree(const std::string &original, bool with_terminator = false) {
        nodes_.emplace_back();
        for (char c : original) {
            add_letter(c);
        }
        if (with_terminator) {
            add_letter(TERMINATOR);  // добавляем служебный символ в конец, чтобы все суффиксы заканчивались в листе
            count_leaves(0);  // подсчитываем листья во всех поддеревьях дерева
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

    /**
     * @brief Подсчёт количества вхождений подстроки в исходную строку.
     * @param substring Подстрока.
     * @return Количество вхождений подстроки в строку.
     */
    size_t count_substring(const std::string &substring) {
        if (original_.back() != TERMINATOR) {  // проверяем, что использовался служебный символ и были подсчитаны листья
            add_letter(TERMINATOR);  // добавляем служебный символ в конец, чтобы все суффиксы заканчивались в листе
            count_leaves(0);  // подсчитываем листья во всех поддеревьях дерева
        }

        /*
         * Будем читать шаблон вдоль дерева от корня.
         * Если в какой-то момент не сможем прочитать следующую букву шаблона,
         * значит шаблон ни разу не встречался в тексте.
         * Допустим, что он встречался, тогда,прочитав его, приходим в вершину v или останавливаемся на ребре.
         * В случае остановки на ребре пройдем дальше от корня до первой вершины v.
         * Далее посчитаем листья-потомки вершины v. Это будет количество вхождений.
         */

        size_t current_node_idx = 0;  // начинаем с корня
        size_t i = 0;
        while (i < substring.size()) {  // пока не проверим все символы искомой подстроки
            auto edge_iter = nodes_[current_node_idx].edges.find(
                    substring[i]);  // ищем ребро, соответствующее следующей букве шаблона
            if (edge_iter == nodes_[current_node_idx].edges.end()) {
                return 0;  // подстрока не встречалась в тексте
            } else {
                if (edge_iter->second.length > 1) {  // если ребро соответствует не одной букве, а нескольким
                    // определим количество символов, которые нужно сравнить
                    auto cmp_length = std::min(edge_iter->second.length, static_cast<uint64_t>(substring.size() - i));
                    // сравниваем символы, соответствующие ребру с частью подстроки
                    auto comparison = original_.compare(edge_iter->second.first_position, cmp_length, substring, i,
                                                        cmp_length);
                    if (comparison != 0) {
                        return 0;  // подстрока не встречалась в тексте
                    } else {  // символы совпали
                        i += cmp_length;  // мы проверили несколько символов
                        current_node_idx = edge_iter->second.to;  // проходим по ребру к следующей вершине
                    }
                } else {
                    i += 1;  // мы проверили один символ
                    current_node_idx = edge_iter->second.to;  // проходим по ребру к следующей вершине
                }
            }
        }
        return nodes_[current_node_idx].count_leaves;  // возвращаем количество листьев-потомков
    }

    /**
     * @brief Проверяет, встречается ли подстрока в тексте.
     * @param substring Подстрока.
     * @return true, если подстрока в тексте встречалась. Иначе - false.
     */
    bool has_substring(const std::string &substring) const {
        /*
         * Будем читать шаблон вдоль дерева от корня.
         * Если в какой-то момент не сможем прочитать следующую букву шаблона,
         * значит шаблон ни разу не встречался в тексте.
         */

        size_t current_node_idx = 0;
        size_t i = 0;
        while (i < substring.size()) {  // пока не проверим все символы искомой подстроки
            auto edge_iter = nodes_[current_node_idx].edges.find(
                    substring[i]);  // ищем ребро, соответствующее следующей букве шаблона
            if (edge_iter == nodes_[current_node_idx].edges.end()) {
                return false;  // подстрока не встречалась в тексте
            } else {
                if (edge_iter->second.length > 1) {  // если ребро соответствует не одной букве, а нескольким
                    // определим количество символов, которые нужно сравнить
                    auto cmp_length = std::min(edge_iter->second.length, static_cast<uint64_t>(substring.size() - i));
                    // сравниваем символы, соответствующие ребру с частью подстроки
                    auto comparison = original_.compare(edge_iter->second.first_position, cmp_length, substring, i,
                                                        cmp_length);
                    if (comparison != 0) {
                        return false;  // подстрока не встречалась в тексте
                    } else {  // символы совпали
                        i += cmp_length;  // мы проверили несколько символов
                        current_node_idx = edge_iter->second.to;  // проходим по ребру к следующей вершине
                    }
                } else {
                    i += 1;  // мы проверили один символ
                    current_node_idx = edge_iter->second.to;  // проходим по ребру к следующей вершине
                }
            }
        }
        return true;  // подстрока была найдена
    }

    /**
     * @brief Достроить дерево для продолжения строки.
     * @param text Продолжение исходной строки.
     */
    void add_text(const std::string &text) {
        for (char c : text) {
            add_letter(c);
        }
    }

    friend SuffixArray;

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

        size_t count_leaves{0};

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

    /**
     * @brief Подсчитывает количество листьев во всех поддеревьях дерева.
     * @details Обход в глубину.
     * @param node_index Индекс корня поддерева, для которого считаем листья.
     */
    void count_leaves(size_t node_index) {
        size_t current_count = 0;
        for (auto &edge: nodes_[node_index].edges) {
            count_leaves(edge.second.to);
            current_count += nodes_[edge.second.to].count_leaves;
        }
        if (current_count == 0) {
            current_count = 1;
        }
        nodes_[node_index].count_leaves = current_count;
    }

    const char TERMINATOR = '$';

    std::string original_;
    std::vector<Node> nodes_;
    /**
     * @brief На каждом шаге алгоритма будем хранить самый длинный неуникальный суффикс строки.
     * @details При дописывании нового символа мы увеличим length на 1 и добавим все уникальные суффиксы строки,
     * @details которые возникли после добавления нового символа.
     */
    Suffix longest_suffix;
};

/**
 * Класс для построения и использования суффиксного массива строки.
 */
class SuffixArray {
public:
    /**
     * @brief Конструирует суффиксный массив для строки.
     * @param original Строка, для которой строим суффиксный массив.
     */
    explicit SuffixArray(std::string original) : original_(std::move(original)) {
        build_suffix_array();
    }

    bool operator==(const SuffixArray &b) const {
        bool result = true;
        result &= this->original_ == b.original_;
        result &= this->suffixes_ == b.suffixes_;
        if (!this->lcp_neighboring_.empty() && !b.lcp_neighboring_.empty()) {
            result &= this->lcp_neighboring_ == b.lcp_neighboring_;
        }
        return result;
    }

    /**
     * @brief Конструирует суффиксный массив для строки.
     * @param original Строка, для которой строим суффиксный массив.
     */
    explicit SuffixArray(const SuffixTree &suffix_tree) : original_(suffix_tree.original_) {
        /*
         * Пусть к строке дописан специальный символ для сохранения инварианта.
         * Рассмотрим лексикографический по ребрам порядок обхода сжатого суффиксного дерева.
         * Пусть два суффикса имеют общее начало, но различаются в i-ом символе.
         * Первым будет рассмотрено поддерево по ребру с меньшим символом, значит и лист,
         * соответствующий этому суффиксу, будет посещен первым.
         * Тогда суффиксный массив строится из суффиксного дерева обходом в глубину в указанном порядке.
         * Пусть длина строки length, глубина листа в символах depth, тогда номер суффикса i=length−depth.
         *
         * Для заполнения массива lcp нам понадобится вершина minNode, которая будет означать вершину
         * с минимальной глубиной, в которую мы поднимались при переходе между суффиксами.
         * Поскольку мы точно поднимались туда, но не поднимались выше, это будет наименьший общий предок этих узлов.
         * Из этого следует, что у рассматриваемых суффиксов совпадает ровно lcp=minNode.depth символов.
         */
        build_from_tree(suffix_tree);
    }

    /**
     * @brief Конструирует суффиксный массив для конкатенации двух строк.
     * @param first Первая строка, для которой строим суффиксный массив.
     * @param second Вторая строка, для которой строим суффиксный массив.
     */
    explicit SuffixArray(const std::string &first, const std::string &second) :
            original_(first + static_cast<char>(terminator_ + 1) + second) {
        build_suffix_array();
    }

    /**
     * @brief Печать информаии о суффиксном массиве в поток вывода.
     * @param stream Поток вывода.
     * @param from С какого индекса начать печатать (для выбора, печатать ли служебный разделитель).
     */
    void print_suffixes_and_lcp(std::ostream &stream = std::cout) {
        for (size_t i = 1; i < suffixes_.size(); ++i) {
            stream << suffixes_[i] + 1 << " ";
        }
        stream << std::endl;
        if (!lcp_neighboring_.empty()) {
            for (size_t i = 1; i < lcp_neighboring_.size(); ++i) {
                stream << lcp_neighboring_[i] << " ";
            }
        }
        stream << std::endl;
    }

    /**
     * @brief Подсчитывает lcp для соседних суффиксов в суффиксном массиве.
     * @details Алгоритм Касаи, Аримуры, Арикавы, Ли, Парка
     */
    void build_lcp_neighboring() {
        /*
         * Нам не нужно сравнивать все символы, когда мы вычисляем LCP между суффиксом Si и его соседним суффиксом
         * в массиве Suf^−1. Чтобы вычислить LCP всех соседних суффиксов в массиве Suf^−1 эффективно,
         * будем рассматривать суффиксы по порядку начиная с S1 и заканчивая Sn.
         */
        auto len = original_.size();
        lcp_neighboring_.resize(len, 0);
        std::vector<size_t> pos(len);   // pos[] — массив, обратный массиву suf
        for (size_t i = 0; i < len; ++i) {
            pos[suffixes_[i]] = i;
        }
        size_t k = 0;
        for (size_t i = 0; i < len; ++i) {
            if (k > 0) {
                k -= 1;
            }
            if (pos[i] == len - 1) {
                lcp_neighboring_[len - 1] = 0;
                k = 0;
                continue;
            } else {
                auto j = suffixes_[pos[i] + 1];
                while ((std::max(i + k, j + k) < len) && (original_[i + k] == original_[j + k])) {
                    k += 1;
                }
                lcp_neighboring_[pos[i]] = k;
            }
        }
        lcp_neighboring_.erase(std::prev(lcp_neighboring_.end()));
    }

    /**
     * @brief Получить суффиксный массив.
     * @return Суффиксный массив (массив индексов).
     */
    std::vector<size_t> get_suffixes() const {
        return suffixes_;
    }

    /**
     * @brief Получить lcp всех соседних суффиксов.
     * @return Массив со значениями lcp для соседних суффиксов.
     */
    std::vector<size_t> get_lcp_neighboring() {
        if (lcp_neighboring_.empty()) {
            build_lcp_neighboring();
        }
        return lcp_neighboring_;
    }

    /**
     * @brief Поиск подстроки в исходной строке с помощью суффиксного массива.
     * @param substring Искомая подстрока.
     * @return Индекс начала подстроки в строке, если она найдена.
     */
    std::optional<size_t> find_substring(const std::string &substring) {
        /*
         * Заметим, что любая подстрока — это префикс какого-то суффикса.
         * Давайте рассмотрим суффиксный массив строки s.
         * Пусть длина строки p равна |p|.
         * Отрежем от каждого суффикса первые |p| символов, получим все подстроки длины |p| в отсортированном порядке.
         * Теперь среди них нам нужно найти строку p. Это можно сделать обычным бинарным поиском.
         * Время работы бинарного поиска будет O(|p|logn), потому что каждое сравнение строк работает за время O(|p|).
         */
        auto &original = original_;
        auto found = std::lower_bound(suffixes_.begin(), suffixes_.end(), substring, [&original](auto &suf, auto &rhs) {
            return (original.compare(suf, std::string::npos, rhs) < 0);
        });
        if (found == suffixes_.end()) {
            return std::nullopt;
        }
        if (original.substr(*found, substring.size()) != substring) {
            return std::nullopt;
        }
        return *found;
    }

    /**
     * @brief Подсчёт количества различных подстрок в оригинальной строке.
     * @return Количество различных подстрок.
     */
    uint64_t count_different_substrings() {
        /*
         * Вспомним, что любая строка — это префикс какого-то суффикса, таким образом нам нужно посчитать,
         * сколько различных префиксов у суффиксов строки s. Давайте перебирать суффиксы строки s в
         * отсортированном порядке. Посмотрим на очередной суффикс. Давайте посмотрим, сколько новых префиксов
         * он нам добавляет. Общее число префиксов равно длине суффикса. Чтобы узнать, какие из них уже встречались
         * в предыдущих строках, нужно просто посмотреть на LCP этого суффикса с предыдущим. Таким образом, мы
         * можем за O(1) понять, сколько новых префиксов добавляет каждый суффикс. Сложив это вместе, получим ответ.
         */
        if (lcp_neighboring_.empty()) {
            build_lcp_neighboring();
        }
        uint64_t count = original_.size() - 1 - suffixes_[1];
        for (size_t i = 2; i < suffixes_.size(); ++i) {
            count += original_.size() - 1 - suffixes_[i] - lcp_neighboring_[i - 1];
        }
        return count;
    }

    /**
     * @brief Подсчёт количества различных подстрок в оригинальной строке.
     * @return Количество различных подстрок.
     */
    uint64_t count_different_substrings_2() {
        /*
         * M = n * (n − 1) / 2 - sum(lcp[i])
         */
        if (lcp_neighboring_.empty()) {
            build_lcp_neighboring();
        }
        uint64_t n = original_.size();
        uint64_t count = n * (n - 1) / 2;
        for (auto lcp: lcp_neighboring_) {
            count -= lcp;
        }
        return count;
    }

    /**
     * @brief Поиск наибольшей общей подстроки.
     * @details Должен был использоваться конструктор для двух строк.
     * @return Наибольшая общая подстрока (наименьшая лексикографически).
     */
    std::string find_largest_common_substring() {
        if (lcp_neighboring_.empty()) {
            build_lcp_neighboring();
        }
        std::pair<size_t, size_t> lcs{0, 0};  // индекс начала и длина
        size_t max_lcp = 0;
        for (size_t i = 0; i < suffixes_.size() - 1; ++i) {
            // Берём xor: рассматриваем только суффиксы, начинающиеся в разных подстроках
            // В suffixes_[1] хранится суффикс, начинающийся с разделителя между строками.
            // По suffixes_[1] проходит граница строк.
            if ((suffixes_[i] < suffixes_[1]) ^ (suffixes_[i + 1] < suffixes_[1])) {
                if (lcp_neighboring_[i] > max_lcp) {
                    max_lcp = lcp_neighboring_[i];
                    lcs = {suffixes_[i], max_lcp};
                }
            }
        }
        return original_.substr(lcs.first, lcs.second);
    }

private:
    /**
     * @brief Строит суффиксный массив из суффиксного дерева обходом в глубину.
     * @param tree Суффиксное дерево.
     * @param current_node_idx Индекс текущей вершины дерева.
     * @param current_length Глубина текущей вершины в символах.
     */
    void build_from_tree(const SuffixTree &tree, size_t current_node_idx = 0, size_t current_length = 0) {
        if (tree.nodes_[current_node_idx].edges.empty()) {
            suffixes_.push_back(original_.size() - current_length - 1);
            return;
        }
        auto &edges = tree.nodes_[current_node_idx].edges;
        size_t i = 0;
        for (auto &edge: edges) {
            auto length_edge = edge.second.length;
            if (length_edge > original_.size()) {
                length_edge = original_.size() - edge.second.first_position - 1;
            }
            build_from_tree(tree, edge.second.to, current_length + length_edge);
            if (i < edges.size() - 1) {
                lcp_neighboring_.push_back(current_length);
            }
            ++i;
        }
    }

    /* преобразует масив count, так что
     теперь он содержит позиции в массиве suffs с которых
     необходимо вставлять подстроки, начинающиеся с соответствующих символов */
    /**
     * @brief Часть сортировки подсчётом.
     * @brief Из массива количеств объектов формирует массив начал их позиций после сортировки.
     * @details Из массива количеств формирует массив, который содержит позиции в суффиксном массиве,
     * @details с которых необходимо вставлять подстроки, начинающиеся с соответствующих символов
     * @param count Массив количеств объектов (результат первого шага сортировки подсчётом).
     * @return Массив начал позиций объектов после сортировки.
     */
    std::vector<size_t> calc_positions(const std::vector<size_t> &count) {
        std::vector<size_t> result;
        result.push_back(0);
        for (size_t i = 0; i < count.size() - 1; ++i) {
            result.push_back(result.back() + count[i]);
        }
        return result;
    }

    /**
     * @brief Конструирует суффиксный массив.
     */
    void build_suffix_array() {
        // добавим к строке символ, который меньше любого другого символа в строке
        original_ += terminator_;

        // нулевая итерация
        // На нулевой итерации отсортируем циклические подстроки длины 1, т.е. первые символы строк, и разделим
        // их на классы эквивалентности (одинаковые символы должны быть отнесены к одному классу эквивалентности).
        std::vector<size_t> count(alphabet_size, 0);
        for (auto ch: original_) {  // первый шаг сортировки подсчётом
            count[ch] += 1;  // считаем, сколько символов каждого класса (типа) встречается в строке
        }
        // При помощи сортировки подсчетом построим массив,
        // содержащий номера суффиксов, отсортированных в лексикографическом порядке.
        auto positions = calc_positions(count);
        suffixes_.resize(original_.size(), 0);
        for (size_t i = 0; i < original_.size(); ++i) {
            auto position = positions[original_[i]]++;
            assert((position >= 0) && (position < suffixes_.size()));
            suffixes_[position] = i;  // suffixes_ будет хранить индексы начал отсортированных подстрок
        }
        // По этому массиву построим массив классов эквивалентности.
        // Одинаковые символы принадлежат одному классу.
        std::vector<size_t> classes(original_.size());
        size_t classes_number = 0;
        char last_char = terminator_;
        for (auto suf: suffixes_) {
            assert((suf >= 0) && (suf < original_.size()));
            if (original_[suf] != last_char) {
                last_char = original_[suf];
                classes_number += 1;
            }
            assert((suf >= 0) && (suf < classes.size()));
            classes[suf] = classes_number;
        }

        // нулевая итерация завершена
        // сортируем подстроки длиной 2 * cur_len = 2^k
        size_t cur_len = 1;
        while (cur_len <= original_.size()) {
            /*
             * Отсортируем подстроки длины 2^k по данным парам и запишем порядок в массив p.
             * Воспользуемся здесь приёмом, на котором основана цифровая сортировка:
             * отсортируем пары сначала по вторым элементам, а затем по первым (устойчивой сортировкой).
             * Однако вторые элементы уже упорядочены — этот порядок задан в массиве от предыдущей итерации.
             * Тогда, чтобы получить порядок пар по вторым элементам, надо от каждого элемента массива p отнять 2^(k − 1)
             * (p даёт упорядочение подстрок длины 2^(k − 1), и при переходе к строке вдвое большей длины эти подстроки
             * становятся их вторыми половинками, поэтому от позиции второй половинки отнимается длина первой половинки).
             */
            // сортируем по второй половине подстроки
            std::vector<size_t> sorted_by_second(original_.size());
            for (size_t i = 0; i < sorted_by_second.size(); ++i) {
                sorted_by_second[i] = (suffixes_[i] + original_.size() - cur_len) % original_.size();
            }
            // сортируем по первой половине
            // Чтобы произвести устойчивую сортировку по первым элементам пар, применим сортировку подсчетом за O(n).
            count = std::vector<size_t>(classes.size(), 0);
            for (auto by2: sorted_by_second) {
                auto index = classes[by2];
                assert((index >= 0) && (index < count.size()));
                count[index] += 1;
            }
            positions = calc_positions(count);
            for (size_t i = 0; i < sorted_by_second.size(); ++i) {
                auto position = positions[classes[sorted_by_second[i]]]++;
                assert((position >= 0) && (position < suffixes_.size()));
                suffixes_[position] = sorted_by_second[i];
            }
            // Осталось пересчитать номера классов эквивалентности c,
            // пройдя по новой перестановке p и сравнивая соседние элементы (как пары двух чисел).
            std::vector<size_t> new_classes(original_.size());
            classes_number = 0;
            for (size_t i = 1; i < original_.size(); ++i) {
                auto mid1 = (suffixes_[i] + cur_len) % original_.size();
                auto mid2 = (suffixes_[i - 1] + cur_len) % original_.size();

                assert((suffixes_[i] >= 0) && (suffixes_[i] < classes.size()));
                assert((suffixes_[i - 1] >= 0) && (suffixes_[i - 1] < classes.size()));
                assert((mid1 >= 0) && (mid1 < classes.size()));
                assert((mid2 >= 0) && (mid2 < classes.size()));

                if ((classes[suffixes_[i]] != classes[suffixes_[i - 1]]) or (classes[mid1] != classes[mid2])) {
                    classes_number++;
                }
                assert((suffixes_[i] >= 0) && (suffixes_[i] < new_classes.size()));
                new_classes[suffixes_[i]] = classes_number;
            }
            classes = new_classes;
            cur_len *= 2;
        }
    }

    const size_t alphabet_size = 256;
    const char terminator_{'$'};
    std::string original_;
    std::vector<size_t> suffixes_;
    std::vector<size_t> lcp_neighboring_;
};

// Начало тестов

void test_from_task() {
    std::string original = "ababb";
    SuffixTree tree(original, true);
    SuffixArray arr_1(tree);
    SuffixArray arr_2(original);
    arr_2.build_lcp_neighboring();
    assert(arr_1 == arr_2);
}

void test_from_abacabadabacaba() {
    std::string original = "abacabadabacaba";
    SuffixTree tree(original, true);
    SuffixArray arr_1(tree);
    SuffixArray arr_2(original);
    arr_2.build_lcp_neighboring();
    assert(arr_1 == arr_2);
}


void run_all_tests() {
    test_from_task();
    test_from_abacabadabacaba();
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
    SuffixTree tree(original, true);
    SuffixArray arr(tree);
    arr.print_suffixes_and_lcp();

    return 0;
}