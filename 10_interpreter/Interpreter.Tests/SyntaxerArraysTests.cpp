//
// Created by ants on 12.05.20.
//

#include "gtest/gtest.h"

#include "Interpreter/Lexer.h"
#include "Interpreter/Syntaxer.h"

class SyntaxerArraysTests : public ::testing::Test {
};


TEST_F(SyntaxerArraysTests, Execute_CreateAndUsingArray) {
//arrange
    const std::string code1 = "a = [1, 2, 3, 4]; ";
    const std::string code2 = "b = len(a); ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    Syntaxer::execute(lexemes1, grammar);
    auto lexemes2 = Lexer::parse_code(code2, grammar);
    Syntaxer::execute(lexemes2, grammar);
//assert
    auto lexeme = grammar.find_lexeme("b");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexeme.value().get_data().data()), 4);
}

TEST_F(SyntaxerArraysTests, Execute_ReadByIndex) {
//arrange
    const std::string code1 = "a = [1, 2, 3, 4]; ";
    const std::string code2 = "b = a[1] * 11; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    Syntaxer::execute(lexemes1, grammar);
    auto lexemes2 = Lexer::parse_code(code2, grammar);
    Syntaxer::execute(lexemes2, grammar);
//assert
    auto lexeme = grammar.find_lexeme("b");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexeme.value().get_data().data()), 22);
}

TEST_F(SyntaxerArraysTests, Execute_WriteByIndex) {
//arrange
    const std::string code1 = "a = [1, 2, 3, 4]; ";
    const std::string code2 = "a[1] = 11; ";
    const std::string code3 = "b = a[1]; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    Syntaxer::execute(lexemes1, grammar);
    auto lexemes2 = Lexer::parse_code(code2, grammar);
    Syntaxer::execute(lexemes2, grammar);
    auto lexemes3 = Lexer::parse_code(code3, grammar);
    Syntaxer::execute(lexemes3, grammar);
//assert
    auto lexeme = grammar.find_lexeme("b");
    ASSERT_TRUE(lexeme.has_value());
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexeme.value().get_data().data()), 11);
}