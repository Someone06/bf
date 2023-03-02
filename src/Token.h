#ifndef BF_TOKEN_H
#define BF_TOKEN_H

#include<ostream>
#include "TokenType.h"


class Token final {
public:
    using pos_type = std::char_traits<char>::pos_type;

    Token(TokenType kind, pos_type position): kind_{kind}, position_{position}{}

    [[nodiscard]] TokenType kind() const noexcept {
        return kind_;
    }

    [[nodiscard]] pos_type position() const noexcept {
        return position_;
    }

private:
    TokenType kind_;
    pos_type position_;
};

std::ostream& operator<<(std::ostream& os, const Token& token);

#endif
