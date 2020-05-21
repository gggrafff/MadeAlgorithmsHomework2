//
// Created by ants on 12.05.20.
//

#include "gtest/gtest.h"

#include "Interpreter/Lexer.h"
#include "Interpreter/Syntaxer.h"

class SyntaxerFunctionsTests : public ::testing::Test {
};


TEST_F(SyntaxerFunctionsTests, Execute_CallingFunction) {
//arrange
    const std::string code1 = "a = square(3); ";
    const std::string code2 = "b = sqrt(6.25); ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    Syntaxer::execute(lexemes1, grammar);
    auto lexemes2 = Lexer::parse_code(code2, grammar);
    Syntaxer::execute(lexemes2, grammar);
//assert
    auto lexeme = grammar.find_lexeme("a");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexeme.value().get_data().data()), 9);
    lexeme = grammar.find_lexeme("b");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const float *>(lexeme.value().get_data().data()), 2.5f);
}
