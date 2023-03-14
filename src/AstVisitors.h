#ifndef BF_ASTVISITORS_H
#define BF_ASTVISITORS_H

#include <functional>
#include <limits>

#include "AST.h"

class ASTWalker : protected Visitor {
public:
    explicit ASTWalker(AST& ast) : Visitor{}, a{ast}{}
    void visit_all();

protected:
    void visit(const Left &left) override;
    void visit(const Right &right) override;
    void visit(const Inc &inc) override;
    void visit(const Dec &dec) override;
    void visit(const In &in) override;
    void visit(const Out &out) override;
    void visit(const While &aWhile) override;

    [[nodiscard]] const AST& ast() const noexcept;

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

#endif