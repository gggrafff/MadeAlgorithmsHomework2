#include "gtest/gtest.h"

#include "Interpreter/Lexer.h"

class LexerBracketsAndFunctionsTests : public ::testing::Test {
};

TEST_F(LexerBracketsAndFunctionsTests, ParseCode_BracketsArithmetical) {
//arrange
    const std::string code1 = "a = 2 * (2 + 3);";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 10);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[2].get_data().data()), 2);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::MULTIPLICATION);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::LEFT_CIRCLE_BRACKET);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[5].get_data().data()), 2);
    ASSERT_EQ(lexemes1[6].get_type(), Lexeme::LexemeType::PLUS);
    ASSERT_EQ(lexemes1[7].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[7].get_data().data()), 3);
    ASSERT_EQ(lexemes1[8].get_type(), Lexeme::LexemeType::RIGHT_CIRCLE_BRACKET);
    ASSERT_EQ(lexemes1[9].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerBracketsAndFunctionsTests, ParseCode_BracketsLogical) {
//arrange
    const std::string code1 = "a = true && (true || false);";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 10);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexemes1[2].get_data().data()), true);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::LOGICAL_AND);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::LEFT_CIRCLE_BRACKET);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexemes1[5].get_data().data()), true);
    ASSERT_EQ(lexemes1[6].get_type(), Lexeme::LexemeType::LOGICAL_OR);
    ASSERT_EQ(lexemes1[7].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexemes1[7].get_data().data()), false);
    ASSERT_EQ(lexemes1[8].get_type(), Lexeme::LexemeType::RIGHT_CIRCLE_BRACKET);
    ASSERT_EQ(lexemes1[9].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerBracketsAndFunctionsTests, ParseCode_FunctionsCalling) {
//arrange
    const std::string code1 = "a = len(b);";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 7);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::LEFT_CIRCLE_BRACKET);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::RIGHT_CIRCLE_BRACKET);
    ASSERT_EQ(lexemes1[6].get_type(), Lexeme::LexemeType::END);
}