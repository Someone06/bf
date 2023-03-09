#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <ranges>
#include <variant>
#include <vector>

#include "AST.h"
#include "Token.h"
#include "TokenType.h"
#include "format_string.h"

template<std::input_iterator T>
requires std::move_constructible<T>
class InputRange final {
public:
   explicit InputRange(T begin, T end) : b{std::move(begin)}, e{std::move(end)} {}

   T begin() { return b; }
   T end() { return e; }

private:
   T b;
   T e;
};

template<std::input_iterator T>
requires std::same_as<std::iter_value_t<T>, char> && std::move_constructible<T>
std::ranges::input_range auto lex(T begin, T end = T{}) {
    InputRange r {std::move(begin), std::move(end)};

    auto toSymbol = [line=0, col=0] (char c) mutable {
        if(c == '\n') {
            ++line;
            col = 0;
        } else {
            ++col;
        }

        return std::make_tuple(from_symbol(c), line, col);
    };

    auto isSymbol = [](auto t) {
        return std::get<0>(t).has_value();
    };

    auto toToken = [](auto t) {
        return Token(*std::get<0>(t), std::get<1>(t), std::get<2>(t));
    };

    return r | std::views::transform(toSymbol)
             | std::views::filter(isSymbol)
             | std::views::transform(toToken);
}

template<std::ranges::input_range T>
requires std::same_as<std::ranges::range_value_t<T>, Token>
std::variant<AST, std::string> parse(T tokens) {
   std::vector<std::vector<std::unique_ptr<Node>>> stack {};
   std::vector<Token> leftTokens {};
   std::vector<std::unique_ptr<Node>>* cur {&stack.emplace_back()};

    for(Token t : tokens) {
       switch (t.kind()) {
           case TokenType::Inc:
               cur->push_back(std::make_unique<Right>(t, 1));
               break;
           case TokenType::Dec:
               cur->push_back(std::make_unique<Left>(t, 1));
               break;
           case TokenType::Add:
               cur->push_back(std::make_unique<Inc>(t, 1));
               break;
           case TokenType::Sub:
               cur->push_back(std::make_unique<Dec>(t, 1));
               break;
           case TokenType::In:
               cur->push_back(std::make_unique<In>(t));
               break;
           case TokenType::Out:
               cur->push_back(std::make_unique<Out>(t));
               break;
           case TokenType::Left:
               cur = &stack.emplace_back();
               leftTokens.push_back(t);
               break;
           case TokenType::Right:
               if (leftTokens.empty()) {
                   auto msg = format_string("Error: Unexpected token '%s' at line '%d', column '%d'.", t, t.row(), t.col());
                   return std::variant<AST, std::string>{std::move(msg)};
               }

               assert(stack.size() >= 2);
               std::vector<std::unique_ptr<Node>> tmp{};
               tmp.swap(stack.back());
               stack.pop_back();
               cur = &stack.back();
               cur->push_back(std::make_unique<While>(
                       leftTokens.back(), t, std::move(tmp)));
               leftTokens.pop_back();
               break;
       }
   };

   if(!leftTokens.empty()) {
       Token t = leftTokens.back();
       auto msg = format_string("Error: Unmatched token '%s' at  line '%d', column '%d'", t, t.row(), t.col());
       return std::variant<AST, std::string>{std::move(msg)};
   }

   assert(stack.size() == 1);
   return std::variant<AST, std::string> {AST {std::move(stack.at(1))}};
}

template<std::input_iterator T>
requires std::same_as<std::iter_value_t<T>, char>
        std::variant<AST, std::string> lexAndParse(T begin, T end) {
    std::ranges::input_range auto lexed = lex(begin, end);
    return parse(lexed);
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cerr << "Args: Input file";
        return 1;
    }

    std::ifstream in{argv[1]};
    std::istreambuf_iterator<char> b {in};
    std::istreambuf_iterator<char> e {};
    auto parsed = lexAndParse(b, e);
    if(std::holds_alternative<std::string>(parsed)) {
        std::cerr << std::get<std::string>(parsed);
    }
}


