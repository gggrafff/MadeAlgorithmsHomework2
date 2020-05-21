//
// Created by ants on 12.05.20.
//

#include "gtest/gtest.h"

#include "Interpreter/Lexer.h"
#include "Interpreter/Syntaxer.h"

class SyntaxerComparisonTests : public ::testing::Test {
};


TEST_F(SyntaxerComparisonTests, Execute_ComparisonsLessGreater) {
//arrange
    const std::string code1 = "x = 1 < 2; ";
    const std::string code2 = "y = 1 > 2; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    Syntaxer::execute(lexemes1, grammar);
    auto lexemes2 = Lexer::parse_code(code2, grammar);
    Syntaxer::execute(lexemes2, grammar);
//assert
    auto lexeme = grammar.find_lexeme("x");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexeme.value().get_data().data()), true);
    lexeme = grammar.find_lexeme("y");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexeme.value().get_data().data()), false);
}

TEST_F(SyntaxerComparisonTests, Execute_ComparisonsEqualOrNotEqual) {
//arrange
    const std::string code1 = "x = 1 == 1; ";
    const std::string code2 = "y = 1 == 2; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    Syntaxer::execute(lexemes1, grammar);
    auto lexemes2 = Lexer::parse_code(code2, grammar);
    Syntaxer::execute(lexemes2, grammar);
//assert
    auto lexeme = grammar.find_lexeme("x");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexeme.value().get_data().data()), true);
    lexeme = grammar.find_lexeme("y");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexeme.value().get_data().data()), false);
}

TEST_F(SyntaxerComparisonTests, Execute_ComparisonsLessOrEqual) {
//arrange
    const std::string code1 = "x = 1 <= 2; ";
    const std::string code2 = "y = 2 <= 2; ";
    const std::string code3 = "z = 3 <= 2; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    Syntaxer::execute(lexemes1, grammar);
    auto lexemes2 = Lexer::parse_code(code2, grammar);
    Syntaxer::execute(lexemes2, grammar);
    auto lexemes3 = Lexer::parse_code(code3, grammar);
    Syntaxer::execute(lexemes3, grammar);
//assert
    auto lexeme = grammar.find_lexeme("x");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexeme.value().get_data().data()), true);
    lexeme = grammar.find_lexeme("y");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexeme.value().get_data().data()), true);
    lexeme = grammar.find_lexeme("z");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexeme.value().get_data().data()), false);
}

TEST_F(SyntaxerComparisonTests, Execute_ComparisonsGreaterOrEqual) {
//arrange
    const std::string code1 = "x = 1 >= 2; ";
    const std::string code2 = "y = 2 >= 2; ";
    const std::string code3 = "z = 3 >= 2; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    Syntaxer::execute(lexemes1, grammar);
    auto lexemes2 = Lexer::parse_code(code2, grammar);
    Syntaxer::execute(lexemes2, grammar);
    auto lexemes3 = Lexer::parse_code(code3, grammar);
    Syntaxer::execute(lexemes3, grammar);
//assert
    auto lexeme = grammar.find_lexeme("x");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexeme.value().get_data().data()), false);
    lexeme = grammar.find_lexeme("y");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexeme.value().get_data().data()), true);
    lexeme = grammar.find_lexeme("z");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexeme.value().get_data().data()), true);
}