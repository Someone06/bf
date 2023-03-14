#include "AstVisitors.h"
#include <algorithm>

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
