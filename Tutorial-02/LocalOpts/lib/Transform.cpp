#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

#include <cmath>
#include <stdint.h>

using namespace llvm;

inline bool IsPowerOf2(uint32_t a)
{
    return (a & (a-1)) == 0;
}

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
    for(auto it = B.begin(); it != B.end(); ++it) {
        Instruction& instr = *it;
        
        if(instr.getOpcode() == Instruction::Mul) {
            auto operand1 = instr.getOperand(0);
            auto operand2 = instr.getOperand(1);
            ConstantInt* op1const = dyn_cast<ConstantInt>(operand1);
            ConstantInt* op2const = dyn_cast<ConstantInt>(operand2);
            
            bool replace = false;
            uint32_t shiftRhs = 0;
            Value* shiftLhs = 0;
            if(op1const) {
                uint32_t intValue = op1const->getSExtValue();
                if(IsPowerOf2(intValue)) {
                    replace = true;
                    shiftLhs = operand2;
                    shiftRhs = log2(intValue);
                }
            }
            if(!replace && op2const) {
                uint32_t intValue = op2const->getSExtValue();
                if(IsPowerOf2(intValue)) {
                    replace = true;
                    shiftLhs = operand1;
                    shiftRhs = log2(intValue);
                }
            }
            
            if(replace) {
                Constant* rhsValue = ConstantInt::getSigned(operand1->getType(),
                                                            shiftRhs);
                Instruction* lShInstr = BinaryOperator::Create(Instruction::Shl,
                                                               shiftLhs, rhsValue);
                
                lShInstr->insertAfter(&instr);
                instr.replaceAllUsesWith(lShInstr);
            }
        }
    }
    
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
