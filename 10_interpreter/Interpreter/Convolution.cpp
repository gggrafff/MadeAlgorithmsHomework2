#include "Convolution.h"
#include "Grammar.h"

bool Convolution::starts_with(const std::vector<Lexeme::LexemeType> &pattern) const {
    if (pattern.size() > from_.size()) {
        return false;
    }
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] != from_[i]) {
            return false;
        }
    }
    return true;
}

bool Convolution::ends_with(const std::vector<Lexeme::LexemeType> &pattern) const {
    if (pattern.size() > from_.size()) {
        return false;
    }
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[pattern.size() - 1 - i] != from_[from_.size() - 1 - i]) {
            return false;
        }
    }
    return true;
}

bool Convolution::is_end(const std::vector<Lexeme::LexemeType> &sequence) const {
    if (sequence.size() < from_.size()) {
        return false;
    }
    for (size_t i = 0; i < from_.size(); ++i) {
        if (sequence[sequence.size() - 1 - i] != from_[from_.size() - 1 - i]) {
            return false;
        }
    }
    return true;
}

bool Convolution::match(const std::vector<Lexeme::LexemeType> &pattern) const {
    return (pattern.size() == from_.size()) && starts_with(pattern);
}

void Convolution::apply(std::vector<Lexeme> &lexemes, Grammar &grammar) {
    action_(lexemes, grammar);
}

void Convolution::debug_print() const {
    std::cerr << "Convolution of a sequence: ";
    for (auto type: from_) {
        std::cerr << Lexeme::LexemeTypeToString(type) << " ";
    }
    std::cerr << std::endl;
}