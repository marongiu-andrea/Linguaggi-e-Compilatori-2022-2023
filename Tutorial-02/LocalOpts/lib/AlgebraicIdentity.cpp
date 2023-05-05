#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"


#include "llvm/IR/PatternMatch.h" /**< Aggiunto*/

// /usr/lib/llvm-14/include/llvm/Support/Casting.h ha tutte le regole di casting

using namespace llvm;
using namespace llvm::PatternMatch;


#include <stack>

/**
 * @brief Cancella o impila dai la cera, toglie la cera
 * 
 * Se il puntatore è nullo svuota, altrimenti riempi lo stack
 * 
 * @param I 
 */
static void inStackDelstack (Instruction *I) {
  static std::stack<Instruction*>dl;
  if (I==nullptr) {
    while(!dl.empty()) {
      dl.top()->eraseFromParent();
      dl.pop();
    }
  } 
  else dl.push(I); 
}



/**
 * @brief soluzione identità algebrica per somma
 * @param I possibili solo iteratori con istruzioni zeri (alg id)
 */
static void algIdAdd(BasicBlock::iterator  &II , const bool opr) {
  Instruction &I = (*II);
  // core
  if (dyn_cast<ConstantInt>(II->getOperand(opr))->isZero()) {
    (&I)->replaceAllUsesWith( II->getOperand( !(opr) ) );
    inStackDelstack(&I) ;
  }
}


/**
 * @brief Simmetrico ma per lo zero
 * 
*/
static void algIdMul(BasicBlock::iterator  &II, const bool opr) {
  Instruction &I = (*II);
  // core
  if (dyn_cast<ConstantInt>(II->getOperand(opr))->isOne() ) {
    (&I)->replaceAllUsesWith( II->getOperand( !(opr) ) );
    inStackDelstack(&I) ;
  }
}



/**
 * @brief Discerne i tipi di operazioni binarie con costanti tra tutte
 * 
 * @param B 
 * @return 
 * 
 */
static void filtcondBase(BasicBlock &B) {
  
  for (BasicBlock::iterator I = B.begin() ; I != B.end() ; ++I)  {
    const APInt *k;
    // espressione  binaria
    // un operatore costante, ma non entrambi
    if (I->isBinaryOp()  &&  
        (match(I->getOperand(0), m_APInt(k)) || match(I->getOperand(1), m_APInt(k))) &&   
        !(match(I->getOperand(0), m_APInt(k)) && match(I->getOperand(1), m_APInt(k)))
        )  {
      assert(I->getNumOperands() == 2 && "isn't I a bynary expresson?");

      // quale dei due
      bool opr;
      if (dyn_cast<ConstantInt>(I->getOperand(0)) ) opr=0;
      else opr=1;

      switch (I->getOpcode()) {
        case Instruction::Add: {
          outs() << "here a sum perhaps to be canceled\n";
          algIdAdd(I,opr);
          break;
        }
        case Instruction::Sub: {
          outs() << "here a sub perhaps to be canceled\n";
          algIdAdd(I,opr); //la somma è uguale 
          break;
        }
        case Instruction::Mul: {
          outs() << "here a mul perhaps to be canceled\n";
          algIdMul(I,opr);
          break;
        }
        case Instruction::SDiv: {
          outs() << "here a div perhaps to be canceled\n";
          algIdMul(I,opr); //la moltiplicazione intera è uguale 
          break;
        }
        case Instruction::Shl: {
          outs() << "here a shift perhaps to be canceled\n";
          algIdAdd(I,opr); 
          break;
        }
        default: {
          outs()<<""; //pass
        }
      }
    }
  }
  inStackDelstack(nullptr);
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
PreservedAnalyses AlgebraicIdentity::run([[maybe_unused]] Module &M, ModuleAnalysisManager &A) {
  outs() << "Nome File " << M.getSourceFileName() << "\n\n"; 
  for (Module::iterator Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) return PreservedAnalyses::none();
  }
  outs() << '\n';
  return PreservedAnalyses::none();
} 

