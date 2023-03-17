#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <math.h>
#include <list>
#include <string.h>

using namespace llvm;

bool isPowOfTwo(int n);


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

bool myRun(BasicBlock& bb)
{
  std::list<Instruction*> toBeRemoved;

  for(auto& inst : bb)
  {
        if(!strcmp(inst.getOpcodeName(),"mul"))     // cerchiamo le mul
        {
          outs() << inst << "\n";

          ConstantInt* opLeft = dyn_cast<ConstantInt>(inst.getOperand(0));
          ConstantInt* opRight = dyn_cast<ConstantInt>(inst.getOperand(1));

          Instruction* newInst = nullptr;

          if(opLeft && !opRight)
          {
            uint64_t val = opLeft->getZExtValue();

            outs() << "OpLeft: " << val << "\n";
            if(val > 0 and isPowOfTwo(int(val)))
            {
              outs() << val << " è potenza del 2\n";

              Value* newOp = ConstantInt::get(IntegerType::get(bb.getContext(), 32), log2(val), false);

              outs() << "newOp: " << dyn_cast<ConstantInt>(newOp)->getZExtValue() << "\n";

              newInst = BinaryOperator::Create(Instruction::Shl, inst.getOperand(1), newOp);
            }
          }
          else if(opRight && !opLeft)
          {
            uint64_t val = opRight->getZExtValue();

            outs() << "OpRight: " << val << "\n";

            if(val > 0 and isPowOfTwo(int(val)))
            {
              outs() << val << " è potenza del 2\n";
          
              Value* newOp = ConstantInt::get(IntegerType::get(bb.getContext(), 32), log2(val), false);

              outs() << "newOp: " << dyn_cast<ConstantInt>(newOp)->getZExtValue() << "\n";
              
              newInst = BinaryOperator::Create(Instruction::Shl, inst.getOperand(0), newOp);
            }
          }

          if(newInst)
          {
            newInst->insertBefore(&inst);
            inst.replaceAllUsesWith(newInst);
            toBeRemoved.push_back(&inst);
          }
        }
    }
    for(auto i : toBeRemoved)
    {
      i->eraseFromParent();
    }
    return true;
}
bool isPowOfTwo(int n)
{
  if(n <= 1)
    return false;
  return floor(log2(n)) == ceil(log2(n));
}

bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (myRun(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}


PreservedAnalyses TransformPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

