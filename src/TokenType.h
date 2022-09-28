#ifndef BF_TOKENTYPE_H
#define BF_TOKENTYPE_H

#include <optional>
#include <string>

enum class TokenType {
    Inc,
    Dec,
    Add,
    Sub,
    In,
    Out,
    Left,
    Right,
};

[[nodiscard]] std::string_view to_symbol(TokenType kind);

[[nodiscard]] std::optional<TokenType> from_symbol(char symbol);

[[nodiscard]] std::string_view to_string(TokenType kind);

std::ostream& operator<<(std::ostream& os, const TokenType& kind);

#endif
