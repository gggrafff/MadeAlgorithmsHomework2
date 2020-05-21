//
// Created by ants on 14.05.20.
//

#include "Grammar.h"
#include <cmath>
#include "Lexeme.h"

const std::__cxx11::string &Lexeme::get_code() const {
    return code_;
}

void Lexeme::set_code(const std::__cxx11::string &code) {
    Lexeme::code_ = code;
}

Lexeme::LexemeType Lexeme::get_type() const {
    return type_;
}

void Lexeme::set_type(Lexeme::LexemeType type) {
    Lexeme::type_ = type;
}

Lexeme::ValueType Lexeme::get_type_value() const {
    return type_value_;
}

void Lexeme::set_type_value(Lexeme::ValueType type_value) {
    Lexeme::type_value_ = type_value;
}

Lexeme::ValueType Lexeme::get_type_subvalue() const {
    return type_subvalue_;
}

void Lexeme::set_type_subvalue(Lexeme::ValueType type_subvalue) {
    Lexeme::type_subvalue_ = type_subvalue;
}

const std::vector<uint8_t> &Lexeme::get_data() const {
    return data_;
}

void Lexeme::set_data(const std::vector<uint8_t> &data) {
    Lexeme::data_ = data;
}

std::__cxx11::string Lexeme::LexemeTypeToString(Lexeme::LexemeType type) {
    switch (type) {
        case IDENTIFIER:
            return "IDENTIFIER";
        case CONSTANT:
            return "CONSTANT";
        case FACTOR:
            return "FACTOR";
        case TERM:
            return "TERM";
        case PLUS:
            return "PLUS";
        case ASSIGN:
            return "ASSIGN";
        case EXPRESSION:
            return "EXPRESSION";
        case STATEMENT:
            return "STATEMENT";
        case MINUS:
            return "MINUS";
        case MULTIPLICATION:
            return "MULTIPLICATION";
        case DIVISION:
            return "DIVISION";

        case LEFT_CIRCLE_BRACKET:
            return "LEFT_CIRCLE_BRACKET";
        case RIGHT_CIRCLE_BRACKET:
            return "RIGHT_CIRCLE_BRACKET";
        case LEFT_SQUARE_BRACKET:
            return "LEFT_SQUARE_BRACKET";
        case RIGHT_SQUARE_BRACKET:
            return "RIGHT_SQUARE_BRACKET";
        case END:
            return "END";
        case COMMA:
            return "COMMA";

        case LOGICAL_AND:
            return "LOGICAL_AND";
        case LOGICAL_XOR:
            return "LOGICAL_XOR";
        case LOGICAL_OR:
            return "LOGICAL_OR";
        case LOGICAL_NOT:
            return "LOGICAL_NOT";

        case LESS_OR_EQUAL:
            return "LESS_OR_EQUAL";
        case GREATER_OR_EQUAL:
            return "GREATER_OR_EQUAL";
        case LESS:
            return "LESS";
        case GREATER:
            return "GREATER";
        case EQUAL:
            return "EQUAL";
        case NOT_EQUAL:
            return "NOT_EQUAL";

        case LIST:
            return "LIST";

        default:
            return "Unknown lexeme";
    }
}

bool Lexeme::operator==(const Lexeme &rhs) const {
    return ((this->type_ == rhs.type_) && (this->code_ == rhs.code_));
}

bool Lexeme::operator!=(const Lexeme &rhs) const { return !(*this == rhs); }

bool Lexeme::need_next(Lexeme::LexemeType next_type) const {
    if (need_next_always()) {
        return true;
    }
    auto next_types = need_next_types();
    auto iter_found = find(next_types.begin(), next_types.end(), next_type);
    return iter_found != next_types.end();
}

void Lexeme::print() const {
    std::cout << "\t";
    if (is_value()) {
        switch (get_type_value()) {
            case Lexeme::ValueType::INTEGER: {
                std::cout << code_ << " = " << *reinterpret_cast<const int32_t *>(get_data().data()) << std::endl;
                break;
            }
            case Lexeme::ValueType::FLOAT: {
                std::cout << code_ << " = " << *reinterpret_cast<const float *>(get_data().data()) << std::endl;
                break;
            }
            case Lexeme::ValueType::BOOL: {
                std::cout << code_ << " = " << (*reinterpret_cast<const bool *>(get_data().data()) ? "true" : "false") << std::endl;
                break;
            }
            case Lexeme::ValueType::ARR: {
                std::cout << code_ << " = array[ ";
                switch (get_type_subvalue()) {
                    case Lexeme::ValueType::INTEGER:{
                        auto data = reinterpret_cast<const int32_t *>(get_data().data());
                        for (size_t i = 0; i < get_data().size() / sizeof(int32_t); ++i)
                        {
                            std::cout << *data << " ";
                            ++data;
                        }
                        break;
                    }
                    case Lexeme::ValueType::FLOAT:{
                        auto data = reinterpret_cast<const float *>(get_data().data());
                        for (size_t i = 0; i < get_data().size() / sizeof(float); ++i)
                        {
                            std::cout << *data << " ";
                            ++data;
                        }
                        break;
                    }
                    case Lexeme::ValueType::BOOL:{
                        auto data = reinterpret_cast<const bool *>(get_data().data());
                        for (size_t i = 0; i < get_data().size() / sizeof(bool); ++i)
                        {
                            std::cout << *data << " ";
                            ++data;
                        }
                        break;
                    }
                    default:{
                        auto data = get_data().data();
                        for (size_t i = 0; i < get_data().size() / sizeof(uint8_t); ++i)
                        {
                            std::cout << static_cast<short>(*data) << " ";
                            ++data;
                        }
                        break;
                    }
                }
                std::cout << "]" << std::endl;
                break;
            }
            default:
                std::cout << "Unknown type of value." << std::endl;
                break;
        }
    } else if (is_action()) {
        std::cout << code_ << " = function" << std::endl;
    } else {
        std::cout << code_ << std::endl;
    }
}

void Lexeme::debug_print() const {
    std::cerr << "Lexeme '" << code_ << "': " << std::endl;
    std::cerr << "Type\t" << LexemeTypeToString(type_) << std::endl;
    if (is_value()) {
        std::cerr << "Contains value: ";
        switch (get_type_value()) {
            case Lexeme::ValueType::INTEGER: {
                std::cerr << *reinterpret_cast<const int32_t *>(get_data().data()) << std::endl;
                break;
            }
            case Lexeme::ValueType::FLOAT: {
                std::cerr << *reinterpret_cast<const float *>(get_data().data()) << std::endl;
                break;
            }
            case Lexeme::ValueType::BOOL: {
                std::cerr << code_ << " = " << (*reinterpret_cast<const bool *>(get_data().data()) ? "true" : "false") << std::endl;
                break;
            }
            case Lexeme::ValueType::ARR: {
                std::cerr << code_ << " = array[ ";
                switch (get_type_subvalue()) {
                    case Lexeme::ValueType::INTEGER:{
                        auto data = reinterpret_cast<const int32_t *>(get_data().data());
                        for (size_t i = 0; i < get_data().size() / sizeof(int32_t); ++i)
                        {
                            std::cerr << *data << " ";
                            ++data;
                        }
                        break;
                    }
                    case Lexeme::ValueType::FLOAT:{
                        auto data = reinterpret_cast<const float *>(get_data().data());
                        for (size_t i = 0; i < get_data().size() / sizeof(float); ++i)
                        {
                            std::cerr << *data << " ";
                            ++data;
                        }
                        break;
                    }
                    case Lexeme::ValueType::BOOL:{
                        auto data = reinterpret_cast<const bool *>(get_data().data());
                        for (size_t i = 0; i < get_data().size() / sizeof(bool); ++i)
                        {
                            std::cerr << *data << " ";
                            ++data;
                        }
                        break;
                    }
                    default:{
                        auto data = get_data().data();
                        for (size_t i = 0; i < get_data().size() / sizeof(uint8_t); ++i)
                        {
                            std::cerr << static_cast<short>(*data) << " ";
                            ++data;
                        }
                        break;
                    }
                }
                std::cerr << "]" << std::endl;
                break;
            }
            default:
                std::cerr << "Unknown type of value." << std::endl;
                break;
        }
    }
    if (is_value()) {
        std::cerr << "Contains action" << std::endl;
    }
}

bool Lexeme::is_action() const {return static_cast<bool>(action_);}

bool Lexeme::is_value() const {return !data_.empty();}

Lexeme Lexeme::exec_action(Lexeme &lhs, Lexeme &rhs) const {
    return action_(lhs, rhs);
}

bool Lexeme::need_next_always() const {
    switch (type_){
        case ASSIGN:
        case PLUS:
        case MINUS:
        case MULTIPLICATION:
        case DIVISION:
        case LEFT_CIRCLE_BRACKET:
        case LOGICAL_AND:
        case LOGICAL_OR:
        case LOGICAL_XOR:
        case LOGICAL_NOT:
        case EQUAL:
        case NOT_EQUAL:
        case LESS:
        case GREATER:
        case LESS_OR_EQUAL:
        case GREATER_OR_EQUAL:
        case LIST:
        case COMMA:
            return true;
        default:
            return false;
    }
}

std::vector<Lexeme::LexemeType> Lexeme::need_next_types() const {
    switch (type_){
        case TERM:
            return {PLUS, MINUS, MULTIPLICATION, DIVISION, LOGICAL_OR, LOGICAL_XOR, LOGICAL_AND};
        case IDENTIFIER:
            return {ASSIGN, LEFT_CIRCLE_BRACKET, LEFT_SQUARE_BRACKET};
        case STATEMENT:
            return {END};
        case EXPRESSION:
            return {EQUAL, NOT_EQUAL, LESS, GREATER, LESS_OR_EQUAL, GREATER_OR_EQUAL};
        default:
            return {};
    }
}