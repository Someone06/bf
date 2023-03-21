#ifndef BF_ASTVISITORS_H
#define BF_ASTVISITORS_H

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Token.h"
#include "AST.h"
#include "debug.h"
#include "NullOstream.h"

class ASTWalker : protected Visitor {
public:
    explicit ASTWalker(AST& ast) : Visitor{}, a{ast}{}
    void visit();

protected:
    void visit(const Left &left) override;
    void visit(const Right &right) override;
    void visit(const Inc &inc) override;
    void visit(const Dec &dec) override;
    void visit(const In &in) override;
    void visit(const Out &out) override;
    void visit(const While &aWhile) override;

    [[maybe_unused]] [[nodiscard]] const AST& ast() const noexcept;

private:
    void visit_body(const std::vector<std::unique_ptr<Node>>& nodes);

    AST& a;
};

class ASTPrinter : private ASTWalker {
 public:
    ASTPrinter(AST& ast, std::ostream& out, std::uint64_t increment = 4)
         : ASTWalker{ast}, o{out}, inc{increment} {}
    void print();

private:
    void visit(const Left &node) override;
    void visit(const Right &node) override;
    void visit(const Inc &node) override;
    void visit(const Dec &node) override;
    void visit(const In &node) override;
    void visit(const Out &node) override;
    void visit(const While &node) override;

    void visitPrimitive(const Node& node);
    void visitRepeating(const Repeating& repeating);

    void indent();
    void deIndent();

    void printToken(Token t);
    void printNewline();
    void printIndent();

    std::uint64_t inc;
    std::uint64_t indentation {0};
    std::ostream& o;
};

class OutOfRangeMemoryAccess final : public std::runtime_error {
public:
   OutOfRangeMemoryAccess(const std::string& msg, Token token)
        : std::runtime_error{msg}, t{token} {}

     [[maybe_unused]] [[nodiscard]] Token token() {
        return t;
    }
private:
    Token t;
};

class ASTExecutor : private ASTWalker {
public:
    ASTExecutor(AST& ast, std::istream& in, std::ostream& out, std::ostream& err = Debug::if_debug<std::ostream&>(std::cerr, cnull), size_t memorySize = 30'000)
            : ASTWalker{ast}, i{in}, o{out}, e{err}, size{memorySize}, mem(size) {}

    void run();

private:
    void visit(const Left &node) override;
    void visit(const Right &node) override;
    void visit(const Inc &node) override;
    void visit(const Dec &node) override;
    void visit(const In &node) override;
    void visit(const Out &node) override;
    void visit(const While &node) override;

    void reset();

    std::istream& i;
    std::ostream& o;
    std::ostream& e;

    const size_t size;
    std::vector<char> mem;

    bool dirty {false};
    size_t ptr {0};
};

class NextNodeResolver final : private ASTWalker {
public:
    using ASTWalker::ASTWalker;

    std::unordered_map<const While*, const Node*> resolve();

private:
    void link(const Node* node);

    void visit(const Left &left) override;
    void visit(const Right &right) override;
    void visit(const Inc &inc) override;
    void visit(const Dec &dec) override;
    void visit(const In &in) override;
    void visit(const Out &out) override;
    void visit(const While &aWhile) override;

    std::unordered_map<const While*, const Node*> map {};
    std::vector<const While*> prev {};
};

#endif