//
// Created by ants on 12.05.20.
//

#include "gtest/gtest.h"

#include "Interpreter/Lexer.h"
#include "Interpreter/Syntaxer.h"

class SyntaxerLogicalTests : public ::testing::Test {
};


TEST_F(SyntaxerLogicalTests, Execute_LogicalOperations) {
//arrange
    const std::string code1 = "x = true && false; ";
    const std::string code2 = "y = true || false; ";
    const std::string code3 = "z = !true; ";
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
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexeme.value().get_data().data()), false);
}
