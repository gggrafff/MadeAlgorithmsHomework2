/*
 * E. Замощение доминошками
 * ограничение по времени на тест 1 секунда
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод

 * Вам дана прямоугольная сетка из квадратных ячеек.
 * Символ 'X' представляет собой уже покрытую ячейку, символ '.' - это ячейка, которую еще нужно покрыть.
 * Вы хотите покрыть все ячейки '.', непересекающимися доминошками 2×1.
 * Найдите количество способов сделать это. Два способа считаются различными,
 * если две ячейки покрыты одним и тем же домино в одном замощении и двумя разными домино в другом замощении.
 *
 * Входные данные
 * Первая строка содержит числа n и m (1≤n,m≤12). Следующие n строк содержат по m символов — описание доски.
 * Выходные данные
 * Выведите число замощений доминошками.
 *
 * Примеры
 * Входные данные
 * 2 2
 * ..
 * ..
 * Выходные данные
 * 2
 *
 * Входные данные
 * 3 3
 * ...
 * .X.
 * ...
 * Выходные данные
 * 2
 *
 * Входные данные
 * 3 3
 * ...
 * ...
 * ..X
 * Выходные данные
 * 4
 */

/*
 * Что храним? dp[x][mask] - число способов заполнить первые x клеток, чтобы торчал профиль mask
 * База dp[0][0] = 1; dp[0][i] = 0;
 * Переход 
 * 	dp[x+1][mask >> 1] += dp[x][mask] ничего не кладём
 * 	dp[x+1][mask'] += dp[x][mask] горизонтальная mask'=(mask>>1)+2^(n-1)
 * 	dp[x+1][mask''] += dp[x][mask] вертикальная mask'' = (mask>>1) + 1, x не последняя в столбце (mask & 2) == 0
 * Порядок по возрастанию x
 * Ответ dp[n*m][0]
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>


using Board = std::vector<std::vector<bool>>;

uint64_t count_fillings(const Board &board) {
    assert(board.size() <= 30);
    const uint64_t N = (1ULL << board.size());
    std::vector<std::vector<uint32_t>> dp(board.size() * board.front().size() + 1, std::vector<uint32_t>(N, 0));
    dp[0][0] = 1;
    for (size_t x = 0; x < board.size() * board.front().size(); ++x) {
        for (uint32_t mask = 0; mask < N; ++mask) {
            if (dp[x][mask] == 0) {
                continue;
            }
            if (((mask & 1) == 1) || (board[x % board.size()][x / board.size()])) {  // если клетка занята
                dp[x + 1][mask >> 1] += dp[x][mask];  // ничего не кладём
            } else {
                // кладём горизонтальную
                if ((x + board.size() < board.size() * board.front().size()) &&  // x - не в последнем столбце доски
                    (!board[(x + board.size()) % board.size()][(x + board.size()) /
                                                               board.size()])) {  // следующая по горизонтали клетка доступна по условию
                    const auto mask_hor = (mask >> 1) + (1 << (board.size() - 1));
                    dp[x + 1][mask_hor] += dp[x][mask];
                }

                // кладём вертикальную
                if (
                        ((mask & 2) == 0) &&  // следующая клетка не занята
                        ((x + 1) % board.size() != 0) &&  // x - не в последней строке доски
                        (x < board.size() * board.front().size() - 1) &&
                        (!board[(x + 1) % board.size()][(x + 1) /
                                                        board.size()])) {  // следующая по вертикали клетка доступна по условию
                    const auto mask_ver = (mask >> 1) + 1;
                    dp[x + 1][mask_ver] += dp[x][mask];
                }
            }
        }
    }
    return dp.back()[0];
}


// Начало тестов

void test_from_task_1() {
    const Board board = {
            {false, false},
            {false, false},};
    const auto result = count_fillings(board);
    assert(result == 2);
}

void test_from_task_2() {
    const Board board = {
            {false, false, false},
            {false, true,  false},
            {false, false, false},};
    const auto result = count_fillings(board);
    assert(result == 2);
}

void test_from_task_3() {
    const Board board = {
            {false, false, false},
            {false, false, false},
            {false, false, true},};
    const auto result = count_fillings(board);
    assert(result == 4);
}

void test_asymmetric_board_1() {
    const Board board = {
            {false, false},
            {false, false},
            {false, false},};
    const auto result = count_fillings(board);
    assert(result == 3);
}

void test_asymmetric_board_2() {
    const Board board = {
            {false, false, false},
            {false, false, false},};
    const auto result = count_fillings(board);
    assert(result == 3);
}

void test_passing_through_border_1() {
    const Board board = {
            {false, false},
            {true,  true},
            {false, false},};
    const auto result = count_fillings(board);
    assert(result == 1);
}

void test_passing_through_border_2() {
    const Board board = {
            {false, true,  false, false},
            {false, false, false, true},
            {false, false, false, false},};
    const auto result = count_fillings(board);
    assert(result == 1);
}

void test_1x1_1() {
    const Board board = {
            {false},};
    const auto result = count_fillings(board);
    assert(result == 0);
}

void test_1x1_2() {
    const Board board = {
            {true},};
    const auto result = count_fillings(board);
    assert(result == 1);
}

void test_impossible_1() {
    const Board board = {
            {false, false, false},
            {false, false, false},
            {false, false, false},};
    const auto result = count_fillings(board);
    assert(result == 0);
}

void test_impossible_2() {
    const Board board = {
            {false, false, false},
            {false, false, false},
            {false, true, false},
            {false, false, false},};
    const auto result = count_fillings(board);
    assert(result == 0);
}

void test_impossible_3() {
    const Board board = {
            {false, false, false},
            {true, false, false},
            {false, false, true},
            {false, false, true},};
    const auto result = count_fillings(board);
    assert(result == 0);
}

void test_one_solving_1() {
    const Board board = {
            {false, false, },};
    const auto result = count_fillings(board);
    assert(result == 1);
}

void test_one_solving_2() {
    const Board board = {
            {false, }, {false, } ,};
    const auto result = count_fillings(board);
    assert(result == 1);
}

void test_one_solving_3() {
    const Board board = {
            {true, false, false, }, {false, false, true, } ,};
    const auto result = count_fillings(board);
    assert(result == 1);
}

void test_big_1() {
    const Board board = Board(3, std::vector<bool>(4, false));
    const auto result = count_fillings(board);
    assert(result == 11);
}

void test_last_row_1() {
	const Board board = {
		{false, false,  true, false, true, true},
		{true, false,  false, false, false, false}, };
    const auto result = count_fillings(board);
    assert(result == 1);
}

void test_by_dm_1()
{
	const Board board = {
            {false, false, true},
            {false, false, true},};
    const auto result = count_fillings(board);
    assert(result == 2);
}

void test_by_al_1() {
    const Board board = {
            {false, false, false, false, },
            {false, true, false, false, },
            {false, false, true, false, },
            {false, false, false, false, },};
    const auto result = count_fillings(board);
    assert(result == 0);
}

void test_by_al_2() {
    const Board board = {
            {false, false, false, false, false, },
            {false, true, false, false, false, },
            {false, false, true, false, false, },
            {false, false, false, false, false, },
            {false, false, false, true, false, },};
    const auto result = count_fillings(board);
    assert(result == 8);
}


void run_all_tests() {
    test_from_task_1();
    test_from_task_2();
    test_from_task_3();
    test_asymmetric_board_1();
    test_asymmetric_board_2();
    test_passing_through_border_1();
    test_passing_through_border_2();
    test_1x1_1();
    test_1x1_2();
    test_impossible_1();
    test_impossible_2();
    test_impossible_3();
    test_one_solving_1();
    test_one_solving_2();;
    test_one_solving_3();
	test_big_1();
	test_last_row_1();
	test_by_dm_1();
	test_by_al_1();
    test_by_al_2();
}

// Конец тестов

int main(int argc, char *argv[]) {
    run_all_tests();
    return 0;
    if (argc > 1) {
        if (std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        }
    }

    // Чтение входных данных
    size_t n{0};
    size_t m{0};
    std::cin >> n >> m;
    Board board(n, std::vector<bool>(m, false));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            char c;
            std::cin >> c;
            if (c == 'X') {
                board[i][j] = true;
            }
        }
    }

    const auto result = count_fillings(board);

    // Запись результата
    std::cout << result;

    return 0;
}