/*
 * D. Ненокку
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Очень известный автор не менее известной книги решил написать продолжение своего произведения.
 * Он писал все свои книги на компьютере, подключенном к интернету.
 * Из-за такой неосторожности мальчику Ненокку удалось получить доступ к еще ненаписанной книге.
 * Каждый вечер мальчик залазил на компьютер писателя и записывал на свой компьютер новые записи.
 * Ненокку, записав на свой компьютер очередную главу, заинтересовался, а использовал ли хоть раз писатель слово "книга".
 * Но он не любит читать книги (он лучше полазает в интернете), и поэтому он просит вас узнать есть ли то
 * или иное слово в тексте произведения. Но естественно его интересует не только одно слово, а достаточно много.
 *
 * Входные данные
 * В каждой строчке входного файла записано одна из двух записей.
 *      ? <слово> (<слово> - это набор не более 50 латинских символов);
 *      A <текст> (<текст> - это набор не более 105 латинских символов).
 * 1 означает просьбу проверить существование подстроки <слово> в произведение.
 * 2 означает добавление в произведение <текст>.
 * Писатель только начал работать над произведением, поэтому он не мог написать более 10^5 символов.
 * А входной файл содержит не более 15 мегабайт информации.
 *
 * Выходные данные
 * Выведите на каждую строчку типа 1 "YES", если существует подстрока <слово>, и "NO" в противном случае.
 * Не следует различать регистр букв.
 *
 * Пример
 * Входные данные
 * ? love
 * ? is
 * A Loveis
 * ? love
 * ? WHO
 * A Whoareyou
 * ? is
 * Выходные данные
 * NO
 * NO
 * YES
 * NO
 * YES
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
 *
 * @param s Приводит все символы строки в нижний регистр.
 * @return Строка, в которой все символы в нижнем регистре.
 */
std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); }
    );
    return s;
}



// Начало тестов

void test_from_task() {
    SuffixTree suffix_tree("");
    assert(!suffix_tree.has_substring("love"));
    assert(!suffix_tree.has_substring("is"));
    suffix_tree.add_text("loveis");
    assert(suffix_tree.has_substring("love"));
    assert(!suffix_tree.has_substring("who"));
    suffix_tree.add_text("whoareyou");
    assert(suffix_tree.has_substring("is"));
}

void test_border() {
    SuffixTree suffix_tree("");
    assert(!suffix_tree.has_substring("love"));
    assert(!suffix_tree.has_substring("is"));
    suffix_tree.add_text("loveis");
    assert(suffix_tree.has_substring("love"));
    assert(!suffix_tree.has_substring("who"));
    suffix_tree.add_text("whoareyou");
    assert(suffix_tree.has_substring("is"));

    assert(suffix_tree.has_substring("who"));
    assert(suffix_tree.has_substring("iswho"));
    assert(suffix_tree.has_substring("sw"));
    assert(suffix_tree.has_substring("s"));
}

void test_repeate() {
    SuffixTree suffix_tree("");
    assert(!suffix_tree.has_substring("love"));
    assert(!suffix_tree.has_substring("is"));

    suffix_tree.add_text("loveis");
    assert(suffix_tree.has_substring("love"));
    assert(suffix_tree.has_substring("is"));

    suffix_tree.add_text("loveis");
    assert(suffix_tree.has_substring("love"));
    assert(suffix_tree.has_substring("is"));

    suffix_tree.add_text("whoareyou");
    suffix_tree.add_text("loveis");
    assert(suffix_tree.has_substring("love"));
    assert(suffix_tree.has_substring("is"));
    assert(suffix_tree.has_substring("who"));
}

void test_aaa() {
    SuffixTree suffix_tree("");

    suffix_tree.add_text("aaa");
    assert(suffix_tree.has_substring("a"));
    assert(suffix_tree.has_substring("aa"));
    assert(suffix_tree.has_substring("aaa"));

    suffix_tree.add_text("aaa");
    assert(suffix_tree.has_substring("a"));
    assert(suffix_tree.has_substring("aa"));
    assert(suffix_tree.has_substring("aaa"));
    assert(suffix_tree.has_substring("aaaa"));
    assert(suffix_tree.has_substring("aaaaa"));
    assert(suffix_tree.has_substring("aaaaaa"));

    assert(!suffix_tree.has_substring("b"));
    assert(!suffix_tree.has_substring("ab"));
    assert(!suffix_tree.has_substring("aaaab"));
    assert(!suffix_tree.has_substring("ba"));
}

void test_aaa_2() {
    SuffixTree suffix_tree("");

    suffix_tree.add_text("aaa");
    assert(suffix_tree.has_substring("a"));
    assert(suffix_tree.has_substring("aa"));
    assert(suffix_tree.has_substring("aaa"));
    assert(!suffix_tree.has_substring("aaaa"));
}

void test_pr() {
    SuffixTree suffix_tree("");

    suffix_tree.add_text("dghdshgdfgjg");
    assert(suffix_tree.has_substring("shg"));
    assert(suffix_tree.has_substring("gdf"));
    assert(suffix_tree.has_substring("jg"));
    assert(suffix_tree.has_substring("s"));

    suffix_tree.add_text("gjhgkhgjkgfjgfjgfkjhgkyguyut");
    assert(suffix_tree.has_substring("shg"));
    assert(suffix_tree.has_substring("gdf"));
    assert(suffix_tree.has_substring("jg"));
    assert(suffix_tree.has_substring("s"));
    assert(suffix_tree.has_substring("uyu"));
    assert(suffix_tree.has_substring("yut"));
    assert(suffix_tree.has_substring("kjh"));
    assert(suffix_tree.has_substring("y"));
}

void run_all_tests() {
    test_from_task();
    test_border();
    test_repeate();
    test_aaa();
    test_aaa_2();
    test_pr();
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
    SuffixTree tree("");
    std::string command;
    while (std::getline(std::cin, command)) {
        std::string text = to_lower(command.substr(2));
        switch (command[0]) {
            case '?': {
                auto has = tree.has_substring(text);
                if (has) {
                    std::cout << "YES\n";
                } else {
                    std::cout << "NO\n";
                }
                break;
            }
            case 'A':
                tree.add_text(text);
                break;
            default:
                assert(false);
                break;
        }
    }


    return 0;
}