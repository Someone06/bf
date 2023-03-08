#include "TokenType.h"
[[nodiscard]] std::string_view to_symbol(TokenType kind) {
    switch(kind) {
        case TokenType::Inc:
            return {">"};
        case TokenType::Dec:
            return {"<"};
        case TokenType::Add:
            return {"+"};
        case TokenType::Sub:
            return {"-"};
        case TokenType::In:
            return {","};
        case TokenType::Out:
            return {"."};
        case TokenType::Left:
            return {"["};
        case TokenType::Right:
            return {"]"};
        default:
            throw std::logic_error("Unreachable!");
    }
}

[[nodiscard]] std::optional<TokenType> from_symbol(const char symbol) {
    switch(symbol) {
        case '>':
            return TokenType::Inc;
        case '<':
            return TokenType::Dec;
        case '+':
            return TokenType::Add;
        case '-':
            return TokenType::Sub;
        case ',':
            return TokenType::In;
        case '.':
            return TokenType::Out;
        case '[':
            return TokenType::Left;
        case ']':
            return TokenType::Right;
        default:
            return {};
    }
}

[[nodiscard]] std::string_view to_string(TokenType kind) {
    switch(kind) {
        case TokenType::Inc:
            return {"Inc"};
        case TokenType::Dec:
            return {"Dec"};
        case TokenType::Add:
            return {"Add"};
        case TokenType::Sub:
            return {"Sub"};
        case TokenType::In:
            return {"In"};
        case TokenType::Out:
            return {"Out"};
        case TokenType::Left:
            return {"Left"};
        case TokenType::Right:
            return {"Right"};
        default:
            throw std::logic_error("Unreachable!");
    }
}

std::ostream& operator<<(std::ostream& os, const TokenType& kind) {
    return (os << to_string(kind));
}