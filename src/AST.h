#ifndef BF_AST_H
#define BF_AST_H

#include "Token.h"
#include <memory>
#include <ranges>
#include <vector>

class Left;
class Right;
class Inc;
class Dec;
class In;
class Out;
class While;

struct Visitor {
    Visitor(const Visitor&) = delete;
    Visitor& operator=(const Visitor&) = delete;
    Visitor(Visitor&&) = delete;
    Visitor& operator=(Visitor&&) = delete;
    virtual ~Visitor() = default;

    virtual void visit(const Left&) = 0;
    virtual void visit(const Right&) = 0;
    virtual void visit(const Inc&) = 0;
    virtual void visit(const Dec&) = 0;
    virtual void visit(const In&) = 0;
    virtual void visit(const Out&) = 0;
    virtual void visit(const While&) = 0;
};

class Node {
public:
    explicit Node(Token token): t{token} {}

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node&&) = delete;
    Node& operator=(Node&&) = delete;
    virtual ~Node() = default;

    [[nodiscard]] const Token& token() const noexcept {
        return t;
    }

    virtual void accept(Visitor& v) const noexcept = 0;

private:
    Token t;
};

class Repeating : public Node {
public:
    Repeating(Token token, std::uint8_t count) : Node{token}, c{count} {}

    [[nodiscard]] std::uint8_t get_count() const noexcept {
        return c;
    }

private:
    std::uint8_t c;

};

class Left final : public Repeating {
public:
    using Repeating::Repeating;
    void accept(Visitor& v) const noexcept override {
       v.visit(*this);
    }
};

class Right final : public Repeating {
public:
    using Repeating::Repeating;
    void accept(Visitor& v) const noexcept override {
       v.visit(*this);
    }
};

class Inc final : public Repeating {
public:
    using Repeating::Repeating;
    void accept(Visitor& v) const noexcept override {
       v.visit(*this);
    }
};

class Dec final : public Repeating {
public:
    using Repeating::Repeating;
    void accept(Visitor& v) const noexcept override {
       v.visit(*this);
    }
};

class In final : public Node {
public:
    using Node::Node;
    void accept(Visitor& v) const noexcept override {
       v.visit(*this);
    }
};

class Out final : public Node {
public:
    using Node::Node;
    void accept(Visitor& v) const noexcept override {
       v.visit(*this);
    }
};

class While final : public Node {
public:
    While(Token opening, Token closing, std::vector<std::unique_ptr<Node>> body): Node{opening}, c{closing}, b{std::move(body)} {}

    void accept(Visitor& v) const noexcept override {
       v.visit(*this);
    }

    [[nodiscard]] const std::vector<std::unique_ptr<Node>>& body() {
        return b;
    }

    [[nodiscard]] Token closing() const noexcept {
        return c;
    }

private:
    const Token c;
    const std::vector<std::unique_ptr<Node>> b;
};

class AST final {
public:
    explicit AST(std::vector<std::unique_ptr<Node>> nodes): n{std::move(nodes)}{}

    AST(const AST&) = delete;
    AST& operator=(const AST&) = delete;
    AST(AST&&) = default;
    AST& operator=(AST&&) = default;
    ~AST() = default;

    const std::vector<std::unique_ptr<Node>>& nodes() {
        return n;
    }

private:
    std::vector<std::unique_ptr<Node>> n;
};

#endif
