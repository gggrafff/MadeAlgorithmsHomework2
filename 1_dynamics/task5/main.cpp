/*
 * E. Кафе
 *
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 64 мегабайта
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Около Петиного университета недавно открылось новое кафе, в котором действует следующая система скидок:
 * при каждой покупке более чем на 100 рублей покупатель получает купон, дающий право на один бесплатный обед
 * (при покупке на сумму 100 рублей и меньше такой купон покупатель не получает).
 * Однажды Пете на глаза попался прейскурант на ближайшие n дней. Внимательно его изучив, он решил,
 * что будет обедать в этом кафе все n дней, причем каждый день он будет покупать в кафе ровно один обед.
 * Однако стипендия у Пети небольшая, и поэтому он хочет по максимуму использовать предоставляемую систему скидок так,
 * чтобы его суммарные затраты были минимальны. Требуется найти минимально возможную суммарную стоимость
 * обедов и номера дней, в которые Пете следует воспользоваться купонами.
 *
 * Входные данные
 * В первой строке входного файла записано целое число n (0 ≤ n ≤ 100).
 * В каждой из последующих n строк записано одно целое число, обозначающее стоимость обеда
 * в рублях на соответствующий день. Стоимость — неотрицательное целое число, не превосходящее 300.
 * Выходные данные
 * В первой строке выдайте минимальную возможную суммарную стоимость обедов.
 * Во второй строке выдайте два числа k1 и k2 — количество купонов, которые останутся неиспользованными у
 * Пети после этих n дней и количество использованных им купонов соответственно.
 * В последующих k2 строках выдайте в возрастающем порядке номера дней, когда Пете следует воспользоваться купонами.
 * Если существует несколько решений с минимальной суммарной стоимостью, то выдайте то из них,
 * в котором значение k1 максимально (на случай, если Петя когда-нибудь ещё решит заглянуть в это кафе).
 * Если таких решений несколько, выведите любое из них.
 *
 * Примеры
 * Входные данные
 * 5
 * 110
 * 40
 * 120
 * 110
 * 60
 * Выходные данные
 * 260
 * 0 2
 * 3
 * 5
 *
 * Входные данные
 * 3
 * 110
 * 110
 * 110
 * Выходные данные
 * 220
 * 1 1
 * 2
 */

/*
 * Что храним? dp[i][j] - суммарная стоимость обедов по истечении i-го дня и при наличии на руках j купонов
 * База dp[0][0] = prices[0] и dp[0][1] = inf, если prices[0] < threshold, иначе: dp[0][0] = inf и dp[0][1] = prices[0]
 * Переход dp[i][j] = min{ dp[i-1][j] + prices[i](если prices[i]<threshold), dp[i-1][j+1], dp[i-1][j-1] + prices[i] (если prices[i]>=threshold) }
 * Порядок по возрастанию j, по возрастанию i
 * Ответ min{ dp[N][j] }, если минимумов несколько, то максимизируем по j
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cassert>

/**
 * @brief Структура для хранения результата
 */
struct Strategy {
    uint64_t cost{0};  // Суммарная стоимость обедов
    uint64_t spend_coupons{0};  // Потрачено купонов
    uint64_t rest_coupons{0};  // Осталось купонов
    std::vector<size_t> days_for_free;  // Номера дней, в которые нужно пообедать за купоны
};

/**
 * @brief Позиция в двумерном массиве
 */
using Position = std::pair<size_t, size_t>;


/**
 * @brief Поиск наиболее выгодной стратегии оплаты обедов методом ДП.
 * @param prices Цены на обеды.
 * @return Стратегия оплаты обедов.
 */
Strategy find_economical_strategy(const std::vector<uint64_t> &prices) {
    if (prices.empty())
    {
        return {};

    }
    std::vector<std::vector<uint64_t>> dp(
            prices.size() + 1,
            std::vector<uint64_t>(prices.size() + 2, std::numeric_limits<uint32_t>::max()));  // с каёмкой
    std::vector<std::vector<Position>> parents(prices.size(), std::vector<Position>(prices.size(), {0, 0}));
    const uint64_t threshold = 100;
    if (prices[0] <= threshold) {  // база
        dp[1][1] = prices[0];
    } else {
        dp[1][2] = prices[0];
    }
    for (size_t i = 2; i < dp.size(); ++i) {  // учитываем каёмку и базу
        for (size_t j = 1; j < dp.front().size() - 1; ++j) {  // учитываем каёмку
            if (prices[i - 1] <= threshold) {  // не получаем купона
                if (dp[i - 1][j] + prices[i - 1] < dp[i - 1][j + 1]) {
                    dp[i][j] = dp[i - 1][j] + prices[i - 1];  // обедаем за деньги
                    parents[i - 1][j - 1] = {i - 2, j - 1};  // массив родителей нумеруется с 0
                } else {
                    dp[i][j] = dp[i - 1][j + 1];  // обедаем за купон
                    parents[i - 1][j - 1] = {i - 2, j};  // массив родителей нумеруется с 0
                }
            } else {  // можем получить купон, если заплатим
                if (dp[i - 1][j - 1] + prices[i - 1] < dp[i - 1][j + 1]) {
                    dp[i][j] = dp[i - 1][j - 1] + prices[i - 1];  // обедаем за деньги, получаем купон
                    parents[i - 1][j - 1] = {i - 2, j - 2};  // массив родителей нумеруется с 0
                } else {
                    dp[i][j] = dp[i - 1][j + 1];  // обедаем за купон
                    parents[i - 1][j - 1] = {i - 2, j};  // массив родителей нумеруется с 0
                }
            }
        }
    }

    // Заполняем структуру с результатом
    auto min_cost = std::min_element(dp.back().rbegin(), dp.back().rend());
    Strategy strategy;
    strategy.cost = *min_cost;
    strategy.rest_coupons = dp.back().size() - std::distance(dp.back().rbegin(), min_cost) - 2;

    // Поиск дней для бесплатного обеда в массиве родителей
    Position current_position{prices.size() - 1, strategy.rest_coupons};
    if (current_position.second >= parents.size()) {
        current_position.second = parents.size() - 1;
    }
    while (parents[current_position.first][current_position.second] != current_position) {
        if (current_position.second < parents[current_position.first][current_position.second].second) {
            strategy.days_for_free.push_back(current_position.first + 1);
        }
        current_position = parents[current_position.first][current_position.second];
    }
    std::sort(strategy.days_for_free.begin(), strategy.days_for_free.end());

    strategy.spend_coupons = strategy.days_for_free.size();

    return strategy;
}

// Начало тестов

void test_from_task_1() {
    auto strategy = find_economical_strategy({110, 40, 120, 110, 60});
    assert(strategy.cost == 260);
    assert(strategy.rest_coupons == 0);
    assert(strategy.spend_coupons == 2);
    assert(strategy.days_for_free == std::vector<size_t>({3, 5}));
}

void test_from_task_2() {
    auto strategy = find_economical_strategy({110, 110, 110});
    assert(strategy.cost == 220);
    assert(strategy.rest_coupons == 1);
    assert(strategy.spend_coupons == 1);
    assert(strategy.days_for_free == std::vector<size_t>({2}) || strategy.days_for_free == std::vector<size_t>({3}));
}

void test_all_cheap() {
    auto strategy = find_economical_strategy({10, 40, 30, 90, 60});
    assert(strategy.cost == 230);
    assert(strategy.rest_coupons == 0);
    assert(strategy.spend_coupons == 0);
    assert(strategy.days_for_free.empty());
}

void test_all_expensive() {
    auto strategy = find_economical_strategy({110, 140, 130, 190, 160});
    assert(strategy.cost == 380);
    assert(strategy.rest_coupons == 1);
    assert(strategy.spend_coupons == 2);
    assert(strategy.days_for_free == std::vector<size_t>({4, 5}));
}

void test_zigzag_with_offset_1() {
    auto strategy = find_economical_strategy({110, 40, 30, 190, 60, 50, 150});
    assert(strategy.cost == 420);
    assert(strategy.rest_coupons == 0);
    assert(strategy.spend_coupons == 2);
    assert(strategy.days_for_free == std::vector<size_t>({5, 7}));
}

void test_zigzag_with_offset_2() {
    auto strategy = find_economical_strategy({110, 40, 30, 190, 60, 50, 150, 30});
    assert(strategy.cost == 440);
    assert(strategy.rest_coupons == 0);
    assert(strategy.spend_coupons == 2);
    assert(strategy.days_for_free == std::vector<size_t>({4, 8}));
}

void test_one_cheap_day() {
    auto strategy = find_economical_strategy({30});
    assert(strategy.cost == 30);
    assert(strategy.rest_coupons == 0);
    assert(strategy.spend_coupons == 0);
    assert(strategy.days_for_free.empty());
}

void test_one_expensive_day() {
    auto strategy = find_economical_strategy({110});
    assert(strategy.cost == 110);
    assert(strategy.rest_coupons == 1);
    assert(strategy.spend_coupons == 0);
    assert(strategy.days_for_free.empty());
}

void test_empty() {
	auto strategy = find_economical_strategy({});
    assert(strategy.cost ==0);
    assert(strategy.rest_coupons == 0);
    assert(strategy.spend_coupons == 0);
    assert(strategy.days_for_free.empty());
}

void run_all_tests()
{
    test_from_task_1();
    test_from_task_2();
    test_all_cheap();
    test_all_expensive();
    test_zigzag_with_offset_1();
    test_zigzag_with_offset_2();
    test_one_cheap_day();
    test_one_expensive_day();
    test_empty();
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
    size_t N;
    std::cin >> N;
    std::vector<uint64_t> prices(N, 0);
    for (auto &price : prices) {
        std::cin >> price;
    }

    auto strategy = find_economical_strategy(prices);

    // Запись результата
    std::cout << strategy.cost << std::endl;
    std::cout << strategy.rest_coupons << " " << strategy.spend_coupons << std::endl;
    for (auto &day : strategy.days_for_free) {
        std::cout << day << std::endl;
    }
    return 0;
}