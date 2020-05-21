#include <cmath>

#include "Grammar.h"
#include "Convolution.h"

/**
 * @brief Здесь описана вся грамматика языка и даже чуть больше.
 */
Grammar::Grammar() {
    // добавляем в язык лексемы

    // Константы и стандартные функции
    std::vector<uint8_t> pi(sizeof(float));
    *const_cast<float *>(reinterpret_cast<const float *>(pi.data())) = 3.1415926f;
    add_lexeme(Lexeme(Lexeme::LexemeType::CONSTANT, "PI", Lexeme::ValueType::FLOAT, pi));

    std::vector<uint8_t> phi(sizeof(float));
    *const_cast<float *>(reinterpret_cast<const float *>(pi.data())) = 1.618f;
    add_lexeme(Lexeme(Lexeme::LexemeType::CONSTANT, "PHI", Lexeme::ValueType::FLOAT, phi));

    std::vector<uint8_t> true_const(sizeof(bool));
    *const_cast<bool *>(reinterpret_cast<const bool *>(true_const.data())) = true;
    add_lexeme(Lexeme(Lexeme::LexemeType::CONSTANT, "true", Lexeme::ValueType::BOOL, true_const));

    std::vector<uint8_t> false_const(sizeof(bool));
    *const_cast<bool *>(reinterpret_cast<const bool *>(true_const.data())) = false;
    add_lexeme(Lexeme(Lexeme::LexemeType::CONSTANT, "false", Lexeme::ValueType::BOOL, false_const));

    static const Lexeme false_lexeme = lexemes_["false"];
    static const Lexeme true_lexeme = lexemes_["true"];

    add_lexeme(Lexeme(Lexeme::LexemeType::IDENTIFIER, "sqrt", [](Lexeme &lhs, Lexeme &rhs) {
        Lexeme result = lhs;
        switch (result.get_type_value()) {
            case Lexeme::ValueType::FLOAT: {
                auto result_data = const_cast<float *>(reinterpret_cast<const float *>(result.get_data().data()));
                *result_data = std::sqrt(*result_data);
                break;
            }
            default:
                std::cerr << "Operands must be of float type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::FACTOR);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::IDENTIFIER, "square", [](Lexeme &lhs, Lexeme &rhs) {
        Lexeme result = lhs;
        switch (result.get_type_value()) {
            case Lexeme::ValueType::INTEGER: {
                auto result_data = const_cast<int32_t *>(reinterpret_cast<const int32_t *>(result.get_data().data()));
                *result_data = *result_data * *result_data;
                break;
            }
            case Lexeme::ValueType::FLOAT: {
                auto result_data = const_cast<float *>(reinterpret_cast<const float *>(result.get_data().data()));
                *result_data = *result_data * *result_data;
                break;
            }
            default:
                std::cerr << "Operands must be of float type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::FACTOR);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::IDENTIFIER, "print", [](Lexeme &lhs, Lexeme &rhs) {
        lhs.print();
        return Lexeme();
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::IDENTIFIER, "len", [](Lexeme &lhs, Lexeme &rhs) {
        std::vector<uint8_t> zero(sizeof(int));
        *const_cast<int32_t *>(reinterpret_cast<const int32_t *>(zero.data())) = 0;
        auto result = Lexeme(Lexeme::LexemeType::FACTOR, "", Lexeme::ValueType::INTEGER, zero);
        auto result_data = const_cast<int32_t *>(reinterpret_cast<const int32_t *>(result.get_data().data()));

        switch (lhs.get_type_value()) {
            case Lexeme::ValueType::ARR: {
                switch (lhs.get_type_subvalue()) {
                    case Lexeme::ValueType::INTEGER: {
                        auto length = lhs.get_data().size() / sizeof(int32_t);
                        *result_data = length;
                        break;
                    }
                    case Lexeme::ValueType::FLOAT: {
                        auto length = lhs.get_data().size() / sizeof(float);
                        *result_data = length;
                        break;
                    }
                    case Lexeme::ValueType::BOOL: {
                        auto length = lhs.get_data().size() / sizeof(bool);
                        *result_data = length;
                        break;
                    }
                    default:
                        auto length = lhs.get_data().size();
                        *result_data = length;
                        break;
                }
            }
                break;
            default:
                std::cerr << "Operand must be of array type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::FACTOR);
        return result;
    }));


    // лексемы-разделители
    add_lexeme(Lexeme(Lexeme::LexemeType::LEFT_CIRCLE_BRACKET, "("));
    add_lexeme(Lexeme(Lexeme::LexemeType::RIGHT_CIRCLE_BRACKET, ")"));
    add_lexeme(Lexeme(Lexeme::LexemeType::LEFT_SQUARE_BRACKET, "["));
    add_lexeme(Lexeme(Lexeme::LexemeType::RIGHT_SQUARE_BRACKET, "]"));
    add_lexeme(Lexeme(Lexeme::LexemeType::END, ";"));
    add_lexeme(Lexeme(Lexeme::LexemeType::COMMA, ","));


    // присваивание
    add_lexeme(Lexeme(Lexeme::LexemeType::ASSIGN, "=", [](Lexeme &lhs, Lexeme &rhs) {
        lhs.set_data(rhs.get_data());
        lhs.set_type_value(rhs.get_type_value());
        if (rhs.get_type_value() == Lexeme::ValueType::ARR) {
            lhs.set_type_subvalue(rhs.get_type_subvalue());
        }
        lhs.print();
        return Lexeme();
    }));


    // арифметические операции
    add_lexeme(Lexeme(Lexeme::LexemeType::PLUS, "+", [](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = lhs;
        switch (result.get_type_value()) {
            case Lexeme::ValueType::INTEGER: {
                auto result_data = const_cast<int32_t *>(reinterpret_cast<const int32_t *>(result.get_data().data()));
                *result_data += *reinterpret_cast<const int32_t *>(rhs.get_data().data());
                break;
            }
            case Lexeme::ValueType::FLOAT: {
                auto result_data = const_cast<float *>(reinterpret_cast<const float *>(result.get_data().data()));
                *result_data += *reinterpret_cast<const float *>(rhs.get_data().data());
                break;
            }
            default:
                std::cerr << "Operands must be of number type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::EXPRESSION);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::MINUS, "-", [](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = lhs;
        switch (result.get_type_value()) {
            case Lexeme::ValueType::INTEGER: {
                auto result_data = const_cast<int32_t *>(reinterpret_cast<const int32_t *>(result.get_data().data()));
                *result_data -= *reinterpret_cast<const int32_t *>(rhs.get_data().data());
                break;
            }
            case Lexeme::ValueType::FLOAT: {
                auto result_data = const_cast<float *>(reinterpret_cast<const float *>(result.get_data().data()));
                *result_data -= *reinterpret_cast<const float *>(rhs.get_data().data());
                break;
            }
            default:
                std::cerr << "Operands must be of number type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::EXPRESSION);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::DIVISION, "/", [](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = lhs;
        switch (result.get_type_value()) {
            case Lexeme::ValueType::INTEGER: {
                auto result_data = const_cast<int32_t *>(reinterpret_cast<const int32_t *>(result.get_data().data()));
                *result_data /= *reinterpret_cast<const int32_t *>(rhs.get_data().data());
                break;
            }
            case Lexeme::ValueType::FLOAT: {
                auto result_data = const_cast<float *>(reinterpret_cast<const float *>(result.get_data().data()));
                *result_data /= *reinterpret_cast<const float *>(rhs.get_data().data());
                break;
            }
            default:
                std::cerr << "Operands must be of number type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::TERM);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::MULTIPLICATION, "*", [](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = lhs;
        switch (result.get_type_value()) {
            case Lexeme::ValueType::INTEGER: {
                auto result_data = const_cast<int32_t *>(reinterpret_cast<const int32_t *>(result.get_data().data()));
                *result_data *= *reinterpret_cast<const int32_t *>(rhs.get_data().data());
                break;
            }
            case Lexeme::ValueType::FLOAT: {
                auto result_data = const_cast<float *>(reinterpret_cast<const float *>(result.get_data().data()));
                *result_data *= *reinterpret_cast<const float *>(rhs.get_data().data());
                break;
            }
            default:
                std::cerr << "Operands must be of number type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::TERM);
        return result;
    }));


    // логические операции
    add_lexeme(Lexeme(Lexeme::LexemeType::LOGICAL_AND, "&&", [](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = lhs;
        switch (result.get_type_value()) {
            case Lexeme::ValueType::BOOL: {
                auto result_data = const_cast<bool *>(reinterpret_cast<const bool *>(result.get_data().data()));
                *result_data &= *reinterpret_cast<const bool *>(rhs.get_data().data());
                break;
            }
            default:
                std::cerr << "Operands must be of boolean type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::EXPRESSION);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::LOGICAL_OR, "||", [](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = lhs;
        switch (result.get_type_value()) {
            case Lexeme::ValueType::BOOL: {
                auto result_data = const_cast<bool *>(reinterpret_cast<const bool *>(result.get_data().data()));
                *result_data |= *reinterpret_cast<const bool *>(rhs.get_data().data());
                break;
            }
            default:
                std::cerr << "Operands must be of boolean type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::EXPRESSION);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::LOGICAL_XOR, "^", [](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = lhs;
        switch (result.get_type_value()) {
            case Lexeme::ValueType::BOOL: {
                auto result_data = const_cast<bool *>(reinterpret_cast<const bool *>(result.get_data().data()));
                *result_data ^= *reinterpret_cast<const bool *>(rhs.get_data().data());
                break;
            }
            default:
                std::cerr << "Operands must be of boolean type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::EXPRESSION);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::LOGICAL_NOT, "!", [](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != Lexeme::ValueType::BOOL) {
            std::cerr << "Operand must be of boolean type." << std::endl;
            return lhs;
        }
        Lexeme result = lhs;
        auto result_data = const_cast<bool *>(reinterpret_cast<const bool *>(result.get_data().data()));
        *result_data = !*result_data;
        result.set_type(Lexeme::LexemeType::TERM);
        return result;
    }));


    // операции сравнения
    add_lexeme(Lexeme(Lexeme::LexemeType::EQUAL, "==", [&true_lexeme](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = true_lexeme;
        auto result_data = const_cast<bool *>(reinterpret_cast<const bool *>(result.get_data().data()));
        *result_data &= (lhs.get_data() == rhs.get_data());
        result.set_type(Lexeme::LexemeType::EXPRESSION);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::NOT_EQUAL, "!=", [&true_lexeme](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = true_lexeme;
        auto result_data = const_cast<bool *>(reinterpret_cast<const bool *>(result.get_data().data()));
        *result_data &= (lhs.get_data() != rhs.get_data());
        result.set_type(Lexeme::LexemeType::EXPRESSION);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::LESS, "<", [&true_lexeme](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = true_lexeme;
        auto result_data = const_cast<bool *>(reinterpret_cast<const bool *>(result.get_data().data()));
        switch (lhs.get_type_value()) {
            case Lexeme::ValueType::INTEGER: {
                *result_data &= *reinterpret_cast<const int32_t *>(lhs.get_data().data()) <
                                *reinterpret_cast<const int32_t *>(rhs.get_data().data());
                break;
            }
            case Lexeme::ValueType::FLOAT: {
                *result_data &= *reinterpret_cast<const float *>(lhs.get_data().data()) <
                                *reinterpret_cast<const float *>(rhs.get_data().data());
                break;
            }
            default:
                std::cerr << "Operands must be of number type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::EXPRESSION);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::GREATER, ">", [&true_lexeme](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = true_lexeme;
        auto result_data = const_cast<bool *>(reinterpret_cast<const bool *>(result.get_data().data()));
        switch (lhs.get_type_value()) {
            case Lexeme::ValueType::INTEGER: {
                *result_data &= *reinterpret_cast<const int32_t *>(lhs.get_data().data()) >
                                *reinterpret_cast<const int32_t *>(rhs.get_data().data());
                break;
            }
            case Lexeme::ValueType::FLOAT: {
                *result_data &= *reinterpret_cast<const float *>(lhs.get_data().data()) >
                                *reinterpret_cast<const float *>(rhs.get_data().data());
                break;
            }
            default:
                std::cerr << "Operands must be of number type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::EXPRESSION);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::GREATER_OR_EQUAL, ">=", [&true_lexeme](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = true_lexeme;
        auto result_data = const_cast<bool *>(reinterpret_cast<const bool *>(result.get_data().data()));
        switch (lhs.get_type_value()) {
            case Lexeme::ValueType::INTEGER: {
                *result_data &= *reinterpret_cast<const int32_t *>(lhs.get_data().data()) >=
                                *reinterpret_cast<const int32_t *>(rhs.get_data().data());
                break;
            }
            case Lexeme::ValueType::FLOAT: {
                *result_data &= *reinterpret_cast<const float *>(lhs.get_data().data()) >=
                                *reinterpret_cast<const float *>(rhs.get_data().data());
                break;
            }
            default:
                std::cerr << "Operands must be of number type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::EXPRESSION);
        return result;
    }));
    add_lexeme(Lexeme(Lexeme::LexemeType::LESS_OR_EQUAL, "<=", [&true_lexeme](Lexeme &lhs, Lexeme &rhs) {
        if (lhs.get_type_value() != rhs.get_type_value()) {
            std::cerr << "Operands must be of the same type." << std::endl;
            return lhs;
        }
        Lexeme result = true_lexeme;
        auto result_data = const_cast<bool *>(reinterpret_cast<const bool *>(result.get_data().data()));
        switch (lhs.get_type_value()) {
            case Lexeme::ValueType::INTEGER: {
                *result_data &= *reinterpret_cast<const int32_t *>(lhs.get_data().data()) <=
                                *reinterpret_cast<const int32_t *>(rhs.get_data().data());
                break;
            }
            case Lexeme::ValueType::FLOAT: {
                *result_data &= *reinterpret_cast<const float *>(lhs.get_data().data()) <=
                                *reinterpret_cast<const float *>(rhs.get_data().data());
                break;
            }
            default:
                std::cerr << "Operands must be of number type." << std::endl;
                return lhs;
        }
        result.set_type(Lexeme::LexemeType::EXPRESSION);
        return result;
    }));


    // добавляем в язык свёртки

    // общего назначения
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::IDENTIFIER},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                lexemes.back().set_type(Lexeme::LexemeType::FACTOR);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::CONSTANT},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                lexemes.back().set_type(Lexeme::LexemeType::FACTOR);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::FACTOR},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                lexemes.back().set_type(Lexeme::LexemeType::TERM);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::TERM},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                lexemes.back().set_type(Lexeme::LexemeType::EXPRESSION);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::STATEMENT, Lexeme::LexemeType::END},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                lexemes.erase(lexemes.end() - 2, lexemes.end());
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::IDENTIFIER, Lexeme::LexemeType::ASSIGN, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                grammar.add_lexeme(*(lexemes.end() - 3));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.emplace_back(Lexeme::LexemeType::STATEMENT, "");
            }));


    // арифметические операции
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::TERM, Lexeme::LexemeType::PLUS, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::TERM, Lexeme::LexemeType::MINUS, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::TERM, Lexeme::LexemeType::MULTIPLICATION, Lexeme::LexemeType::FACTOR},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::TERM, Lexeme::LexemeType::DIVISION, Lexeme::LexemeType::FACTOR},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));


    // приоритет операций
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::LEFT_CIRCLE_BRACKET, Lexeme::LexemeType::EXPRESSION,
             Lexeme::LexemeType::RIGHT_CIRCLE_BRACKET},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = *(lexemes.end() - 2);
                result.set_type(Lexeme::LexemeType::FACTOR);
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));


    // логические операции
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::TERM, Lexeme::LexemeType::LOGICAL_AND, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::TERM, Lexeme::LexemeType::LOGICAL_OR, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::TERM, Lexeme::LexemeType::LOGICAL_XOR, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::LOGICAL_NOT, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto dummy = Lexeme();
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 1), dummy);
                lexemes.erase(lexemes.end() - 2, lexemes.end());
                lexemes.push_back(result);
            }));


    // операции сравнения
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::EXPRESSION, Lexeme::LexemeType::EQUAL, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::EXPRESSION, Lexeme::LexemeType::NOT_EQUAL, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::EXPRESSION, Lexeme::LexemeType::LESS, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::EXPRESSION, Lexeme::LexemeType::GREATER, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::EXPRESSION, Lexeme::LexemeType::LESS_OR_EQUAL, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::EXPRESSION, Lexeme::LexemeType::GREATER_OR_EQUAL, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto result = (lexemes.end() - 2)->exec_action(*(lexemes.end() - 3), *(lexemes.end() - 1));
                lexemes.erase(lexemes.end() - 3, lexemes.end());
                lexemes.push_back(result);
            }));


    //вызов функций одного переменного
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::IDENTIFIER, Lexeme::LexemeType::LEFT_CIRCLE_BRACKET, Lexeme::LexemeType::EXPRESSION,
             Lexeme::LexemeType::RIGHT_CIRCLE_BRACKET},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                auto dummy = Lexeme();
                auto result = (lexemes.end() - 4)->exec_action(*(lexemes.end() - 2), dummy);
                lexemes.erase(lexemes.end() - 4, lexemes.end());
                lexemes.push_back(result);
            }));


    // массивы
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                lexemes.back().set_type(Lexeme::LexemeType::LIST);
                lexemes.back().set_type_subvalue(lexemes.back().get_type_value());
                lexemes.back().set_type_value(Lexeme::ValueType::ARR);
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::LIST, Lexeme::LexemeType::END},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                lexemes.erase(lexemes.end() - 2, lexemes.end());
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::LIST, Lexeme::LexemeType::COMMA, Lexeme::LexemeType::EXPRESSION},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                if ((lexemes.end() - 3)->get_type_subvalue() != (lexemes.end() - 1)->get_type_value()) {
                    std::cerr << "Operands must be of the same type." << std::endl;
                    lexemes.erase(lexemes.end() - 2, lexemes.end());
                    return;
                }
                auto data_old = (lexemes.end() - 3)->get_data();
                auto data_new = (lexemes.end() - 1)->get_data();
                data_old.insert(data_old.end(), data_new.begin(), data_new.end());

                (lexemes.end() - 3)->set_data(data_old);

                lexemes.erase(lexemes.end() - 2, lexemes.end());
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::IDENTIFIER, Lexeme::LexemeType::ASSIGN, Lexeme::LexemeType::LEFT_SQUARE_BRACKET,
             Lexeme::LexemeType::LIST, Lexeme::LexemeType::RIGHT_SQUARE_BRACKET},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                (lexemes.end() - 4)->exec_action(*(lexemes.end() - 5), *(lexemes.end() - 2));
                grammar.add_lexeme(*(lexemes.end() - 5));
                lexemes.erase(lexemes.end() - 5, lexemes.end());
                lexemes.emplace_back(Lexeme::LexemeType::STATEMENT, "");
            }));
    convolutions_.push_back(Convolution(
            {Lexeme::LexemeType::IDENTIFIER, Lexeme::LexemeType::LEFT_SQUARE_BRACKET, Lexeme::LexemeType::EXPRESSION,
             Lexeme::LexemeType::RIGHT_SQUARE_BRACKET},
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                if ((lexemes.end() - 2)->get_type_value() != Lexeme::ValueType::INTEGER) {
                    std::cerr << "Index must be of the integer type." << std::endl;
                    lexemes.erase(lexemes.end() - 4, lexemes.end());
                    return;
                }
                auto index = *reinterpret_cast<const int32_t *>((lexemes.end() - 2)->get_data().data());
                Lexeme result = Lexeme();
                std::string result_code = (lexemes.end() - 4)->get_code() + "[" + std::to_string(index) + "]";
                switch ((lexemes.end() - 4)->get_type_value()) {
                    case Lexeme::ValueType::ARR: {
                        switch ((lexemes.end() - 4)->get_type_subvalue()) {
                            case Lexeme::ValueType::INTEGER: {
                                auto array_data = reinterpret_cast<const int32_t *>((lexemes.end() -
                                                                                     4)->get_data().data());
                                std::vector<uint8_t> value(sizeof(int));
                                *const_cast<int32_t *>(reinterpret_cast<const int32_t *>(value.data())) = array_data[index];
                                result = Lexeme(Lexeme::LexemeType::FACTOR, result_code, Lexeme::ValueType::INTEGER,
                                                value);
                                break;
                            }
                            case Lexeme::ValueType::FLOAT: {
                                auto array_data = reinterpret_cast<const float *>((lexemes.end() -
                                                                                   4)->get_data().data());
                                std::vector<uint8_t> value(sizeof(float));
                                *const_cast<float *>(reinterpret_cast<const float *>(value.data())) = array_data[index];
                                result = Lexeme(Lexeme::LexemeType::FACTOR, result_code, Lexeme::ValueType::FLOAT,
                                                value);
                                break;
                            }
                            case Lexeme::ValueType::BOOL: {
                                auto array_data = reinterpret_cast<const bool *>((lexemes.end() -
                                                                                  4)->get_data().data());
                                std::vector<uint8_t> value(sizeof(bool));
                                *const_cast<bool *>(reinterpret_cast<const bool *>(value.data())) = array_data[index];
                                result = Lexeme(Lexeme::LexemeType::FACTOR, result_code, Lexeme::ValueType::BOOL,
                                                value);
                            }
                            default:
                                auto array_data = (lexemes.end() - 4)->get_data().data();
                                std::vector<uint8_t> value(sizeof(uint8_t));
                                *const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(value.data())) = array_data[index];
                                result = Lexeme(Lexeme::LexemeType::FACTOR, result_code, Lexeme::ValueType::INTEGER,
                                                value);
                                break;
                        }
                    }
                        break;
                    default:
                        std::cerr << "Operand must be of array type." << std::endl;
                        return;
                }
                result.set_type(Lexeme::LexemeType::FACTOR);
                lexemes.erase(lexemes.end() - 4, lexemes.end());
                lexemes.push_back(result);
                return;
            }));
    convolutions_.push_back(Convolution(
            {
                    Lexeme::LexemeType::IDENTIFIER,
                    Lexeme::LexemeType::LEFT_SQUARE_BRACKET,
                    Lexeme::LexemeType::EXPRESSION,
                    Lexeme::LexemeType::RIGHT_SQUARE_BRACKET,
                    Lexeme::LexemeType::ASSIGN,
                    Lexeme::LexemeType::EXPRESSION,
            },
            [](std::vector<Lexeme> &lexemes, Grammar &grammar) {
                if ((lexemes.end() - 4)->get_type_value() != Lexeme::ValueType::INTEGER) {
                    std::cerr << "Index must be of the integer type." << std::endl;
                    lexemes.erase(lexemes.end() - 6, lexemes.end());
                    return;
                }
                if ((lexemes.end() - 6)->get_type_subvalue() != (lexemes.end() - 1)->get_type_value()){
                    std::cerr << "Operands must be of the same type." << std::endl;
                    lexemes.erase(lexemes.end() - 6, lexemes.end());
                    return;
                }
                auto index = *reinterpret_cast<const int32_t *>((lexemes.end() - 4)->get_data().data());
                Lexeme result = *(lexemes.end() - 6);
                switch (result.get_type_value()) {
                    case Lexeme::ValueType::ARR: {
                        switch (result.get_type_subvalue()) {
                            case Lexeme::ValueType::INTEGER: {
                                auto array_data = const_cast<int32_t*>(reinterpret_cast<const int32_t *>(result.get_data().data()));
                                array_data[index] = *reinterpret_cast<const int32_t *>((lexemes.end() - 1)->get_data().data());
                                break;
                            }
                            case Lexeme::ValueType::FLOAT: {
                                auto array_data = const_cast<float*>(reinterpret_cast<const float *>(result.get_data().data()));
                                array_data[index] = *reinterpret_cast<const float *>((lexemes.end() - 1)->get_data().data());
                                break;
                            }
                            case Lexeme::ValueType::BOOL: {
                                auto array_data = const_cast<bool*>(reinterpret_cast<const bool *>(result.get_data().data()));
                                array_data[index] = *reinterpret_cast<const bool *>((lexemes.end() - 1)->get_data().data());
                            }
                            default:
                                auto array_data = const_cast<uint8_t *>(result.get_data().data());
                                array_data[index] = *(lexemes.end() - 1)->get_data().data();
                                break;
                        }
                    }
                        break;
                    default:
                        std::cerr << "Operand must be of array type." << std::endl;
                        return;
                }
                lexemes.erase(lexemes.end() - 6, lexemes.end());
                lexemes.emplace_back(Lexeme::LexemeType::STATEMENT, "");
                grammar.add_lexeme(result);
                result.print();
                return;
            }));

}

void Grammar::add_lexeme(const Lexeme &lexeme) {
    lexemes_[lexeme.get_code()] = lexeme;
}

std::optional<Lexeme> Grammar::find_lexeme(const std::string &code) const {
    auto found = lexemes_.find(code);
    if (found == lexemes_.end()) {
        return std::nullopt;
    }
    return found->second;
}

bool Grammar::need_next(const std::vector<Lexeme> &current_sequence, const Lexeme &next) {
    if (current_sequence.empty()) {
        return true;
    }
    if (current_sequence.back().need_next(next.get_type())) {
        return true;
    }
    std::vector<Lexeme::LexemeType> pattern;
    for (const auto &lexeme: current_sequence) {
        pattern.push_back(lexeme.get_type());
    }
    pattern.push_back(next.get_type());
    for (const auto &conv: convolutions_) {
        if (conv.is_end(pattern) || conv.starts_with(pattern)) {
            return true;
        }
    }
    return false;
}

const std::optional<Convolution> Grammar::find_convolution(const std::vector<Lexeme> &current_sequence) {
    std::vector<Lexeme::LexemeType> pattern;
    for (const auto &lexeme: current_sequence) {
        pattern.push_back(lexeme.get_type());
    }
    auto iter_found = std::find_if(
            convolutions_.begin(),
            convolutions_.end(),
            [&pattern](const auto &conv) { return conv.is_end(pattern); });
    if (iter_found == convolutions_.end()) {
        return std::nullopt;
    }
    auto iter_max = iter_found;
    while (iter_found != convolutions_.end()) {
        iter_found = std::find_if(
                iter_found + 1,
                convolutions_.end(),
                [&pattern](const auto &conv) { return conv.is_end(pattern); });
        if (iter_found->size() > iter_max->size()) {
            iter_max = iter_found;
        }
    }
    return *iter_max;
}

