#include "Lexer.h"

std::optional<Token> Lexer::next() {
    char c;
    while(*in >> c) {
        auto symbol = from_symbol(c);
        if(symbol) {
            return Token{*symbol, in->tellg()};
        }
    }

    return std::nullopt;
}