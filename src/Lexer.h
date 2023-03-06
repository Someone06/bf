#ifndef BF_LEXER_H
#define BF_LEXER_H

#include <fstream>
#include <iterator>
#include <memory>
#include <ranges>
#include <utility>

#include "Token.h"

class Lexer final {
public:
    explicit Lexer(std::unique_ptr<std::istream> input): in{std::move(input)} {}

    std::optional<Token> next();

private:
    std::unique_ptr<std::istream> in;
};

#endif
