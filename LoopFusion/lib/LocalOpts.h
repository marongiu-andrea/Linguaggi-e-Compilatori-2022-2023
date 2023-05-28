#pragma once 


// serve fare la prima parte  dell'esercizi
// come controllare se loop sono adiacenti
// candizione di adiacenza se le istruzini vanno nel preheader faòlisce
// beve contenere solo una branch e la branch deve saltare dentro il loop successivo

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>



class TransformPass final
    : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &,
                              llvm::FunctionAnalysisManager &);
};

