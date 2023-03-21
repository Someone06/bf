#include "AST.h"
#include "AstVisitors.h"
#include "LexAndParse.h"
#include "LLVM.h"

#include <fstream>
#include <iostream>
#include <variant>

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

        NextNodeResolver resolver{ast};
        auto resolved {resolver.resolve()};

        std::ofstream out {"/tmp/bf/build/out.bc"};
        LLVM codeGen {LLVM{ast, resolved, out}};
        codeGen.generate_ir();
    }
}


