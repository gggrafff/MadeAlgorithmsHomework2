#include "gtest/gtest.h"

#include "Interpreter/Lexer.h"

class LexerArraysTests : public ::testing::Test {
};


TEST_F(LexerArraysTests, ParseCode_CreateArray) {
//arrange
    const std::string code1 = "a = [1, 2];";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 8);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::LEFT_SQUARE_BRACKET);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[3].get_data().data()), 1);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::COMMA);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[5].get_data().data()), 2);
    ASSERT_EQ(lexemes1[6].get_type(), Lexeme::LexemeType::RIGHT_SQUARE_BRACKET);
    ASSERT_EQ(lexemes1[7].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerArraysTests, ParseCode_ReadByIndex) {
//arrange
    const std::string code1 = "a = b[1] + 2;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 9);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::LEFT_SQUARE_BRACKET);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[4].get_data().data()), 1);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::RIGHT_SQUARE_BRACKET);
    ASSERT_EQ(lexemes1[6].get_type(), Lexeme::LexemeType::PLUS);
    ASSERT_EQ(lexemes1[7].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[7].get_data().data()), 2);
    ASSERT_EQ(lexemes1[8].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerArraysTests, ParseCode_WriteByIndex) {
//arrange
    const std::string code1 = "b[1] = 2;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 7);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::LEFT_SQUARE_BRACKET);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[2].get_data().data()), 1);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::RIGHT_SQUARE_BRACKET);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[5].get_data().data()), 2);
    ASSERT_EQ(lexemes1[6].get_type(), Lexeme::LexemeType::END);
}