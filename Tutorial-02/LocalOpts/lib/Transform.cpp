#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

#include <cmath>
#include <stdint.h>
#include <vector>

using namespace llvm;
#if 0
bool runOnBasicBlock(BasicBlock &B) {
    // Preleviamo le prime due istruzioni del BB
    Instruction &Inst1st = *B.begin(), &Inst2nd = *(++B.begin());
    
    // L'indirizzo della prima istruzione deve essere uguale a quello del 
    // primo operando della seconda istruzione (per costruzione dell'esempio)
    assert(&Inst1st == Inst2nd.getOperand(0));
    
    // Stampa la prima istruzione
    outs() << "PRIMA ISTRUZIONE: " << Inst1st << "\n";
    // Stampa la prima istruzione come operando
    outs() << "COME OPERANDO: ";
    Inst1st.printAsOperand(outs(), false);
    outs() << "\n";
    
    // User-->Use-->Value
    outs() << "I MIEI OPERANDI SONO:\n";
    for (auto *Iter = Inst1st.op_begin(); Iter != Inst1st.op_end(); ++Iter) {
        Value *Operand = *Iter;
        
        if (Argument *Arg = dyn_cast<Argument>(Operand)) {
            outs() << "\t" << *Arg << ": SONO L'ARGOMENTO N. " << Arg->getArgNo() 
                <<" DELLA FUNZIONE" << Arg->getParent()->getName()
                << "\n";
        }
        if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) {
            outs() << "\t" << *C << ": SONO UNA COSTANTE INTERA DI VALORE " << C->getValue()
                << "\n";
        }
    }
    
    outs() << "LA LISTA DEI MIEI USERS:\n";
    for (auto Iter = Inst1st.user_begin(); Iter != Inst1st.user_end(); ++Iter) {
        outs() << "\t" << *(dyn_cast<Instruction>(*Iter)) << "\n";
    }
    
    outs() << "E DEI MIEI USI (CHE E' LA STESSA):\n";
    for (auto Iter = Inst1st.use_begin(); Iter != Inst1st.use_end(); ++Iter) {
        outs() << "\t" << *(dyn_cast<Instruction>(Iter->getUser())) << "\n";
    }
    
    // Manipolazione delle istruzioni
    Instruction *NewInst = BinaryOperator::Create(
                                                  Instruction::Add, Inst1st.getOperand(0), Inst1st.getOperand(0));
    
    NewInst->insertAfter(&Inst1st);
    // Si possono aggiornare le singole references separatamente?
    // Controlla la documentazione e prova a rispondere.
    
    Inst1st.replaceAllUsesWith(NewInst);
    
    return true;
}
#else
bool runOnBasicBlock(BasicBlock &B) {
    std::vector<Instruction *> instrToDelete;
    for(auto it = B.begin(); it != B.end(); ++it) {
        Instruction& instr = *it;

        if (!instr.isBinaryOp())
            continue;;
        
        bool replace = false;
        uint64_t shiftRhs = 0;
        Value* shiftLhs = 0;

        auto parseOperand = [&](llvm::Value *left, llvm::ConstantInt *rhs) {
            if (!rhs || rhs->isNegative())
                return;

            uint64_t intValue = rhs->getZExtValue();
            if (isPowerOf2_64(intValue)) {
                replace = true;
                shiftLhs = left;
                shiftRhs = log2(intValue);
            }
        };
        
        auto operand1 = instr.getOperand(0);
        auto operand2 = instr.getOperand(1);
        ConstantInt* op1const = dyn_cast<ConstantInt>(operand1);
        ConstantInt* op2const = dyn_cast<ConstantInt>(operand2);
        Instruction* newInstr = nullptr;

        switch (instr.getOpcode())  
        {
        case Instruction::Mul:
        {
            parseOperand(operand2, op1const);
            if (!replace)
                parseOperand(operand1, op2const);

            if(replace) {
                Constant* rhsValue = ConstantInt::getSigned(operand1->getType(),
                                                            shiftRhs);
                newInstr = BinaryOperator::Create(Instruction::Shl,
                                                    shiftLhs, rhsValue);
            }

            break;
        }
        
        case Instruction::SDiv:
        {
            auto operand1 = instr.getOperand(0);
            auto operand2 = instr.getOperand(1);
            ConstantInt* op2const = dyn_cast<ConstantInt>(operand2);

            parseOperand(operand1, op2const);
            if (replace) {
                Constant *rhsValue = ConstantInt::getSigned(operand2->getType(), shiftRhs);
                
                newInstr = BinaryOperator::Create(Instruction::LShr,
                                                    shiftLhs, rhsValue);
                
            
            }
            break;
        }
        default:
            continue;;
        }

        if (newInstr) {
            newInstr->insertAfter(&instr);
            instr.replaceAllUsesWith(newInstr);
            instrToDelete.push_back(&instr);
        }
    }

    for (auto *instr: instrToDelete)
      instr->eraseFromParent();

    instrToDelete.clear();

    
    outs() << "Done!\n";
    return true;
}
#endif

bool runOnFunction(Function &F) {
    bool Transformed = false;
    
    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
        if (runOnBasicBlock(*Iter))
            Transformed = true;
    }
    
    return Transformed;
}

PreservedAnalyses TransformPass::run([[maybe_unused]] Module &M,
                                     ModuleAnalysisManager &) {
    // Un semplice passo di esempio di manipolazione della IR
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
        if (runOnFunction(*Iter))
            return PreservedAnalyses::none();
    }
    
    return PreservedAnalyses::none();
}
