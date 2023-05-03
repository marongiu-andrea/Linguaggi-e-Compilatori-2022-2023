#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"


#include "llvm/IR/PatternMatch.h" /**< Aggiunto*/

// /usr/lib/llvm-14/include/llvm/Support/Casting.h ha tutte le regole di casting

using namespace llvm;
using namespace llvm::PatternMatch;


#include <stack>

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
 * @brief trova se log b2 intero ritorna valore, altrimenti -1 (ha valenza anche dell'infinito)
 * @param ins 
 * @return 
 */
int lb2_int(int64_t ins) {
  // APInt MRb =(  1<<( ( sizeof(APInt) *8 ) -1 ) ); // MSb 1
  const int64_t LRb = 1  ;// LRb 1
  int log = -1; int uni = 0;
  if (ins == 0 ) return -1; 
  while(ins) { log++;
      if(ins & LRb) uni++; // conta uni del bitvector
      ins = ins >> 1;
  }
  if (uni > 1 ) return -1; // non è log base 2 
  return log;
}


/**
 * @brief se log base 2 pari, allor shifta
 * 
*/
static void StrengthL(BasicBlock::iterator  &II, const bool opr) {
  Instruction &I = (*II);
  // core 
  int lb2  = lb2_int( (dyn_cast<ConstantInt>(II->getOperand(opr)))->getSExtValue()  );

  Type *Ty = I.getType();

  if ( lb2 > 0  ) {
    Instruction *NewInst = BinaryOperator::CreateShl(II->getOperand( !(opr) ), ConstantInt::get(Ty, lb2));
    (&I)->replaceAllUsesWith( II->getOperand( opr ) );
    NewInst->insertAfter(&I) ;
    inStackDelstack(&I) ;
  }
}

/**
 * @brief se log base 2 pari, allora shiftaa
 * 
*/
static void StrengthR(BasicBlock::iterator  &II, const bool opr) {
  Instruction &I = (*II);
  // core 
  int lb2  = lb2_int( (dyn_cast<ConstantInt>(II->getOperand(opr)))->getSExtValue()  );
  
  Type *Ty = I.getType();
  if ( lb2 > 0  ) {
    Instruction *NewInst = BinaryOperator::Create( Instruction::Shl, II->getOperand( !(opr) ), ConstantInt::get(Ty, -lb2));
    (&I)->replaceAllUsesWith( II->getOperand( opr ) );
    NewInst->insertAfter(&I) ;
    inStackDelstack(&I) ;
  }
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

      // quale dei due
      bool opr;
      if (dyn_cast<ConstantInt>(I->getOperand(0)) ) opr=0;
      else opr=1;

      switch (I->getOpcode()) {
        case Instruction::Mul: {
          outs() << "here a mul perhaps to be reduced\n";
          StrengthL(I,opr);
          break;
        }
        case Instruction::SDiv: {
          outs() << "here a div perhaps to be reduced\n";
          StrengthR(I,opr); //la divisione intera è uguale ma contraria 
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
PreservedAnalyses Sred::run([[maybe_unused]] Module &M, ModuleAnalysisManager &A) {
  outs() << "Nome File " << M.getSourceFileName() << "\n\n"; 
  for (Module::iterator Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) return PreservedAnalyses::none();
  }
  outs() << '\n';
  return PreservedAnalyses::none();
} 

