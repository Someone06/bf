#ifndef BF_LLVM_H
#define BF_LLVM_H

#include <ostream>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "AST.h"
#include "AstVisitors.h"

class LLVM final : private ASTWalker {
    LLVM(AST& ast, std::ostream &out)
        : a{ast},
          ctxt{llvm::LLVMContext()},
          mod{llvm::Module{"main", ctxt}},
          bd{llvm::IRBuilder(ctxt, llvm::ConstantFolder())},
          o{out} {}

    llvm::Module& generate_ir(const std::string& program_name);

    llvm::Function& createMainFunction();
    llvm::BasicBlock& createInitialBasicBlock(llvm::Function& mainFun);
    llvm::Value &createMem();
    llvm::Value &createMemPtr();

    llvm::Value& createGEP();
    llvm::Value& read();
    void write(llvm::Value& val);
    void inc(uint64_t amount);
    void dec(uint64_t amount);


    void visit(const Left &left) override;
    void visit(const Right &right) override;
    void visit(const Inc &inc) override;
    void visit(const Dec &dec) override;
    void visit(const In &in) override;
    void visit(const Out &out) override;
    void visit(const While &aWhile) override;

private:
    AST& a;
    uint64_t memSz {30'000};

    llvm::LLVMContext ctxt;
    llvm::Module mod;
    llvm::IRBuilder<> bd;

    llvm::Value* mem {nullptr};
    llvm::Value* ptr {nullptr};

    llvm::Function * mainFn {nullptr};
    llvm::BasicBlock * bb {nullptr};

    std::ostream& o;
};


#endif
