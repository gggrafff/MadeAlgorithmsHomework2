//
// Created by ants on 12.05.20.
//

#include "gtest/gtest.h"

#include "Interpreter/Lexer.h"
#include "Interpreter/Syntaxer.h"

class SyntaxerCommonTests : public ::testing::Test {
};

TEST_F(SyntaxerCommonTests, Execute_ConstPlusConst) {
//arrange
    const std::string code1 = "a = 1 + 2; ";
    Grammar grammar;
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//act
    Syntaxer::execute(lexemes1, grammar);
//assert
    auto lexeme = grammar.find_lexeme("a");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexeme.value().get_data().data()), 3);
}

TEST_F(SyntaxerCommonTests, Execute_ReusableVariables) {
//arrange
    const std::string code1 = "a = 1 + 2; ";
    const std::string code2 = "b = 2 + 3; ";
    const std::string code3 = "c = a + b; ";
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
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexeme.value().get_data().data()), 8);
}

TEST_F(SyntaxerCommonTests, Execute_MinusMultiplicationDivision) {
//arrange
    const std::string code1 = "a = 1.3 * 2.5 - 1.1 / 2.2; ";
    Grammar grammar;
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//act
    Syntaxer::execute(lexemes1, grammar);
//assert
    auto lexeme = grammar.find_lexeme("a");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const float *>(lexeme.value().get_data().data()), 2.75f);
}