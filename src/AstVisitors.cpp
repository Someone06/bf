#include <algorithm>

#include "AstVisitors.h"
#include "format_string.h"

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

void ASTWalker::visit() {
    visit_body(a.nodes());
}

const AST &ASTWalker::ast() const noexcept {
    return a;
}

// ------------------------- ASTPrinter ---------------------------------------
void ASTPrinter::print() { ASTWalker::visit(); }

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
// ------------------------- ASTExecutor ---------------------------------------
void error(const Node &node) {
    Token t = node.token();
    auto msg = format_string(
            "Error at: '%s', row &d, column '%d': Memory out of range",
            to_symbol(t.kind()), t.row(), t.col());
    throw OutOfRangeMemoryAccess(msg, t);
}

void ASTExecutor::run() {
    reset();
    dirty = true;
    ASTWalker::visit();
}

void ASTExecutor::visit(const Left &node) {
  char count = node.get_count();
  if(ptr >= count)
       ptr -= count;
  else
       error(node);

}
void ASTExecutor::visit(const Right &node) {
    char count = node.get_count();
    if(size - count >= ptr)
       ptr += count;
    else
       error(node);
}
void ASTExecutor::visit(const Inc &node) {
    char count = node.get_count();
    mem[ptr] += count;
}
void ASTExecutor::visit(const Dec &node) {
    char count = node.get_count();
    mem[ptr] -= count;
}
void ASTExecutor::visit(const In &node) {
    char val = i.get();
    mem[ptr] = val;
}
void ASTExecutor::visit(const Out &node) {
   o.put(mem[ptr]);
}

void ASTExecutor::visit(const While &node) {
  while(mem[ptr]) {
       ASTWalker::visit(node);
  }
}

void ASTExecutor::reset() {
    if(dirty) {
        std::ranges::fill(mem, 0);
    }
}