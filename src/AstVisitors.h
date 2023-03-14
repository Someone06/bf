#ifndef BF_ASTVISITORS_H
#define BF_ASTVISITORS_H

#include<limits>

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

template<typename T>
concept Writeable = requires (T x, std::string_view s) { x << s;};

template<Writeable T>
class ASTPrinter : private ASTWalker {
 public:
    ASTPrinter(AST& ast, T& out, std::uint64_t increment = 4) : ASTWalker{ast}, o{out}, inc{increment}{}

    void print() { visit_all(); }

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

    void printString(std::string_view str);
    void printNewline();
    void printIndent();

    std::uint64_t inc;
    std::uint64_t indentation {0};
    T& o;
};

// --------------------------- Definitions -----------------------------------

#define FUN \
    template<Writeable T> \
    void ASTPrinter<T>::

FUN visitPrimitive(const Node &node) {
    printIndent();
    printString(to_symbol(node.token().kind()));
    printNewline();
}

FUN visitRepeating(const Repeating &repeating) {
    for(auto count = repeating.get_count(); count > 0; --count) {
        visitPrimitive(repeating);
    }
}

#define VISIT_REPEATING(type)     \
    FUN visit(const type &node) { \
        visitRepeating(node);     \
    }
VISIT_REPEATING(Left)
VISIT_REPEATING(Right)
VISIT_REPEATING(Inc)
VISIT_REPEATING(Dec)
#undef VISIT_REPEATING


#define VISIT_PRIMITIVE(type)      \
    FUN visit(const type &node) { \
        visitPrimitive(node);     \
    }
VISIT_PRIMITIVE(In)
VISIT_PRIMITIVE(Out)
#undef VISIT_PRIMITIVE

FUN visit(const While &node) {
    printString(to_symbol(node.token().kind()));
    indent();
    ASTWalker::visit(node);
    deIndent();
    printString(to_symbol(node.closing().kind()));
}

FUN indent() {
    if(std::numeric_limits<decltype(indentation)>::max() - inc >= indentation)
        indentation += inc;
}

FUN deIndent() {
    if(indentation >= inc)
        indentation -= inc;
}

FUN printString(const std::string_view str) {
    o << str;
}

FUN printIndent() {
    printString(std::string(indentation, ' '));
}

FUN printNewline() {
    printString("\n");
}

#undef FUN

#endif
