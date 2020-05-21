#include <iostream>

#include "Lexer.h"


std::vector<Lexeme> Lexer::parse_code(const std::string &code, const Grammar &grammar) {
    std::vector<Lexeme> lexemes;
    size_t current = 0;
    while (current < code.size()) {
        if (std::isspace(code[current])) {  // пробелы просто игнорируем
            current += 1;
            continue;
        } else if (std::isdigit(code[current])) {
            // parse numeric constant
            std::string lexeme_text;
            bool is_integer = true;
            while (current < code.size() && (std::isdigit(code[current]) || code[current] == '.')) {
                if (code[current] == '.') {
                    is_integer = false;
                }
                lexeme_text += code[current];
                current += 1;
            }
            auto found = grammar.find_lexeme(lexeme_text);  // ищем лексему в грамматике языка
            if (found.has_value()) {
                lexemes.push_back(found.value());
            } else {  // если не нашли, то создаём лексему, соответствующую константе
                if (is_integer) {
                    auto value = std::stoi(lexeme_text);
                    std::vector<uint8_t> data(sizeof(value));
                    *const_cast<int32_t *>(reinterpret_cast<const int32_t *>(data.data())) = value;
                    Lexeme lexeme(Lexeme::LexemeType::CONSTANT, lexeme_text, Lexeme::ValueType::INTEGER, data);
                    lexemes.push_back(lexeme);
                } else {
                    auto value = std::stof(lexeme_text);
                    std::vector<uint8_t> data(sizeof(value));
                    *const_cast<float *>(reinterpret_cast<const float *>(data.data())) = value;
                    Lexeme lexeme(Lexeme::LexemeType::CONSTANT, lexeme_text, Lexeme::ValueType::FLOAT, data);
                    lexemes.push_back(lexeme);
                }
            }
        } else if ((std::ispunct(code[current])) && !isbracket(code[current])) {  // оператор
            // parse operator or keyword
            std::string lexeme_text;
            while ((current < code.size()) && std::ispunct(code[current]) && !isbracket(code[current])) {
                lexeme_text += code[current];
                current += 1;
            }
            auto found = grammar.find_lexeme(lexeme_text);
            if (found.has_value()) {
                lexemes.push_back(found.value());
            } else {
                std::cerr << "Unknown lexeme: " << lexeme_text << std::endl;
                throw std::runtime_error("Unknown lexeme: " + lexeme_text);
            }
        }  else if (isbracket(code[current])) {  // скобка
            // parse bracket
            std::string lexeme_text;
            lexeme_text += code[current];
            current += 1;
            auto found = grammar.find_lexeme(lexeme_text);
            if (found.has_value()) {
                lexemes.push_back(found.value());
            } else {
                std::cerr << "Unknown lexeme: " << lexeme_text << std::endl;
                throw std::runtime_error("Unknown lexeme: " + lexeme_text);
            }
        } else if (std::isalpha(code[current])) {  // идентификатор
            // parse id
            std::string lexeme_text;
            while (current < code.size() && (std::isalpha(code[current]) || std::isdigit(code[current]))) {
                lexeme_text += code[current];
                current += 1;
            }
            auto found = grammar.find_lexeme(lexeme_text);
            if (found.has_value()) {
                lexemes.push_back(found.value());
            } else {
                Lexeme lexeme(Lexeme::LexemeType::IDENTIFIER, lexeme_text);
                lexemes.push_back(lexeme);
            }
        }
    }
    return lexemes;
}

bool Lexer::isbracket(char symbol) {
    switch (symbol){
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
            return true;
        default:
            return false;
    }
}
