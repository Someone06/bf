#include <llvm/IR/AssemblyAnnotationWriter.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_os_ostream.h>

#include "LLVM.h"

llvm::Module& LLVM::generate_ir(const std::string& program_name) {
    mainFn = &createMainFunction();
    bb = &createInitialBasicBlock(*mainFn);
    mem = &createMem();
    ptr = &createMemPtr();

    ASTWalker::visit();

    if(!llvm::verifyFunction(*mainFn))
        throw std::runtime_error("Failed to verify main function");

    llvm::AssemblyAnnotationWriter writer {};
    llvm::raw_os_ostream output{o};
    mod.print(output, &writer);
    llvm::verifyModule(mod);
    return mod;
}

llvm::Function& LLVM::createMainFunction() {
    return *llvm::Function::Create(
                llvm::FunctionType::get(llvm::Type::getVoidTy(ctxt), false),
                llvm::Function::ExternalLinkage,
                "bfMain",
                mod
            );
}
llvm::BasicBlock& LLVM::createInitialBasicBlock(llvm::Function& mainFun){
    const auto block {llvm::BasicBlock::Create(ctxt, "entry", &mainFun)};
    bd.SetInsertPoint(block);
    return *block;
}

void LLVM::visit(const Left &left) {
   dec(left.get_count());
}

void LLVM::visit(const Right &right) {
  inc(right.get_count());
}

void LLVM::visit(const Inc &inc) {
 auto& val = read();
 auto nval {bd.CreateAdd(&val, bd.getInt8(inc.get_count()), "mem_add")};
 write(*nval);
}

void LLVM::visit(const Dec &dec) {
    auto& val = read();
    auto nval {bd.CreateSub(&val, bd.getInt8(dec.get_count()), "mem_sub")};
    write(*nval);
}

void LLVM::visit(const In &in) {
    throw std::logic_error("unimplemented");
}

void LLVM::visit(const Out &out) {
    throw std::logic_error("unimplemented");
}

void LLVM::visit(const While &aWhile) {
    auto head {llvm::BasicBlock::Create(ctxt)};
    auto body {llvm::BasicBlock::Create(ctxt)};

    // TODO: Adjust
    auto next {llvm::BasicBlock::Create(ctxt)};

    // Jump from current basic block to header.
    bd.CreateBr(head);

    // Generate header.
    bd.SetInsertPoint(head);
    auto value {&read()};
    auto cond {bd.CreateICmpNE(value, bd.getInt8(0), "whileCondition")};
    bd.CreateCondBr(cond, body, next);

    // Generate body.
    bd.SetInsertPoint(body);
    aWhile.accept(*this);
    bd.CreateBr(head);

    // Setup for next block.
    bb = next;
    bd.SetInsertPoint(next);
}

llvm::Value& LLVM::createMem() {
    auto type {llvm::ArrayType::get(bd.getInt8Ty(), memSz)};
    auto alloc {bd.CreateAlloca(type, bd.getInt64(1), "memory")};
    bd.CreateMemSet(alloc, bd.getInt8(0), (uint64_t) memSz, {llvm::Align{}});
    return *alloc;
}

llvm::Value &LLVM::createMemPtr() {
    auto p {bd.CreateAlloca(bd.getInt64Ty(), bd.getInt64(1), "ptr")};
    bd.CreateStore(bd.getInt64(0), p, "ptr_init");
    return *p;
}

llvm::Value& LLVM::createGEP(){
    auto index {bd.CreateLoad(bd.getInt64Ty(), ptr, "ptr_load")};
    auto llvmIndexes {llvm::ArrayRef<llvm::Value *>{bd.getInt64(0), index}};
    return *bd.CreateGEP(llvm::ArrayType::get(bd.getInt8Ty(), memSz), mem, llvmIndexes, "mem_ptr");
}

llvm::Value &LLVM::read() {
    auto& gep {createGEP()};
    return *bd.CreateLoad(bd.getInt8Ty(), &gep, "mem_load");
}
void LLVM::write(llvm::Value& val) {
    auto& gep {createGEP()};
    bd.CreateStore(&val, gep, "mem_store");
}

void LLVM::inc(uint64_t amount) {
   auto val {bd.CreateLoad(bd.getInt64Ty(), ptr, "ptr_load")};
   auto inc {bd.CreateAdd(val, bd.getInt64(amount), "ptr_inc")};
   bd.CreateStore(inc, ptr, "ptr_store");
}

void LLVM::dec(uint64_t amount) {
   auto val {bd.CreateLoad(bd.getInt64Ty(), ptr, "ptr_load")};
   auto inc {bd.CreateSub(val, bd.getInt64(amount), "ptr_inc")};
   bd.CreateStore(inc, ptr, "ptr_store");
}

