#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <variant>
#include <vector>

#include "AST.h"
#include "AstVisitors.h"
#include "Token.h"
#include "TokenType.h"
#include "format_string.h"

template<typename T>
requires std::move_constructible<T> && std::constructible_from<std::istream_iterator<char>, T&>
class InputRange final {
public:
   explicit InputRange(T source) : src{std::move(source)} {}

   std::input_iterator auto begin() { return std::istream_iterator<char>{src}; }
   std::input_iterator auto end() { return std::istream_iterator<char>{}; }

private:
   T src;
};

template<typename T>
std::ranges::input_range auto lex(InputRange<T>& r) {

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

template<TokenType> struct TokenTypeToASTType {};
#define CONV(from, to) template<> struct TokenTypeToASTType<from> {using type = to;};
    CONV(TokenType::Add, Inc)
    CONV(TokenType::Sub, Dec)
    CONV(TokenType::Inc, Right)
    CONV(TokenType::Dec, Left)
    CONV(TokenType::In, In)
    CONV(TokenType::Out, Out)
#undef CONV

template <TokenType EnumVal>
using enum_to_type = decltype(TokenTypeToASTType<EnumVal>{})::type;


template<std::ranges::input_range T>
requires std::same_as<std::ranges::range_value_t<T>, Token>
std::variant<AST, std::string> parse(T tokens) {
   std::vector<std::vector<std::unique_ptr<Node>>> stack {};
   std::vector<Token> leftTokens {};
   std::vector<std::unique_ptr<Node>>* cur {&stack.emplace_back()};

   std::optional<Token> prev = std::nullopt;
   std::uint_fast8_t counter = 0;


   auto dump = [](std::vector<std::unique_ptr<Node>>* cur, Token t, std::uint16_t counter) {
         auto kind = t.kind();
         switch (kind) {
#define CASE(kind) case kind: cur->push_back(std::make_unique<enum_to_type<kind>>(t, counter)); break;
            CASE(TokenType::Inc)
            CASE(TokenType::Dec)
            CASE(TokenType::Add)
            CASE(TokenType::Sub)
#undef CASE
            default:
                throw std::logic_error("unreachable");
        }
   };

    for(Token t : tokens) {
        if(prev && prev->kind() == t.kind()) {
            ++counter;
            if(counter == 255) {
                dump(cur, *prev, counter);
                counter = 0;
                prev = std::nullopt;
            }

            continue;
        }

        if(prev) {
            dump(cur, *prev, counter);
            counter = 0;
            prev = std::nullopt;
        }

       switch (t.kind()) {
           case TokenType::Inc:
           case TokenType::Dec:
           case TokenType::Add:
           case TokenType::Sub:
               prev = {t};
               counter = 1;
               break;

#define CASE(kind) case kind: cur->push_back(std::make_unique<enum_to_type<kind>>(t)); break;
           CASE(TokenType::In)
           CASE(TokenType::Out)
#undef CASE

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
   }

   if(prev) {
       dump(cur, *prev, counter);
       counter = 0;
       prev = std::nullopt;
   }

   if(!leftTokens.empty()) {
       Token t = leftTokens.back();
       auto msg = format_string("Error: Unmatched token '%s' at  line '%d', column '%d'", t, t.row(), t.col());
       return std::variant<AST, std::string>{std::move(msg)};
   }

   assert(stack.size() == 1);
   return std::variant<AST, std::string> {AST {std::move(stack.at(0))}};
}

template<typename T>
std::variant<AST, std::string> lexAndParse(InputRange<T>& range) {
    std::ranges::input_range auto lexed = lex(range);
    return parse(lexed);
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cerr << "Args: Input file";
        return 1;
    }

    std::ifstream in{argv[1]};
    InputRange range {std::move(in)};
    auto parsed = lexAndParse(range);
    if(std::holds_alternative<std::string>(parsed)) {
        std::cerr << std::get<std::string>(parsed);
    } else {
        auto& ast {std::get<AST>(parsed)};
        ASTPrinter printer{ast, std::cout};
        printer.print();
        ASTExecutor exec {ast, std::cin, std::cout};
        exec.run();
    }
}


