#include <iostream>
#include <vector>

#include "Lexer.h"
#include "Sourcerator.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cerr << "Args: Input file";
        return 1;
    }

    auto file = std::unique_ptr<std::istream>(new std::ifstream{argv[1]});
    Lexer lexer{std::move(file)};
    std::function<std::optional<Token>()> f {[l = std::make_shared<Lexer>(std::move(lexer))]() mutable {return l->next();}};
    auto iter = Sourcerator<Token>{std::move(f)};
    return 0;
}
