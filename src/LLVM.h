#ifndef BF_LLVM_H
#define BF_LLVM_H

#include <ostream>
#include <unordered_map>

#include "AST.h"

void generate_ir(AST& ast, std::unordered_map<const While*, const Node*> next, std::ostream &out);

#endif
