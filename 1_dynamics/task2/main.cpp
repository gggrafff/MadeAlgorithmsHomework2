/*
 * B. Черепаха и монеты
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Черепаха хочет переползти из левого верхнего угла поля размером N на M клеток (2 ≤ N, M ≤ 1000) в правый нижний.
 * За один шаг она может переместиться на соседнюю клетку вправо или на соседнюю клетку вниз.
 * Кроме того, проходя через каждую клетку, Черепаха получает (или теряет) несколько золотых монет
 * (это число известно для каждой клетки).
 * Определите, какое максимальное количество монет может собрать Черепаха по пути и как ей нужно идти для этого.
 *
 * Входные данные
 * В первой строке вводятся два натуральных числа: N и M (2 ≤ N, M ≤ 1000), разделённые пробелом.
 * В каждой из следующих N строк записаны через пробел по M чисел aij(|aij| ≤ 10),
 * которые обозначают количество монет, получаемых Черепашкой при проходе через каждую клетку.
 * Если это число отрицательное, Черепашка теряет монеты.
 * Выходные данные
 * В первой строке программа должна вывести наибольшее количество монет, которое может собрать Черепаха.
 * Во второй строке без пробелов выводятся команды, которые нужно выполнить Черепахе: буква 'R' (от слова right)
 * обозначает шаг вправо, а буква 'D' (от слова down) – шаг вниз.
 *
 * Пример
 * Входные данные
 * 3 3
 * 0 2 -3
 * 2 -5 7
 * 1 2 0
 * Выходные данные
 * 6
 * RRDD
 */

/*
 * Что храним? dp[i][j] - максимальное число монет на пути в точку (i, j)
 * База dp[0][0] = coins[0][0]
 * Переход dp[i] = coins[i][j] + max(dp[i-1][j], dp[i][j-1])
 * Порядок по возрастанию i, по возрастанию j
 * Ответ dp[N][M]
 */

#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

/**
 * @brief Путь черепашки. Первый элемент - последовательность перемещений. Второй элемент - количество монет на пути.
 */
using Path = std::pair<std::vector<char>, int64_t>;
/**
 * @brief Позиция на поле задаётся двумя координатами.
 */
using Position = std::pair<size_t, size_t>;


/**
 * @brief Поиск наиболее прибыльного пути методом динамического программировния.
 * @param N Количество строк на поле.
 * @param M Количество столбцов на поле.
 * @param coins Двумерный массив, хранящий количество монет на каждой клетке (м.б. отрицательным).
 * @return Наиболее прибыльный путь черепашки.
 */
Path find_profitable_path(const size_t N, const size_t M, const std::vector<std::vector<int64_t>>& coins) {
    std::vector<std::vector<int64_t>> dp(N + 1, std::vector<int64_t>(M + 1, std::numeric_limits<int64_t>::min()));  // добавляем каёмку
    dp[0][1] = 0;  // обеспечиваем корректность каёмки для ячейки (0, 0)
    dp[1][0] = 0;  // обеспечиваем корректность каёмки для ячейки (0, 0)
    std::vector<std::vector<Position>> parents(N, std::vector<Position>(M, {0, 0}));
    for (size_t i = 1; i <= N; ++i) {  // из-за каёмки начинаем нумерацию с 1
        for (size_t j = 1; j <= M; ++j) {  // из-за каёмки начинаем нумерацию с 1
            if (dp[i - 1][j] > dp[i][j - 1]) {
                dp[i][j] = dp[i - 1][j] + coins[i - 1][j - 1];  // массив монет нумеруется с 0
                parents[i - 1][j - 1] = {i - 1, j};  // родители в своём контейнере нумеруются с 0, а их позиции в Position - с 1
            } else {
                dp[i][j] = dp[i][j - 1] + coins[i - 1][j - 1];  // массив монет нумеруется с 0
                parents[i - 1][j - 1] = {i, j - 1};  // родители в своём контейнере нумеруются с 0, а их позиции в Position - с 1
            }
        }
    }
    parents[0][0] = {1, 1};
    std::vector<char> path;
    Position current_position{N, M};
    while (parents[current_position.first - 1][current_position.second - 1] != current_position) {
        if (current_position.first != parents[current_position.first - 1][current_position.second - 1].first) {
            path.push_back('D');
        } else {
            path.push_back('R');
        }
        current_position = parents[current_position.first - 1][current_position.second - 1];
    }
    std::reverse(path.begin(), path.end());
    return {path, dp[N][M]};
}

int main(int argc, char *argv[]) {
    // Чтение входных данных
    size_t N{0};
    size_t M{0};
    std::cin >> N >> M;
    std::vector<std::vector<int64_t>> coins(N, std::vector<int64_t>(M, 0));
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            std::cin >> coins[i][j];
        }
    };

    Path path = find_profitable_path(N, M, coins);

    // Вывод результата
    std::cout << path.second << std::endl;
    for (const auto &step: path.first) {
        std::cout << step;
    }

    return 0;
}