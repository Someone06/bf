#include<cinttypes>

#include <llvm/IR/AssemblyAnnotationWriter.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

#include "LLVM.h"
#include "AstVisitors.h"

namespace {

    class LLVM final : private ASTWalker {
    public:
        LLVM(AST &ast, std::unordered_map<const While *, const Node *> next,
             std::ostream &out)
            : ASTWalker{ast}, nextInstruction{std::move(next)},
              ctxt{llvm::LLVMContext()}, mod{llvm::Module{"main", ctxt}},
              bd{llvm::IRBuilder(ctxt, llvm::ConstantFolder())}, o{out} {}

        llvm::Module &generate_ir();

    private:
        llvm::Function &createMainFunction();
        llvm::BasicBlock &createInitialBasicBlock(llvm::Function &mainFun);
        std::pair<llvm::BasicBlock *, bool>
        createNextBlock(const While &aWhile);

        llvm::Value &createMem();
        llvm::Value &createMemPtr();

        llvm::Value &createGEP();
        llvm::Value &read();
        void write(llvm::Value &val);
        void inc(uint64_t amount);
        void dec(uint64_t amount);


        void visit(const Left &left) override;
        void visit(const Right &right) override;
        void visit(const Inc &inc) override;
        void visit(const Dec &dec) override;
        void visit(const In &in) override;
        void visit(const Out &out) override;
        void visit(const While &aWhile) override;

        uint64_t memSz{30'000};

        llvm::LLVMContext ctxt;
        llvm::Module mod;
        llvm::IRBuilder<> bd;

        llvm::Value *mem{nullptr};
        llvm::Value *ptr{nullptr};

        llvm::Function *mainFn{nullptr};

        std::unordered_map<const While *, const Node *> nextInstruction;
        std::unordered_map<const Node *, llvm::BasicBlock *> blockForNode{};

        std::ostream &o;
    };

    llvm::Module &LLVM::generate_ir() {
        mainFn = &createMainFunction();
        createInitialBasicBlock(*mainFn);
        mem = &createMem();
        ptr = &createMemPtr();

        ASTWalker::visit();
        bd.CreateRetVoid();

        if (llvm::verifyFunction(*mainFn, &llvm::errs()))
            throw std::runtime_error("Failed to verify main function");

        llvm::AssemblyAnnotationWriter writer{};
        llvm::raw_os_ostream output{o};
        mod.print(output, &writer);
        if (llvm::verifyModule(mod, &llvm::errs()))
            throw std::runtime_error("Failed to verify module");
        return mod;
    }

    llvm::Function &LLVM::createMainFunction() {
        return *llvm::Function::Create(
                llvm::FunctionType::get(llvm::Type::getVoidTy(ctxt), false),
                llvm::Function::ExternalLinkage, "bfMain", mod);
    }

    llvm::BasicBlock &LLVM::createInitialBasicBlock(llvm::Function &mainFun) {
        const auto block{llvm::BasicBlock::Create(ctxt, "entry", &mainFun)};
        bd.SetInsertPoint(block);
        return *block;
    }

    void LLVM::visit(const Left &left) { dec(left.get_count()); }

    void LLVM::visit(const Right &right) { inc(right.get_count()); }

    void LLVM::visit(const Inc &inc) {
        auto &val = read();
        auto nval{bd.CreateAdd(&val, bd.getInt8(inc.get_count()), "mem_add")};
        write(*nval);
    }

    void LLVM::visit(const Dec &dec) {
        auto &val = read();
        auto nval{bd.CreateSub(&val, bd.getInt8(dec.get_count()), "mem_sub")};
        write(*nval);
    }

    void LLVM::visit(const In &in) {
        auto type{llvm::FunctionType::get(bd.getInt8Ty(), false)};
        auto function{mod.getOrInsertFunction("bfIn", type)};
        auto val{bd.CreateCall(function, {}, "bfInCall")};
        write(*val);
    }

    void LLVM::visit(const Out &out) {
        auto val{&read()};
        auto type{llvm::FunctionType::get(bd.getVoidTy(), {bd.getInt8Ty()},
                                          false)};
        auto function{mod.getOrInsertFunction("bfOut", type)};
        bd.CreateCall(function, {val});
    }

    void LLVM::visit(const While &aWhile) {
        auto head{llvm::BasicBlock::Create(ctxt, "while_head", mainFn)};
        auto body{llvm::BasicBlock::Create(ctxt, "while_body")};
        auto [next, newlyCreated] = createNextBlock(aWhile);
        bd.CreateBr(head);

        bd.SetInsertPoint(head);
        auto value{&read()};
        auto cond{bd.CreateICmpNE(value, bd.getInt8(0), "whileCondition")};
        bd.CreateCondBr(cond, body, next);

        mainFn->getBasicBlockList().push_back(body);
        bd.SetInsertPoint(body);
        ASTWalker::visit(aWhile);
        bd.CreateBr(head);

        if (newlyCreated) mainFn->getBasicBlockList().push_back(next);
        bd.SetInsertPoint(next);
    }

    std::pair<llvm::BasicBlock *, bool>
    LLVM::createNextBlock(const While &aWhile) {
        auto nextNode{nextInstruction.at(&aWhile)};
        auto nextBlock{blockForNode.find(nextNode)};
        if (nextBlock != blockForNode.end()) {
            return std::make_pair(std::get<1>(*nextBlock), false);
        } else {
            auto next{llvm::BasicBlock::Create(ctxt, "block")};
            blockForNode.insert(std::make_pair(nextNode, next));
            return std::make_pair(next, true);
        }
    }

    llvm::Value &LLVM::createMem() {
        auto type{llvm::ArrayType::get(bd.getInt8Ty(), memSz)};
        auto alloc{bd.CreateAlloca(type, bd.getInt64(1), "memory")};
        bd.CreateMemSet(alloc, bd.getInt8(0), (uint64_t) memSz,
                        {llvm::Align{}});
        return *alloc;
    }

    llvm::Value &LLVM::createMemPtr() {
        auto p{bd.CreateAlloca(bd.getInt64Ty(), bd.getInt64(1), "ptr")};
        bd.CreateStore(bd.getInt64(0), p, "ptr_init");
        return *p;
    }

    llvm::Value &LLVM::createGEP() {
        auto index{bd.CreateLoad(bd.getInt64Ty(), ptr, "ptr_load")};
        auto llvmIndexes{llvm::ArrayRef<llvm::Value *>{bd.getInt64(0), index}};
        return *bd.CreateGEP(llvm::ArrayType::get(bd.getInt8Ty(), memSz), mem,
                             llvmIndexes, "mem_ptr");
    }

    llvm::Value &LLVM::read() {
        auto &gep{createGEP()};
        return *bd.CreateLoad(bd.getInt8Ty(), &gep, "mem_load");
    }
    void LLVM::write(llvm::Value &val) {
        auto &gep{createGEP()};
        bd.CreateStore(&val, &gep, "mem_store");
    }

    void LLVM::inc(uint64_t amount) {
        auto val{bd.CreateLoad(bd.getInt64Ty(), ptr, "ptr_load")};
        auto inc{bd.CreateAdd(val, bd.getInt64(amount), "ptr_inc")};
        bd.CreateStore(inc, ptr, "ptr_store");
    }

    void LLVM::dec(uint64_t amount) {
        auto val{bd.CreateLoad(bd.getInt64Ty(), ptr, "ptr_load")};
        auto inc{bd.CreateSub(val, bd.getInt64(amount), "ptr_inc")};
        bd.CreateStore(inc, ptr, "ptr_store");
    }
}

void generate_ir(AST &ast, std::unordered_map<const While *, const Node *> next,
                          std::ostream &out) {
   LLVM(ast, std::move(next), out).generate_ir();
}
