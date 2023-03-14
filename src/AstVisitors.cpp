#include "AstVisitors.h"
#include <algorithm>
// ------------------------- ASTWalker ---------------------------------------
void ASTWalker::visit(const Left &left) {}
void ASTWalker::visit(const Right &right) {}
void ASTWalker::visit(const Inc &inc) {}
void ASTWalker::visit(const Dec &dec) {}
void ASTWalker::visit(const In &in) {}
void ASTWalker::visit(const Out &out) {}

void ASTWalker::visit(const While &aWhile) {
    visit_body(aWhile.body());
}

void ASTWalker::visit_body(const std::vector<std::unique_ptr<Node>> &nodes) {
    auto visit = [this](auto node){node->accept(*this);};
    auto deref = [](const auto& ptr){return ptr.get();};
    std::ranges::for_each(nodes | std::views::transform(deref), visit);
}

void ASTWalker::visit_all() {
    visit_body(a.nodes());
}

const AST &ASTWalker::ast() const noexcept {
    return a;
}

// ------------------------- ASTPrinter ---------------------------------------
void ASTPrinter::print() { visit_all(); }

void ASTPrinter::visitPrimitive(const Node &node) { printToken(node.token());
}

void ASTPrinter::visitRepeating(const Repeating &repeating) {
    for(auto count = repeating.get_count(); count > 0; --count) {
        visitPrimitive(repeating);
    }
}

#define VISIT_REPEATING(type)     \
    void ASTPrinter:: visit(const type &node) { \
        visitRepeating(node);     \
    }
VISIT_REPEATING(Left)
VISIT_REPEATING(Right)
VISIT_REPEATING(Inc)
VISIT_REPEATING(Dec)
#undef VISIT_REPEATING


#define VISIT_PRIMITIVE(type)      \
    void ASTPrinter:: visit(const type &node) { \
        visitPrimitive(node);     \
    }
VISIT_PRIMITIVE(In)
VISIT_PRIMITIVE(Out)
#undef VISIT_PRIMITIVE

void ASTPrinter:: visit(const While &node) {
    printToken(node.token());
    indent();
    ASTWalker::visit(node);
    deIndent();
    printToken(node.closing());
}

void ASTPrinter::indent() {
    if(std::numeric_limits<decltype(indentation)>::max() - inc >= indentation)
        indentation += inc;
}

void ASTPrinter:: deIndent() {
    if(indentation >= inc)
        indentation -= inc;
}

void ASTPrinter:: printIndent() {
    o << std::string(indentation, ' ');
}

void ASTPrinter:: printNewline() {
    o << '\n';
}

void ASTPrinter::printToken(Token t) {
    printIndent();
    o << to_symbol(t.kind());
    printNewline();
}