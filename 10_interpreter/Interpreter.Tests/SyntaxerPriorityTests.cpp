//
// Created by ants on 12.05.20.
//

#include "gtest/gtest.h"

#include "Interpreter/Lexer.h"
#include "Interpreter/Syntaxer.h"

class SyntaxerPriorityTests : public ::testing::Test {
};

TEST_F(SyntaxerPriorityTests, Execute_PlusMultiplication) {
//arrange
    const std::string code1 = "a = 2 * 2 + 3; ";
    const std::string code2 = "b = 3 + 2 * 2; ";
    const std::string code3 = "c = a + b * 3; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    Syntaxer::execute(lexemes1, grammar);
    auto lexemes2 = Lexer::parse_code(code2, grammar);
    Syntaxer::execute(lexemes2, grammar);
    auto lexemes3 = Lexer::parse_code(code3, grammar);
    Syntaxer::execute(lexemes3, grammar);
//assert
    auto lexeme = grammar.find_lexeme("c");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexeme.value().get_data().data()), 28);
}

TEST_F(SyntaxerPriorityTests, Execute_MinusMultiplicationDivision) {
//arrange
    const std::string code1 = "a = 1.3 * 2.5 - 1.1 / 2.2; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    Syntaxer::execute(lexemes1, grammar);
//assert
    auto lexeme = grammar.find_lexeme("a");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const float *>(lexeme.value().get_data().data()), 2.75f);
}

TEST_F(SyntaxerPriorityTests, Execute_Brackets) {
//arrange
    const std::string code1 = "a = 2 * (2 + 3); ";
    const std::string code2 = "b = (2 + 3) * 2; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    Syntaxer::execute(lexemes1, grammar);
    auto lexemes2 = Lexer::parse_code(code2, grammar);
    Syntaxer::execute(lexemes2, grammar);
//assert
    auto lexeme = grammar.find_lexeme("a");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexeme.value().get_data().data()), 10);
    lexeme = grammar.find_lexeme("b");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexeme.value().get_data().data()), 10);
}