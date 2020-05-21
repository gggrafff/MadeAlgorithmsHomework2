#ifndef MADEALGORITHMSHOMEWORK2_GRAMMAR_H
#define MADEALGORITHMSHOMEWORK2_GRAMMAR_H

#include <string>
#include <vector>
#include <functional>
#include <cassert>
#include <unordered_map>
#include <iostream>

#include "Lexeme.h"
#include "Convolution.h"

class Convolution;

/**
 * @brief Класс, описывающий грамматику языка
 */
class Grammar {
public:
    Grammar();

    /**
     * @brief Добавляет лексему в язык.
     * @param lexeme Лексема.
     */
    void add_lexeme(const Lexeme& lexeme);

    /**
     * @brief Найти лексему в языке.
     * @param code Строковое представление лексемы в коде.
     * @return Искомая лексема, если она присутствует в языке.
     */
    std::optional<Lexeme> find_lexeme(const std::string &code) const;

    /**
     * @brief Требуется ли чтение следующей лексемы при синтаксическом анализе.
     * @param current_sequence Состояние анализатора. Предыдущие прочитанные лексемы, возможно, подвергнутые свёрткам.
     * @param next Следующая лексема.
     * @return True, если нужно читать следующую лексему, иначе - false.
     */
    bool need_next(const std::vector<Lexeme> &current_sequence, const Lexeme& next);

    /**
     * @brief Найти свёртку при синтаксическом анализе.
     * @param current_sequence Состояние анализатора. Предыдущие прочитанные лексемы, возможно, подвергнутые свёрткам.
     * @return Свёртка, которую нужно применить, если она имеется в языке.
     */
    const std::optional<Convolution> find_convolution(const std::vector<Lexeme> &current_sequence);

private:
    std::unordered_map<std::string, Lexeme> lexemes_;
    std::vector<Convolution> convolutions_;
};

#endif //MADEALGORITHMSHOMEWORK2_GRAMMAR_H
