#include <string>
#include <iostream>

#include "Grammar.h"
#include "Lexer.h"
#include "Syntaxer.h"

int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    Grammar grammar;

    if (argc > 1) {
        if (std::string(argv[1]) == "-c")
        {
            return 0;
        }

        if (std::string(argv[1]) == "-f")
        {
            return 0;
        }
    }

    std::string command;
    while(getline(std::cin, command))
    {
        if (command == "exit;") {
            break;
        }
        auto lexemes = Lexer::parse_code(command, grammar);
        Syntaxer::execute(lexemes, grammar);
    }
}