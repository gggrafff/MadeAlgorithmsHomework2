#include "gtest/gtest.h"

#include "Interpreter/Lexer.h"

class LexerComparisonTests : public ::testing::Test {
};

TEST_F(LexerComparisonTests, ParseCode_Less) {
//arrange
    const std::string code1 = "a = 1 < 2;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[2].get_data().data()), 1);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::LESS);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[4].get_data().data()), 2);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerComparisonTests, ParseCode_Greater) {
//arrange
    const std::string code1 = "a = 1 > 2;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[2].get_data().data()), 1);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::GREATER);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[4].get_data().data()), 2);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerComparisonTests, ParseCode_Equal) {
//arrange
    const std::string code1 = "a = 1 == 2;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[2].get_data().data()), 1);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::EQUAL);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[4].get_data().data()), 2);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerComparisonTests, ParseCode_NotEqual) {
//arrange
    const std::string code1 = "a = 1 != 2;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[2].get_data().data()), 1);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::NOT_EQUAL);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[4].get_data().data()), 2);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerComparisonTests, ParseCode_LessOrEqual) {
//arrange
    const std::string code1 = "a = 1 <= 2;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[2].get_data().data()), 1);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::LESS_OR_EQUAL);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[4].get_data().data()), 2);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerComparisonTests, ParseCode_GreaterOrEqual) {
//arrange
    const std::string code1 = "a = 1 >= 2;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[2].get_data().data()), 1);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::GREATER_OR_EQUAL);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[4].get_data().data()), 2);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);
}
