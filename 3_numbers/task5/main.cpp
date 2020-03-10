/*
 * E. Китайская теорема
 * ограничение по времени на тест 1 секунда
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Решите в целых числах систему уравнений
 * x≡a(modn)
 * x≡b(modm),
 * где n и m взаимно просты. Среди решений следует выбрать наименьшее неотрицательное число.
 *
 * Входные данные
 * Первая строка входных данных содержит число N, 1≤N≤104, — количество тестов, для которых нужно решить задачу.
 * Следующие N строк содержат по четыре целых числа ai, bi, ni и mi (1≤ni,mi≤109, 0≤ai<ni, 0≤bi<mi).
 * Выходные данные
 * Для каждого из тестов выведите искомое наименьшее неотрицательное число xi.
 *
 * Пример
 * Входные данные
 * 2
 * 1 0 2 3
 * 3 2 5 9
 * Выходные данные
 * 3
 * 38
 */

#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <random>
#include <cassert>
#include <numeric>

/**
 * @brief Описывает линейное сравнение x = r (mod m).
 */
struct LinearComparision {
  uint64_t remainder;
  uint64_t module;
};

using SystemLinearComparision = std::vector<LinearComparision>;

/**
 * @brief Расширенный алгоритм Евклида.
 * @param a Первое из чисел, для которых нужно найти НОД
 * @param b Второе из чисел, для которых нужно найти НОД
 * @return (НОД(a, b), x, y) такие, что a * x + b * y = НОД(a, b)
 */
std::tuple<uint64_t, int64_t, int64_t> continued_division(uint64_t a, uint64_t b) {
  if (b == 0) {
    return {a, 1, 0};
  }
  int64_t x2 = 1;
  int64_t x1 = 0;
  int64_t y2 = 0;
  int64_t y1 = 1;
  while (b > 0) {
    const auto q = a / b;
    const auto r = a - q * b;
    const auto x = x2 - q * x1;
    const auto y = y2 - q * y1;
    a = b;
    b = r;
    x2 = x1;
    x1 = x;
    y2 = y1;
    y1 = y;
  }
  return {a, x2, y2};
}

/**
 * @brief Вычисляет обратный элемент к a в кольце вычетов по модулю n.
 * @details Для вычисления используется расширенный алгоритм Евклида.
 * @param a Элемент, к которому нужно найти обратный.
 * @param n Количество элементов в кольце вычетов.
 * @return Возвращает x такой, что a * x = 1 (mod n).
 */
int64_t calculate_inverse_element(const uint64_t a, const uint64_t n) {
  auto [gcd, x, y] = continued_division(a, n);
  if (gcd > 1) {
    return 0;
  }
  if (x>=0) {
    return x;
  }
  return (a * x + a * n) / a;
}

/**
 * @brief Решение системы двух линейных сравнений x = a (mod n) & x = b (mod m).
 * @details Для поиска решения используется китайская теорема об остатках.
 * @param system Система линейных сравнений.
 * @return Такое число x, которое удовлетворяет всем сравнениям системы.
 */
uint64_t solve_system_linear_comparisons(SystemLinearComparision system) {
  assert(system.size() == 2);
  // Решаем систему:
  // x = r1 (mod a1)
  // x = r2 (mod a2)
  // Или в обозначениях авторов задачи:
  // x = a (mod n)
  // x = b (mod m)
  const auto a = system[0].remainder;
  const auto b = system[1].remainder;
  const auto n = system[0].module;
  const auto m = system[1].module;
  // 1. Вычисляем M = П(ai) = n * m
  const auto M = n * m;
  // 2. Для всех i (в данном случае два уравнения, поэтому i=1..2) находим Mi = M / ai
  const auto M1 = m;
  const auto M2 = n;
  // 3. Находим Mi^-1 = 1 / Mi (mod ai), используя расширенный алгоритм Евклида
  const auto M1_inv = calculate_inverse_element(M1, n);
  const auto M2_inv = calculate_inverse_element(M2, m);
  // 4. Вычисляем искомое значение x = sum(ri * Mi * Mi^-1) (mod M)
  if (M1_inv==0 && M2_inv == 0) {
    return a;
  }
  const auto x = (a * M1 * M1_inv + b * M2 * M2_inv) % M;
  return x;
}


// Начало тестов

void test_gcd_random() {
  std::random_device rd;
  const std::uniform_int_distribution<uint64_t> numbers_generator_big(0, 100);
  for(auto i=0;i<100;++i) {
    const auto first = numbers_generator_big(rd);
    const auto second = numbers_generator_big(rd);
    auto [gcd, x, y] = continued_division(first, second);
    assert(std::gcd(first, second) == gcd);
    assert((first * x + second * y) == gcd);
  }
  const std::uniform_int_distribution<uint64_t> numbers_generator_small(0, 7);
  for (auto i = 0; i < 20; ++i) {
    const auto first = numbers_generator_small(rd);
    const auto second = numbers_generator_small(rd);
    auto [gcd, x, y] = continued_division(first, second);
    assert(std::gcd(first, second) == gcd);
    assert((first * x + second * y) == gcd);
  }
}

void test_inverse_elements() {
  auto result = calculate_inverse_element(3, 5);
  assert(result == 2);
  result = calculate_inverse_element(3, 11);
  assert(result == 4); 
  result = calculate_inverse_element(3, 13);
    assert(result == 9);
}

void test_inverse_elements_random() {
  std::random_device rd;
  const std::uniform_int_distribution<uint64_t> numbers_generator_big(0, 100);
  for (auto i = 0; i < 100; ++i) {
    const auto m = numbers_generator_big(rd) + 1;
    const auto a = numbers_generator_big(rd);
    const auto a_inv = calculate_inverse_element(a, m);
    if (a_inv != 0) {
      assert(((a * a_inv) % m) == 1);
    }
  }
  const std::uniform_int_distribution<uint64_t> numbers_generator_small(0, 7);
  for (auto i = 0; i < 20; ++i) {
    const auto m = numbers_generator_small(rd) + 1;
    const auto a = numbers_generator_small(rd);
    const auto a_inv = calculate_inverse_element(a, m);
    if (a_inv != 0) {
      assert(((a * a_inv) % m) == 1);
    }
  }
}

void test_from_task() {
  auto result = solve_system_linear_comparisons({ { 1, 2, }, { 0, 3 } });
  assert(result == 3);
  result = solve_system_linear_comparisons({ { 3, 5, }, { 2, 9 } });
  assert(result == 38);
}

void test_zeros_remainders() {
  const auto result = solve_system_linear_comparisons({ { 0, 2, }, { 0, 3 } });
  assert(result == 0);
}

void test_from_wiki() {
  auto result = solve_system_linear_comparisons({ { 1, 2, }, { 2, 3 } });
  assert(result == 5);
  result = solve_system_linear_comparisons({ { 1, 2, }, { 6, 7 } });
  assert(result == 13);
  result = solve_system_linear_comparisons({ { 2, 3, }, { 6, 7 } });
  assert(result == 20);
}

void test_from_cyberforum() {
  auto result = solve_system_linear_comparisons({ { 2, 5, }, { 6, 7 } });
  assert(result == 27);
  result = solve_system_linear_comparisons({ { 5, 11, }, { 6, 7 } });
  assert(result == 27);
  result = solve_system_linear_comparisons({ { 2, 5, }, { 5, 11 } });
  assert(result == 27);
}

void test_degenerate_system() {
  const auto result = solve_system_linear_comparisons({ { 2, 3, }, { 2, 3 } });
  assert(result == 2);
}

void test_zeros_remainders_2() {
  const auto result = solve_system_linear_comparisons({ { 0, 1, }, { 0, 1 } });
  assert(result == 0);
}

void test_non_primes() {
  auto result = solve_system_linear_comparisons({ { 1, 6, }, { 2, 5 } });
  assert(result == 7);
  result = solve_system_linear_comparisons({ { 1, 6, }, { 0, 7 } });
  assert(result == 7);
  result = solve_system_linear_comparisons({ { 2, 10, }, { 1, 21 } });
  assert(result == 22);
}

void run_all_tests() {
  test_gcd_random();

  test_inverse_elements();
  test_inverse_elements_random();

  test_from_task();
  test_zeros_remainders();
  test_from_wiki();
  test_degenerate_system();
  test_zeros_remainders_2();
  test_non_primes();
  test_from_cyberforum();
}

// Конец тестов

int main(int argc, char *argv[]) {
  run_all_tests();
  return 0;
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

    // Чтение входных данных
    size_t N{ 0 };
    std::cin >> N;
    std::vector<SystemLinearComparision> systems;
  for(size_t i = 0; i< N; ++i) {
    uint64_t a, b, n, m;
    std::cin >> a >> b >> n >> m;
    systems.push_back({ {a, n},{b, m} });
  }

    // Запись результата
  for (const auto& system : systems) {
    std::cout << solve_system_linear_comparisons(system) << "\n";
  }
    return 0;
}