#include "Grammar.h"
#include "Lexeme.h"

#ifndef MADEALGORITHMSHOMEWORK2_SYNTAXER_H
#define MADEALGORITHMSHOMEWORK2_SYNTAXER_H

/**
 * @brief Синтаксический анализатор
 */
class Syntaxer {
public:
    Syntaxer() = default;
    ~Syntaxer() = default;

    /**
     * @brief Произвести разбор входной последовательности лексем.
     * @param lexemes_input Входные лексемы.
     * @param grammar Грамматика языка.
     */
    static void execute(const std::vector<Lexeme>& lexemes_input, Grammar& grammar);
private:
};


#endif //MADEALGORITHMSHOMEWORK2_SYNTAXER_H
