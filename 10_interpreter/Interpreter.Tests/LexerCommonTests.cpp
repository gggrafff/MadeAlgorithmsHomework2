#include "gtest/gtest.h"

#include "Interpreter/Lexer.h"
#include "Interpreter/Lexeme.h"

class LexerCommonTests : public ::testing::Test {
};

TEST_F(LexerCommonTests, ParseCode_SummingConstants) {
//arrange
    const std::string code1 = "a = 1 + 2; ";
    const std::string code2 = "a=1+2;";
    const std::string code3 = "a    =1 +2; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
    auto lexemes2 = Lexer::parse_code(code2, grammar);
    auto lexemes3 = Lexer::parse_code(code3, grammar);
//assert
    ASSERT_EQ(lexemes1, lexemes2);
    ASSERT_EQ(lexemes2, lexemes3);
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[2].get_data().data()), 1);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::PLUS);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[4].get_data().data()), 2);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerCommonTests, ParseCode_SummingConstantsSeveralStatements) {
//arrange
    const std::string code1 = "a = 1 + 2; a=1+2;a    =1 +2; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 18);
    for (auto i = 0; i < lexemes1.size() / 6; ++i) {
        ASSERT_EQ(lexemes1[0 + i * 6].get_type(), Lexeme::LexemeType::IDENTIFIER);
        ASSERT_EQ(lexemes1[1 + i * 6].get_type(), Lexeme::LexemeType::ASSIGN);
        ASSERT_EQ(lexemes1[2 + i * 6].get_type(), Lexeme::LexemeType::CONSTANT);
        ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[2 + i * 6].get_data().data()), 1);
        ASSERT_EQ(lexemes1[3 + i * 6].get_type(), Lexeme::LexemeType::PLUS);
        ASSERT_EQ(lexemes1[4 + i * 6].get_type(), Lexeme::LexemeType::CONSTANT);
        ASSERT_EQ(*reinterpret_cast<const int32_t *>(lexemes1[4 + i * 6].get_data().data()), 2);
        ASSERT_EQ(lexemes1[5 + i * 6].get_type(), Lexeme::LexemeType::END);
    }
}

TEST_F(LexerCommonTests, ParseCode_FloatNumbersParsing) {
//arrange
    const std::string code1 = "a = 1.3 + 2.5; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const float *>(lexemes1[2].get_data().data()), 1.3f);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::PLUS);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const float *>(lexemes1[4].get_data().data()), 2.5f);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerCommonTests, ParseCode_ReusableIds) {
//arrange
    const std::string code1 = "a = 1.3 + 2.5; b = 1.0 + 0.5; c = a + b;";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 18);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::PLUS);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::END);

    ASSERT_EQ(lexemes1[0 + 6].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1 + 6].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2 + 6].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(lexemes1[3 + 6].get_type(), Lexeme::LexemeType::PLUS);
    ASSERT_EQ(lexemes1[4 + 6].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(lexemes1[5 + 6].get_type(), Lexeme::LexemeType::END);

    ASSERT_EQ(lexemes1[0 + 6 * 2].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1 + 6 * 2].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2 + 6 * 2].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[3 + 6 * 2].get_type(), Lexeme::LexemeType::PLUS);
    ASSERT_EQ(lexemes1[4 + 6 * 2].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[5 + 6 * 2].get_type(), Lexeme::LexemeType::END);
}

TEST_F(LexerCommonTests, ParseCode_MinusMultiplicationDivisionParsing) {
//arrange
    const std::string code1 = "a = 1.3 * 2.5 - 1.1 / 2.2; ";
    Grammar grammar;
//act
    auto lexemes1 = Lexer::parse_code(code1, grammar);
//assert
    ASSERT_EQ(lexemes1.size(), 6);
    ASSERT_EQ(lexemes1[0].get_type(), Lexeme::LexemeType::IDENTIFIER);
    ASSERT_EQ(lexemes1[1].get_type(), Lexeme::LexemeType::ASSIGN);
    ASSERT_EQ(lexemes1[2].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const float *>(lexemes1[2].get_data().data()), 1.3f);
    ASSERT_EQ(lexemes1[3].get_type(), Lexeme::LexemeType::MULTIPLICATION);
    ASSERT_EQ(lexemes1[4].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const float *>(lexemes1[4].get_data().data()), 2.5f);
    ASSERT_EQ(lexemes1[5].get_type(), Lexeme::LexemeType::MINUS);
    ASSERT_EQ(lexemes1[6].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const float *>(lexemes1[6].get_data().data()), 1.1f);
    ASSERT_EQ(lexemes1[7].get_type(), Lexeme::LexemeType::DIVISION);
    ASSERT_EQ(lexemes1[8].get_type(), Lexeme::LexemeType::CONSTANT);
    ASSERT_EQ(*reinterpret_cast<const float *>(lexemes1[8].get_data().data()), 2.2f);
    ASSERT_EQ(lexemes1[9].get_type(), Lexeme::LexemeType::END);
}