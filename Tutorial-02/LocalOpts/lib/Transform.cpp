#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runOnBasicBlock(BasicBlock &B) 
{
  auto itInstr = B.begin();

  // Preleviamo le prime due istruzioni del BB
  Instruction& Inst1st = *itInstr; 
  itInstr++;
  Instruction& Inst2nd = *itInstr;

  // L'indirizzo della prima istruzione deve essere uguale a quello del 
  // primo operando della seconda istruzione (per costruzione dell'esempio)
  assert(&Inst1st == Inst2nd.getOperand(0));

  // Stampa la prima istruzione
  outs() << "Instruction: " << Inst1st << "\n";

  // Stampa la prima istruzione come operando
  outs() << "\tOperand: <";
  Inst1st.printAsOperand(outs(), false);
  outs() << ">\n";

  // User-->Use-->Value
  for (auto* it = Inst1st.op_begin(); it != Inst1st.op_end(); ++it) 
  {
    Value* op = *it;

    if (Argument* arg = dyn_cast<Argument>(op)) 
    {
      outs() << "\t\t<" << *arg << "> arg " << arg->getArgNo() 
        <<" of <" << arg->getParent()->getName() << ">\n";
    }
    if (ConstantInt* constant = dyn_cast<ConstantInt>(op)) 
    {
      outs() << "\t\t<" << *constant << "> constant integer <" << constant->getValue() << ">\n";
    }
  }

  outs() << "\nUser list:\n";
  for (auto it = Inst1st.user_begin(); it != Inst1st.user_end(); ++it) 
  {
    outs() << "\t" << *(dyn_cast<Instruction>(*it)) << "\n";
  }

  outs() << "\nUse list:\n";
  for (auto it = Inst1st.use_begin(); it != Inst1st.use_end(); ++it) 
  {
    outs() << "\t" << *(dyn_cast<Instruction>(it->getUser())) << "\n";
  }

  // Manipolazione delle istruzioni
  Instruction *NewInst = BinaryOperator::Create(
      Instruction::Add, Inst1st.getOperand(0), Inst1st.getOperand(0));

  NewInst->insertAfter(&Inst1st);
  outs() << "uses:" << Inst1st.getNumUses() <<"\n";
  // Si possono aggiornare le singole references separatamente?
  // Controlla la documentazione e prova a rispondere.
  Inst1st.replaceAllUsesWith(NewInst);
  outs() << "uses:" << Inst1st.getNumUses() <<"\n";
  return true;
}

bool runOnFunction(Function &F)
{
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) 
  {
    if (runOnBasicBlock(*Iter)) 
    {
      Transformed = true;
    }
  }
  return Transformed;
}

PreservedAnalyses TransformPass::run([[maybe_unused]] Module &M,ModuleAnalysisManager &) 
{
  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) 
  {
    if (runOnFunction(*Iter)) 
    {
      return PreservedAnalyses::none();
    }
  }
  return PreservedAnalyses::none();
}

