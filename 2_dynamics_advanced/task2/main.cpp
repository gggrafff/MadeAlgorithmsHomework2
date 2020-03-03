/*
 * B. Удаление скобок 2.0
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод

 * Дана строка, составленная из круглых, квадратных и фигурных скобок.
 * Определите, какое наименьшее количество символов необходимо удалить из этой строки,
 * чтобы оставшиеся символы образовывали правильную скобочную последовательность.
 *
 * Входные данные
 * Во входном файле записана строка из круглых, квадратных и фигурных скобок.
 * Длина строки не превосходит {100} символов.
 * Выходные данные
 * Выведите строку максимальной длины, являющейся правильной скобочной последовательностью,
 * которую можно получить из исходной строки удалением некоторых символов.
 *
 * Пример
 * Входные данные
 * ([)]
 * Выходные данные
 * []
 */

/*
 * Что храним? dp[i][j] - минимальный ответ на отрезке [i, j]
 * База dp[i][i] = 1
 * Переход dp[i][j] = min(dp[i][k] + dp[k][j], dp[i+1][j-1] (если концы - корректные взаимные скобочки)), k=i..j
 * Порядок по возрастанию длины
 * Ответ dp[0][n]
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <limits>

/**
 * @brief Восстановление ответа. Рекурсиваня функция.
 * @param i Левый конец отрезка.
 * @param j Правый конец отрезка.
 * @param dp Динамика.
 * @param separators Разделители отрезков на подотрезки.
 * @param brackets Исходная строка.
 * @return Наиболее длинная корректная строка со скобками, полученная из исходной удалением символов.
 */
std::string restore_answer(
        size_t i,
        size_t j,
        const std::vector<std::vector<size_t>> &dp,
        const std::vector<std::vector<ssize_t>> &separators,
        const std::string &brackets) {
    assert(j >= i);
    if (dp[i][j] == j - i + 1) {  // если все символы плохие
        return "";
    }
    if (dp[i][j] == 0) {  // если все символы хорошие (строка полностью корректна)
        return brackets.substr(i, j - i + 1);
    }
    if (separators[i][j] == -1) {  // особый случай типа (...)
        return brackets[i] + restore_answer(i + 1, j - 1, dp, separators, brackets) + brackets[j];
    }
    // остальные случаи: отрезок делится на подотрезки
    auto left_part = restore_answer(i, static_cast<size_t>(separators[i][j]), dp, separators, brackets);
    auto right_part = restore_answer(static_cast<size_t>(separators[i][j] + 1), j, dp, separators, brackets);
    return left_part + right_part;
}

/**
 * @brief Поиск наиболее длинной корректной последовательности скобок в строке методом ДП.
 * @param brackets Строка со скобками.
 * @return Наиболее длинная корректная строка со скобками, полученная из исходной удалением символов.
 */
std::string find_longest_valid_brackets(const std::string &brackets) {
    if (brackets.empty()) {
        return "";
    }

    std::vector<std::vector<size_t>> dp(
            brackets.size(),
            std::vector<size_t>(brackets.size(), std::numeric_limits<size_t>::max()));
    std::vector<std::vector<ssize_t>> separators(  // массив разделителей отрезка на подотрезки, из которых он получен
            brackets.size(),
            std::vector<ssize_t>(
                    brackets.size(),
                    std::numeric_limits<ssize_t>::max()));

    for (size_t i = 0; i < brackets.size(); ++i) {
        dp[i][i] = 1;  // база
    }

    for (size_t length = 2; length <= brackets.size(); ++length) {  // по возрастанию длины
        for (size_t i = 0; i <= brackets.size() - length; ++i) {  // итерируемся по левому концу отрезка
            auto j = i + length - 1;  // правый конец отрезка
            assert(j > i);
            for (size_t k = i;
                 k < j; ++k) {  // ищем такое деление отрезка на подотрезки, которое даст наилучший результат
                auto sum = dp[i][k] + dp[k + 1][j];
                if (sum < dp[i][j]) {
                    dp[i][j] = sum;
                    assert(k >= i);
                    separators[i][j] = k;  // запоминаем, как поделили отрезок на подотрезки
                }
            }
            // отдельно рассмотрим случай типа (...), когда корректное выражение корректно вложено в скобочки
            if ((brackets[i] == '(' && brackets[j] == ')') ||
                (brackets[i] == '[' && brackets[j] == ']') ||
                (brackets[i] == '{' && brackets[j] == '}')) {
                assert(j > i);
                if (j > i + 1) {  // если длина подотрезка не нулевая
                    if (dp[i + 1][j - 1] < dp[i][j]) {
                        dp[i][j] = dp[i + 1][j - 1];
                        separators[i][j] = -1;  // помечаем, что случай особый
                    }
                } else {
                    dp[i][j] = 0;  // для нулевой длины подотрезка
                    separators[i][j] = -1;  // помечаем, что случай особый
                }
            }
        }
    }

    return restore_answer(0, brackets.size() - 1, dp, separators, brackets);  // восстанавливаем ответ рекурсивно
}

// Начало тестов

void test_from_task_1() {
    std::string input = "([)]";
    auto result = find_longest_valid_brackets(input);
    assert(result == "[]");
}

void test_by_shujkova_1() {
    std::string input = "({{)}}";
    auto result = find_longest_valid_brackets(input);
    assert(result == "{{}}");
}

void test_by_shujkova_2() {
    std::string input = "{([(]{)})]";
    auto result = find_longest_valid_brackets(input);
    assert(result == "[({})]");
}

void test_good_brackets() {
    std::string input = "[({})]";
    auto result = find_longest_valid_brackets(input);
    assert(result == "[({})]");
}

void test_empty_string() {
    std::string input = "";
    auto result = find_longest_valid_brackets(input);
    assert(result == "");
}

void test_all_incorrect() {
    std::string input = ")}}{]([({(";
    auto result = find_longest_valid_brackets(input);
    assert(result == "");
}

void test_nested_valid_brackets() {
    std::string input = "{{}({}()[])[]}([{}])[]";
    auto result = find_longest_valid_brackets(input);
    assert(result == "{{}({}()[])[]}([{}])[]");
}

void test_nested_invalid_brackets() {
    std::string input = "{{}(gf{}(s)[]f)[j]}e([{g}])[]g";
    auto result = find_longest_valid_brackets(input);
    assert(result == "{{}({}()[])[]}([{}])[]");
}

void test_one_bracket() {
    std::string input = "{";
    auto result = find_longest_valid_brackets(input);
    assert(result == "");
}

void test_megatits() {
    std::string input = "())(()";
    auto result = find_longest_valid_brackets(input);
    assert(result == "()()");
}

void run_all_tests() {
    test_from_task_1();
    test_by_shujkova_1();
    test_by_shujkova_2();
    test_good_brackets();
    test_empty_string();
    test_all_incorrect();
    test_nested_valid_brackets();
    test_nested_invalid_brackets();
    test_one_bracket();
    test_megatits();
}

// Конец тестов

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        }
    }

    // Чтение входных данных
    std::string brackets;
    std::cin >> brackets;

    auto correct_brackets = find_longest_valid_brackets(brackets);

    // Запись результата
    std::cout << correct_brackets;

    return 0;
}