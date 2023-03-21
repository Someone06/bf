#ifndef BF_LLVM_H
#define BF_LLVM_H

#include <ostream>
#include <unordered_map>

#include <llvm/IR/Module.h>

#include "AST.h"

llvm::Module& generate_ir(AST& ast, std::unordered_map<const While*, const Node*> next, std::ostream &out);

#endif
