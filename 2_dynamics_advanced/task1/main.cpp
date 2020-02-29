/*
 * 
 */

 /*
  * Что храним? dp[mask] - максимальный ответ для подмножества mask
  * База dp[0] = 0
  * Переход dp[mask] = max(dp[mask'], dp[mask''] + 1), где mask' = mask - 2^i, mask'' = mask - 2^i - 2^j (есть ребро между i и j)
  * Порядок по возрастанию mask
  * Ответ dp[2^n - 1]
  */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using Graph = std::vector<std::vector<size_t>>;

uint64_t find_highest_matching(const Graph& graph) {
    std::vector<int64_t> dp(N, 0);
    std::vector<size_t> parents(1, 0);
    for (size_t i = 1; i < N; ++i) {
        auto max_profit_point = std::max_element(next(dp.begin(), (i > K ? i - K : 0)), next(dp.begin(), i));
        dp[i] = coins[i] + *max_profit_point;
        auto distance = std::distance(dp.begin(), max_profit_point);
        parents.push_back((distance > 0 ? static_cast<size_t>(distance) : 0));
    }
    std::vector<size_t> path(1, N-1);
    auto current_index = N - 1;
    while(parents[current_index] != current_index)
    {
        path.push_back(parents[current_index]);
        current_index = parents[current_index];
    }
    std::reverse(path.begin(), path.end());
    return {path, dp[N - 1]};
}

int main(int argc, char *argv[]) {
    // Чтение входных данных
    size_t N{0};
    std::cin >> N;
    Graph graph(N);
    for (size_t i = 0; i < N; ++i) {
        std::string compatibility;
        std::cin >> compatibility;
        for(size_t j = 0; j < N; ++j) {
        	if (compatibility[j] == 'Y') {
        		
        	}
        }
    }
    
    auto pairs_number = find_highest_matching(graph);

    // Запись результата
    std::cout << pairs_number * 2;  // 

    return 0;
}