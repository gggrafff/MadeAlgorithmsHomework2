//
// Created by ants on 10.05.20.
//

#include "Syntaxer.h"

void Syntaxer::execute(const std::vector<Lexeme> &lexemes_input, Grammar &grammar) {
    size_t cursor_input = 0;
    std::vector<Lexeme> current_state;
    while(cursor_input<lexemes_input.size())
    {
        while ((cursor_input < lexemes_input.size()) && grammar.need_next(current_state, lexemes_input[cursor_input]))
        {
            // пока нужно читать следующий символ, читаем его
            current_state.push_back(lexemes_input[cursor_input]);
            cursor_input += 1;
        }
        // производим свёртку
        auto conv = grammar.find_convolution(current_state);
        if (!conv.has_value()){
            std::cerr << "No suitable convolution was found for ";
            for(const auto& lexeme: current_state){
                std::cerr << Lexeme::LexemeTypeToString(lexeme.get_type()) << " ";
            }
            std::cerr << std::endl;
            break;
        } else {
            conv.value().apply(current_state, grammar);
        }
    }
}
