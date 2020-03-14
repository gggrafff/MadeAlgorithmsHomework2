/*
  */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

/*std::vector<uint64_t> factorize(const uint64_t number) {
	std::vector<uint64_t> result;
	for(unt64_t i = 2; i<=std::sqrt(number)+1; ++i)
	{
		if (number%i == )
	}
    return result;
}*/

/**
 * @brief Раскладывает каждое число в массиве на простые множители. Линейное решето Эратосфена.
 * @param numbers Числа, которые нужно факторизовать.
 * @return Массив разложений исходных чисел на простые множители.
 */
std::vector<std::vector<uint64_t>> factorize(const std::vector<uint64_t> &numbers) {
    uint64_t max_number = *std::max_element(numbers.begin(), numbers.end());
    std::vector<uint64_t> primes;
    std::vector<uint64_t> least_prime(max_number + 1, 0);

    for (uint64_t i = 2; i <= max_number; ++i) {
        if (least_prime[i] == 0) {
            least_prime[i] = i;
            primes.push_back(i);
        }
        for (const auto prime: primes) {
            if ((prime > least_prime[i]) || (prime * i > max_number)) {
                break;
            }
            least_prime[prime * i] = prime;
        }
    }

    std::vector<std::vector<uint64_t>> result;
    for (auto number: numbers) {
        result.emplace_back();
        while (number != 1) {
            result.back().push_back(least_prime[number]);
            number /= least_prime[number];
        }
    }
    return result;
}

uint64_t binpow(uint64_t a, uint64_t n)
{
	uint64_t res = 1;
	while(n){
		if (n&1){
			res *= a;
		}
		a*=a;
		n >>= 1;
	}
	return res;
}

bool is_generator(const uint64_t g, const uint64_t p)
{
	auto factors = factorize({p-1})[0];
	uint64_t current_factor = 0;
	for (auto factor: factors)
	{
		if (current_factor != factor)
		{
			if (binpow(g, (p-1)/factor) == 1)
			{
				return false;
			}
			current_factor = factor;
		}
	}
	return true;
}

uint64_t find_min_generator(const uint64_t p)
{
	for(uint64_t g = 2; g<p; ++g)
	{
		if (is_generator(g, p))
		{
			return g;
		}
	}
	assert(false);
	return 0;
}

// Начало тестов

void test_from_task_1() {
}

void run_all_tests() {
    test_from_task_1();
}

// Конец тестов

int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);

    if (argc > 1) {
        if (std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        }
    }

    // Чтение входных данных
    uint64_t p{0};
    std::cin >> p;

    const auto g = find_min_generator(p);

    // Запись результата
    std::cout << g;

    return 0;
}