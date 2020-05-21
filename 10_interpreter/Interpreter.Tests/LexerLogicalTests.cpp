#include "gtest/gtest.h"

#include "Interpreter/Lexer.h"

class LexerLogicalTests : public ::testing::Test {
};

TEST_F(LexerLogicalTests, ParseCode_TrueAndFalse) {
//arrange
    const std::string code1 = "a = true && false;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexemes1[2].get_data().data()), true);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::LOGICAL_AND);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexemes1[4].get_data().data()), false);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerLogicalTests, ParseCode_TrueOrFalse) {
//arrange
    const std::string code1 = "a = true || false;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexemes1[2].get_data().data()), true);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::LOGICAL_OR);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexemes1[4].get_data().data()), false);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerLogicalTests, ParseCode_TrueXorFalse) {
//arrange
    const std::string code1 = "a = true ^ false;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexemes1[2].get_data().data()), true);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::LOGICAL_XOR);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexemes1[4].get_data().data()), false);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerLogicalTests, ParseCode_NotTrue) {
//arrange
    const std::string code1 = "a = !true;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 5);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::LOGICAL_NOT);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const bool *>(lexemes1[3].get_data().data()), true);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::END);
}