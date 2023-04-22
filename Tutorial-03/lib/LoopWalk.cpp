#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>

using namespace llvm;

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll(); //indico di preservare i risutati di questo passo di analisi
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>(); 
    //queste due direttive mi specificano di dover eseguire prima i passi di Dominance e LoopInfo prima di procedere ad eseguire questo passo, perche' i risultati
    //di tali passi verranno utilizzati in questo passo di analisi
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOPPASS INIZIATO...\n"; 
    //verifico se il loop e' nella forma normale/semplificata
    if (L->isLoopSimplifyForm()) {
      outs() << "Il Loop e' nella forma normale!\n\n";
    }

    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    //prendo il blocco di preheader e lo stampo
    BasicBlock *preheader = L->getLoopPreheader();
    if (preheader){
      outs() << "Il loop contiene un blocco di preheader!\n";
      preheader->print(outs());
      outs() << "\n";
    }
    //scorro tutti i BB che compongono il loop, scorro tutte le istruzioni di ogni BB e cerco una sub e stampo l'istruzione che contiene la definizione dei suoi operandi (solo se non sono delle costanti);
    //inoltre, stampo anche il BB che contiene l'istruzione di SUB
    BasicBlock *actualBB;
    //outs() << "Procedo alla stampa dei BB che compongono il loop:\n\n";
    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
      actualBB = *BI;
      for (auto &instr : *actualBB) {
        if( instr.isBinaryOp() && instr.getOpcode() == Instruction::Sub) { //allora ho appena indivudato una istruzione di sottrazione
          //TODO: prendi i due operandi dell'operazione e verifica che non siano castabili a delle costanti
          checkOperand(instr.getOperand(0));
          checkOperand(instr.getOperand(1));
        }
      }
    }
    return false;
  }

  void checkOperand(Value* operand) {
    if (!dyn_cast<ConstantInt>(operand)) { //se l'operando che ho appena ricevuto non e' una costante, allora lo provo a castare ad istruzione e stamparne il BB di appartenenza
      Instruction *instr = dyn_cast<Instruction>(operand);
      if (instr) { //il cast ad istruzione e' andato a buon fine
        BasicBlock *B = instr->getParent();
        B->printAsOperand(outs(),false);
        outs() << "\n";
      }
    }
  }

};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

