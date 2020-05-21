#ifndef MADEALGORITHMSHOMEWORK2_LEXER_H
#define MADEALGORITHMSHOMEWORK2_LEXER_H

#include <vector>
#include <set>

#include "Grammar.h"

/**
 * @brief Лексический анализатор.
 */
class Lexer {
public:
    Lexer() = default;
    ~Lexer() = default;

    /**
     * @brief Произвести разбор строки кода.
     * @param code Строка кода.
     * @param grammar Грамматика языка.
     * @return Набор лексем, соответствующие входной строке.
     */
    static std::vector<Lexeme> parse_code(const std::string& code, const Grammar& grammar);

    /**
     * @brief Проверяет, является ли символ скобкой.
     * @param symbol Символ.
     * @return True, если является скобкой. Иначе - false.
     */
    static bool isbracket(char symbol);
private:
};


#endif //MADEALGORITHMSHOMEWORK2_LEXER_H
