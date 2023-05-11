#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runOnBasicBlock(BasicBlock &B) {
    
    // Preleviamo le prime due istruzioni del BB
    Instruction &Inst1st = *B.begin(), &Inst2nd = *(++B.begin());

    // L'indirizzo della prima istruzione deve essere uguale a quello del 
    // primo operando della seconda istruzione (per costruzione dell'esempio)
    assert(&Inst1st == Inst2nd.getOperand(0));

    // Stampa laMultiInstructionPass prima istruzione
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


  bool runOnFunction(Function &F) {
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
      if (runOnBasicBlock(*Iter)) {
        Transformed = true;
      }
    }

    return Transformed;
  }




<<<<<<< HEAD
PreservedAnalyses TransformPass::run([[maybe_unused]] Function &F, FunctionAnalysisManager &AM) {

  auto &LI = AM.getResult<LoopAnalysis>(F);
  int l = 0;
  int BID = 0;

  SmallVector<Loop *, 4> PreOrderLoops = LI.getLoopsInPreOrder();
  Loop *LP = nullptr;
  for(Loop *L: PreOrderLoops){
    outs() << "\n********* PROCESSING LOOP " << l++ << "\n";
    if(LP == nullptr)
    {
      LP = L;
      continue;
    }
    if (LP.back() != L.front()) continue;
    else {
      outs() << LP << " è un loop adiacente a " << L << "\n";
      /*const SCEV *TripCountSCEV = SE.getBackedgeTakenCount(L);
      const APInt TripCount = dyn_cast<SCEVConstant>(TripCountSCEV)->getAPInt();
      errs() << "Loop has " << TripCount.toString(10) << " iterations.\n";*/
=======
PreservedAnalyses TransformPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) {
      return PreservedAnalyses::none();
>>>>>>> 18d89016306e42a6743f35bdac70056fa07fd6b2
    }
  }
  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {

  }

  return PreservedAnalyses::none();
}

