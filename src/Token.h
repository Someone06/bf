#ifndef BF_TOKEN_H
#define BF_TOKEN_H

#include<ostream>
#include "TokenType.h"


class Token final {
public:
    using position_t = int;
    Token(TokenType kind, position_t row, position_t col): kind_{kind}, r{row}, c{col} {}

    [[nodiscard]] TokenType kind() const noexcept {
        return kind_;
    }

    [[nodiscard]] position_t row() const noexcept {
        return r;
    }

    [[nodiscard]] position_t col() const noexcept {
        return c;
    }

private:
    TokenType kind_;
    int r;
    int c;
};

std::ostream& operator<<(std::ostream& os, const Token& token);

#endif
