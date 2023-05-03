#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"


#include "llvm/IR/PatternMatch.h" /**< Aggiunto*/

// /usr/lib/llvm-14/include/llvm/Support/Casting.h ha tutte le regole di casting

using namespace llvm;
using namespace llvm::PatternMatch;


#include <stack>
#include <queue>


/**
 * @brief Cancella o impila ( dai la cera, toglie la cera )
 * 
 * Se il puntatore è nullo svuota, altrimenti riempi lo stack
 * 
 * @param I 
 */
static void inStackDelstack (Instruction *I) {
  static std::SmallVector<Instruction*>dl; // static così restano in memoria 
  if (I==nullptr) {                  // i puntatori fino allo svuotamento dello stack
    while(!dl.empty()) {
      dl.top()->eraseFromParent();
      dl.pop();
    }
  } 
  else dl.push(I); 
}



/**
 * @brief memorizza 2 variabili in una coda
 * 
*/
static void multInstOpty(BasicBlock::iterator  &II) {
  static std::queue<Instruction*>cM;
  Instruction &I = (*II);
  cM.push(&I);
  if ( cM.size()==2  ) {
    Instruction * i1, * i2;
    i1=cM.front();
    // quale dei due
    bool opr1;
    if (dyn_cast<ConstantInt>(i1->getOperand(0)) ) opr1=0;
    else opr1=1;
    i2=cM.back();
    bool opr2;
    if (dyn_cast<ConstantInt>(i2->getOperand(0)) ) opr2=0;
    else opr2=1;
    
    // se le 2 costanti e i 2 tipi di espressoine binaria sono uguali ==> semplifica
    if ( (dyn_cast<ConstantInt>(i1->getOperand(opr1)))->getSExtValue() ==
          -(dyn_cast<ConstantInt>(i2->getOperand(opr2)))->getSExtValue() ) { // negativo il suo opposto
 
 
      Type *t2 = i2->getType(); // solo il 2, in casi più comlessi, si posso confrontare tutti i tipi

      Instruction *NewInst = BinaryOperator::CreateAdd(i2->getOperand( !(opr2) ), ConstantInt::get(t2, 0));
      (i2)->replaceAllUsesWith( i2->getOperand( opr2 ) );
      NewInst->insertAfter(i2) ;
      
      inStackDelstack(i2) ;
    }
  }
  if ( cM.size()>=2  ) cM.pop();
}




/**
 * @brief Discerne i tipi di espressioni binarie con costanti tra un po' di op
 * 
 * @param B blocco
 * 
 */
static void filtcondBase(BasicBlock &B) {
  std::SmallVector<Instruction*>delList;
  for (BasicBlock::iterator I = B.begin() ; I != B.end() ; ++I)  {
    const APInt *k;
    // espressione  binaria
    // un operatore costante, ma non entrambi

    if (I->isBinaryOp()  &&  
        (match(I->getOperand(0), m_APInt(k)) || match(I->getOperand(1), m_APInt(k))) &&   
        !(match(I->getOperand(0), m_APInt(k)) && match(I->getOperand(1), m_APInt(k)))
        )  {
      assert(I->getNumOperands() == 2 && "isn't I a bynary expresson?");



      switch (I->getOpcode()) {
        case Instruction::Add: {
          outs() << "here a sum perhaps to be optimized\n";
          multInstOpty(I);
          break;
        }
        default: {
          outs()<<""; //pass
        }
      }
    }
  }
  inStackDelstack(nullptr); // qua cancella
}


/**
 * @brief stampa le istruzioni di un basic block a schermo
 * @param B 
 */
static void stampaContaIstruzioni (BasicBlock &B, int &ist) { ist=0;
  for (BasicBlock::iterator Inst1st = B.begin() ; Inst1st!=B.end() ; ++Inst1st ) { 
    outs() << *Inst1st<< "\n"; // stampa istruzioni
    ist++;
  }
}


/**
 * @brief scorre nel basic block
 * 
 * Algebraic Identity
 * Strength Reduction
 * Multi-instruction Operations
 * 
 * @param B ref al BB
 * @return 
 */
static bool runOnBasicBlock(BasicBlock &B) {
  // stampa istruzioni
  int funCall =0; int ist=0;
  #define verbose
  #ifdef verbose
  outs() << '\n'; 
  outs() << "PRE ottimizzazione\n";
  stampaContaIstruzioni(B,ist);
  outs() << "\n" << ist << " istruzioni\n\n";
  #endif

  filtcondBase(B);

  #ifdef verbose
  outs() << '\n'; 
  outs() << "POST ottimizzazione\n";
  stampaContaIstruzioni(B,ist);
  outs() << "\n" << ist << " istruzioni\n\n";
  #endif

  return true;
}


/**
 * @brief ritorna il nome, e gli usi nel sottoblocco, poi trasforma
 * @param F 
 * @return 
 */
static bool runOnFunction(Function &F) { 
  bool Transformed = false; //int usage=0;
  for (Function::iterator Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter))  Transformed = true;
  }
  return Transformed;
}




/**
 * @brief Base, scorre tra i moduli file
 * @param M Modulo
 * @param A Analisi
 * @return analisy
 */               //TransformPass
PreservedAnalyses MultiInstruction::run([[maybe_unused]] Module &M, ModuleAnalysisManager &A) {
  outs() << "Nome File " << M.getSourceFileName() << "\n\n"; 
  for (Module::iterator Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) return PreservedAnalyses::none();
  }
  outs() << '\n';
  return PreservedAnalyses::none();
} 

