/*
 * A. Персистентный стек
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Реализуйте персистентный стек.
 *
 * Входные данные
 * Первая строка содержит количество действий n (1≤n≤200000).
 * В строке номер i+1 содержится описание действия i:
 *      t m — добавить в конец стека номер t (0≤t<i) число m (0<m≤1000);
 *      t 0 — удалить последний элемент стека номер t (0≤t<i). Гарантируется, что стек t не пустой.
 * В результате действия i, описанного в строке i+1, создается стек номер i.
 * Изначально имеется пустой стек с номером ноль.
 * Все числа во входном файле целые.
 *
 * Выходные данные
 * Для каждой операции удаления выведите удаленный элемент на отдельной строке.
 *
 * Пример
 * входные данные
 * 8
 * 0 1
 * 1 5
 * 2 4
 * 3 2
 * 4 3
 * 5 0
 * 6 6
 * 1 0
 * выходные данные
 * 3
 * 1
 */

/*
 * Подробнее о персистентном стеке:
 * https://neerc.ifmo.ru/wiki/index.php?title=Персистентный_стек
 * https://habr.com/ru/post/113585/
 * https://ejudge.lksh.ru/archive/2015/08/Aprime/lection/main.pdf
 */

#include <tuple>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <array>
#include <optional>
#include <string>

/**
 * @brief Пространство имён, объединяющее персистентные структуры данных.
 */
namespace persistence {
    /**
     * @brief Персистентный стек.
     * @tparam T Тип данных, хранящихся в стеке.
     */
    template<typename T>
    class Stack {
    public:
        /**
         * @brief Создаёт пустой стек, который имеет версию 0.
         */
        Stack() {
            storage_.push_back({std::nullopt, 0});
        }

        ~Stack() = default;

        /**
         * @brief Добавляет элемент в стек.
         * @param version Версия стека, в которую нужно добавить новый элемент.
         * @param value Добавляемое значение.
         * @return Номер новой версии стека с добавленным элементом.
         */
        size_t push(size_t version, const T &value) {
            storage_.push_back({value, version});
            return storage_.size() - 1;
        }

        /**
         * @brief Достаёт элемент из стека.
         * @param version Версия стека, из которой надо достать элемент.
         * @return Кортеж: Изъятый элемент и номер новой версии стека после удаления элемента.
         */
        std::tuple<T, size_t> pop(size_t version) {
            auto &prev = storage_[storage_[version].previous_index];
            storage_.push_back(prev);
            return {storage_[version].value.value(), storage_.size() - 1};
        }

        /**
         * @brief Проверяет, пуст ли стек.
         * @param version Версия стека, которую проверяем.
         * @return True, если стек пуст.
         */
        bool empty(size_t version) {
            return !storage_[version].value.has_value();
        }

    private:
        /**
         * @brief Структура, описывающая узел дерева изменений.
         */
        struct Node {
            const std::optional<T> value;  // значение в вершине стека
            const size_t previous_index;  // индекс предыдущей версии стека
        };

        std::vector<Node> storage_;  // хранит все узлы со всеми элементами всех версий стека
    };
}

// Начало тестов

void test_from_task() {
    persistence::Stack<uint16_t> stack;
    stack.push(0, 1);
    stack.push(1, 5);
    stack.push(2, 4);
    stack.push(3, 2);
    stack.push(4, 3);
    assert(std::get<0>(stack.pop(5)) == 3);
    stack.push(6, 6);
    assert(std::get<0>(stack.pop(1)) == 1);
}

void run_all_tests() {
    test_from_task();
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
    size_t N{0};
    std::cin >> N;
    persistence::Stack<uint16_t> stack;
    for (size_t i = 0; i < N; ++i) {
        size_t index{0};
        uint16_t value{0};
        std::cin >> index >> value;
        if (value > 0) {
            stack.push(index, value);
        } else {
            auto[value, new_index] = stack.pop(index);
            std::cout << value << "\n";
        }
    }

    return 0;
}
