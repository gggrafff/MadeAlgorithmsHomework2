/*
 * D. Расстояние по Левенштейну
 *
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дана текстовая строка. С ней можно выполнять следующие операции:
 * 1. Заменить один символ строки на другой символ.
 * 2. Удалить один произвольный символ.
 * 3. Вставить произвольный символ в произвольное место строки.
 * Например, при помощи первой операции из строки «СОК» можно получить строку «СУК»,
 * при помощи второй операции — строку «ОК», при помощи третьей операции — строку «СТОК».
 * Минимальное количество таких операций, при помощи которых можно из одной строки получить другую,
 * называется стоимостью редактирования или расстоянием Левенштейна.
 * Определите расстояние Левенштейна для двух данных строк.
 *
 * Входные данные
 * Программа получает на вход две строки, длина каждой из которых не превосходит 1000 символов, строки состоят только из заглавных латинских букв.
 * Выходные данные
 * Требуется вывести одно число — расстояние Левенштейна для данных строк.
 *
 * Пример
 * Входные данные
 * ABCDEFGH
 * ACDEXGIH
 * Выходные данные
 * 3
 */

 /*
  * Что храним? dp[i][j] - цена редактирования
  * База dp[0][0] = 0, dp[i][0] = i, dp[0][j] = j
  * Переход dp[i][j] = min { dp[i][j-1] + 1, dp[i-1][j] + 1, dp[i-1][j-1] + m(s1[i], s2[j]) }, где m(a,b)==0, если a==b, иначе m(a,b)==1
  * Порядок по возрастанию i, по возрастанию j
  * Ответ dp[M][N], где M, N - длины строк
  */

#include <iostream>
#include <vector>
#include <cassert>

 /**
  * @brief Поиск расстояния Левенштейна между строками методом ДП.
  * @param str1 Первая строка
  * @param str2 Вторая строка
  * @return Расстояние Левенштейна
  */
uint64_t calculate_levenshtein_distance(const std::string& str1, const std::string& str2) {
  std::vector<std::vector<uint64_t>> dp(str1.size() + 1, std::vector<uint64_t>(str2.size() + 1, 0));
  for(size_t i = 0; i < dp.size(); ++i) {
    dp[i][0] = i;
  }
  for (size_t j = 0; j < dp.front().size(); ++j) {
    dp[0][j] = j;
  }
  for (size_t i = 1; i < dp.size(); ++i) {
    for (size_t j = 1; j < dp.front().size(); ++j) {
      dp[i][j] = std::min(dp[i][j - 1] + 1, dp[i - 1][j] + 1);
      dp[i][j] = std::min(dp[i][j], dp[i - 1][j - 1] + (str1[i-1] == str2[j-1] ? 0 : 1));
    }
  }
  return dp.back().back();
}

//Начало тестов

void test_from_task_description()
{
    std::string str1 = "ABCDEFGH";
    std::string str2 = "ACDEXGIH";
    assert(calculate_levenshtein_distance(str1, str2) == 3);
    assert(calculate_levenshtein_distance(str2, str1) == 3);
}

void test_equal_strings()
{
    std::string str1 = "ABCDEFGH";
    std::string str2 = "ABCDEFGH";
    assert(calculate_levenshtein_distance(str1, str2) == 0);
    assert(calculate_levenshtein_distance(str2, str1) == 0);
}

void test_removes()
{
    std::string str1 = "ABCDEFGH";
    std::string str2 = "ACDFH";
    assert(calculate_levenshtein_distance(str1, str2) == 3);
    assert(calculate_levenshtein_distance(str2, str1) == 3);
}

void test_inserts()
{
    std::string str1 = "ABCDEFGH";
    std::string str2 = "AB1CDE2F3GH";
    assert(calculate_levenshtein_distance(str1, str2) == 3);
    assert(calculate_levenshtein_distance(str2, str1) == 3);
}

void test_replaces()
{
    std::string str1 = "ABCDEFGH";
    std::string str2 = "AB1D2FG3";
    assert(calculate_levenshtein_distance(str1, str2) == 3);
    assert(calculate_levenshtein_distance(str2, str1) == 3);
}

void test_polynomial_exponential()
{
    std::string str1 = "polynomial";
    std::string str2 = "exponential";
    assert(calculate_levenshtein_distance(str1, str2) == 6);
    assert(calculate_levenshtein_distance(str2, str1) == 6);
}

void test_saturday_monday()
{
    std::string str1 = "SATURDAY";
    std::string str2 = "MONDAY";
    auto distance = calculate_levenshtein_distance(str1, str2);
    assert(distance == 5);
    distance = calculate_levenshtein_distance(str2, str1);
    assert(distance == 5);
}

void test_application_installation()
{
    std::string str1 = "application";
    std::string str2 = "installation";
    assert(calculate_levenshtein_distance(str1, str2) == 7);
    assert(calculate_levenshtein_distance(str2, str1) == 7);
}

void run_all_tests()
{
    test_from_task_description();
    test_equal_strings();
    test_removes();
    test_inserts();
    test_replaces();
    test_polynomial_exponential();
    test_saturday_monday();
    test_application_installation();
}

// Конец тестов

int main(int argc, char *argv[]) {
    if (argc > 1)
    {
        if(std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        }
    }

    // Чтение входных данных
    std::string str1, str2;
    std::cin >> str1 >> str2;

    const auto distance = calculate_levenshtein_distance(str1, str2);

    // Запись результата
    std::cout << distance;

    return 0;
}