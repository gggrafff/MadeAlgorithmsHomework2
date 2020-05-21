#ifndef MADEALGORITHMSHOMEWORK2_CONVOLUTION_H
#define MADEALGORITHMSHOMEWORK2_CONVOLUTION_H

#include <string>
#include <vector>
#include <functional>

#include "Lexeme.h"
#include "Grammar.h"

class Grammar;

/**
 * @brief Свёртка лексем при синтаксическом анализе.
 */
class Convolution {
public:

    /**
     * @brief Конструктор свёртки.
     * @param from Какую последовательность лексем сворачивает эта свёртка.
     * @param action Действие, которое производится при свёртке.
     */
    Convolution(const std::vector<Lexeme::LexemeType> &from,
                const std::function<void(std::vector<Lexeme> &, Grammar &)> &action) : from_(from), action_(action) {}

    /**
     * @brief Проверить, начинается ли сворачиваемая этой свёрткой последовательность лексем с переданной последовательности лексем.
     * @param pattern Последовательность лексем, которую хотим сравнить.
     * @return True, если последовательность для данной свёртки начинается так же, как переданная последовательность.
     */
    bool starts_with(const std::vector<Lexeme::LexemeType> &pattern) const;

    /**
     * @brief Проверить, заканчивается ли сворачиваемая этой свёрткой последовательность лексем переданной последовательностью лексем.
     * @param pattern Последовательность лексем, которую хотим сравнить.
     * @return True, если последовательность для данной свёртки заканчивается так же, как переданная последовательность.
     */
    bool ends_with(const std::vector<Lexeme::LexemeType> &pattern) const;

    /**
     * @brief Проверить, заканчивается ли переданная последоватлеьность лексем последовательностью лексем , которые сворачиваются этой свёрткой.
     * @param pattern Последовательность лексем, которую хотим сравнить.
     * @return True, если последовательность для данной свёртки является концом переданной последовательности.
     */
    bool is_end(const std::vector<Lexeme::LexemeType> &sequence) const;

    /**
     * @brief Проверить совпадает ли сворачиваемая этой свёрткой последовательность лексем с переданной последовательностью лексем.
     * @param pattern Последовательность лексем, которую хотим сравнить.
     * @return True, если последовательность для данной свёртки совпадает с переданной последовательностью.
     */
    bool match(const std::vector<Lexeme::LexemeType> &pattern) const;

    /**
     * @brief Применить свёртку.
     * @param lexemes Набор лексем. Состояние синтаксического анализатора.
     * @param grammar Грамматика языка.
     */
    void apply(std::vector<Lexeme> &lexemes, Grammar& grammar);

    /**
     * @brief Печать информации о свёртке для отладки и сообщений об ошибках.
     */
    void debug_print() const;

    /**
     * @brief Сколько лексем сворачивает данная свёртка?
     * @return Количество сворачиваемых лексем.
     */
    size_t size() {
        return from_.size();
    }

private:
    std::vector<Lexeme::LexemeType> from_;
    std::function<void(std::vector<Lexeme> &, Grammar& grammar)> action_;
};


#endif //MADEALGORITHMSHOMEWORK2_CONVOLUTION_H
