/*
 * D. Продавец аквариумов
 * ограничение по времени на тест 1 секунда
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Продавец аквариумов для кошек хочет объехать n городов, посетив каждый из них ровно один раз. Помогите ему найти кратчайший путь.
 *
 * Входные данные
 * Первая строка входного файла содержит натуральное число n (1 ≤ n ≤ 13) — количество городов.
 * Следующие n строк содержат по n чисел — длины путей между городами.
 * В i-й строке j-е число — ai, j — это расстояние между городами i и j (0 ≤ ai, j ≤ 106; ai, j = aj, i; ai, i = 0).
 * Выходные данные
 * В первой строке выходного файла выведите длину кратчайшего пути.
 * Во второй строке выведите n чисел — порядок,в котором нужно посетить города.
 *
 * Примеры
 * Входные данные
 * 5
 * 0 183 163 173 181
 * 183 0 165 172 171
 * 163 165 0 189 302
 * 173 172 189 0 167
 * 181 171 302 167 0
 * Выходные данные
 * 666
 * 4 5 2 3 1
 */

/*
 * Что храним? dp[v][mask] - суммарная длина пути, по которому можно придти в v, посетив mask
 * База dp[start][0] = 0; 
 * Переход dp[v][mask] = min{ dp[u][mask - 2^u] + length[u][v] } по всем u из mask
 * Порядок по возрастанию mask
 * Ответ dp[start][2^n - 1]
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <limits>

using Graph = std::vector<std::vector<uint64_t>>;
using Path = std::pair<uint64_t, std::vector<size_t>>;

/**
 * @brief Извлечение значения бита из битовой маски.
 * @tparam NumberType Числовой тип данных, которым представлена битовая маска.
 * @param mask Битовая маска.
 * @param index Номер бита.
 * @return Значение бита.
 */
template<typename NumberType>
NumberType getBit(NumberType mask, uint8_t index) {
  return (mask >> index) & 1;
}

Path find_shortest_path(const Graph& graph) {
  assert(graph.size() <= 30);
  
  const size_t N = (static_cast<size_t>(1) << graph.size());
  std::vector<std::vector<uint64_t>> dp(graph.size(), std::vector<uint64_t>(N, std::numeric_limits<uint32_t>::max()));
  std::vector<std::vector<size_t>> parents(graph.size(), std::vector<size_t>(N, 0));
  
  for (size_t start = 0; start < graph.size(); ++start) {
    dp[start][0] = 0;
  }
  
  for(uint32_t mask = 1; mask < N; ++mask) {
    for (size_t v = 0; v < graph.size(); ++v) {
      for (size_t u = 0; u < graph.size(); ++u) {
        if (getBit(mask, u) == 1) {
          const auto length = graph[u][v] + dp[u][mask - (1 << u)];
          if (length < dp[v][mask]) {
            dp[v][mask] = length;
            parents[v][mask] = u;
          }
        }
      }
    }
  }

  Path path;
  path.first = dp[0][N - 1];
  /*for (auto& elem: dp)
  {
  	for (auto& elem2: elem)
  {
  	std::cout << elem2 << " ";
  }
  std::cout << std::endl;
  }*/
  size_t current_index = 0;
  uint32_t current_mask = N-1;
  while (current_mask!=0){
  	path.second.push_back(parents[current_index][current_mask]);
  	current_index = parents[current_index][current_mask];
  	current_mask -= 1 << current_index;
  }
  return path;
}

// Начало тестов

void test_from_task_1() {
  const Graph graph { 
    {0, 183, 163, 173, 181},
    {183, 0, 165, 172, 171 },
    {163, 165, 0, 189, 302 },
    {173, 172, 189, 0, 167},
    {181, 171, 302, 167, 0}, };
  const auto path = find_shortest_path(graph);
  std::cout << path.first<<std::endl;
  for (auto& city: path.second){
  	std::cout<<city<<" ";
  }
  //assert(path.first == 666);
}

void run_all_tests()
{
    test_from_task_1();
}

// Конец тестов

int main(int argc, char *argv[]) {
  run_all_tests();
  return 0;
    if (argc > 1)
    {
        if(std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        }
    }

    // Чтение входных данных
    size_t N{ 0 };
    std::cin >> N;
    Graph graph(N, {N, 0});
    for (size_t i = 0; i< N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        std::cin >> graph[i][j];
      }
    }

    auto path = find_shortest_path(graph);

    // Запись результата
    std::cout << path.first << std::endl;
    for (auto city : path.second) {
      std::cout << city + 1 << " ";
    }

    return 0;
}