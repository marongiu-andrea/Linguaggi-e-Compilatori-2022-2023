#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"


#include "llvm/IR/PatternMatch.h" /**< Aggiunto*/

using namespace llvm;
using namespace llvm::PatternMatch;



/** 
 * @brief Scorre User istruzione e fa cose
 * @param Inst1st 
 */
static void runOnUser(Instruction &  Inst1st) {
  for (Value::user_iterator Iter = Inst1st.user_begin(); Iter != Inst1st.user_end(); ++Iter) 
    outs() << "\t" << *(dyn_cast<Instruction>(*Iter)) << "\n";
}


/** 
 * @brief Scorre Uses istruzione e fa cosey
 * @param Inst1st 
 */
static void runOnUses(Instruction &  Inst1st) {
    for (Value::use_iterator Iter = Inst1st.use_begin(); Iter != Inst1st.use_end(); ++Iter) 
      outs() << "\t" << *(dyn_cast<Instruction>(Iter->getUser())) << "\n";
}


/** 
 * @brief Scorre operandi istruzione e distingue
 * @param Inst1st 
 */
static void runOnOperand (Instruction &  Inst1st) { 
    for (llvm::Use *Iter = Inst1st.op_begin(); Iter != Inst1st.op_end(); ++Iter) {
      Value *Operand = *Iter; 
      if (Argument *Arg = dyn_cast<Argument>(Operand)) {
        outs() << "\t" << *Arg << ": arg num. " << Arg->getArgNo() 
          <<" of fun  " << Arg->getParent()->getName() << "\n"; 
      }
      if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) {
        outs() << "\t" << *C << ": int const " << C->getValue() << "\n";
      }
    }
  }


/** 
 * @brief Stampa di esempio
 * @param B 
 */
static void verboseInstPrint (BasicBlock &B) { int c = 1;
  for (BasicBlock::iterator I = B.begin() ; I != B.end() ; ++I)   { 
    outs() << '\n';
    outs() <<  c++ << ") \n";
    Instruction &Inst1st = *I;

    outs() << "Istruzione: " << Inst1st << "\n";
    outs() << "Come operando: ";  Inst1st.printAsOperand(outs(), false); outs() << "\n";
    outs() << '\n';
    
    outs() << "I miei operandi sono:\n"; 
    runOnOperand(Inst1st);
    outs() << '\n';

    outs() << "La lista dei miei users sono:\n";
    runOnUser(Inst1st) ;
    outs() << '\n'; 

    outs() << "La lista dei miei usi sono (è la stessa):\n";
    runOnUses(Inst1st) ;

    outs() << '\n';
  }
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
 * @brief Ottimizzazione di base 
 * 
 * 
 * 
 * @param B 
 * @return 
 */
static void helloOptimization(BasicBlock &B) {
  #ifdef vebose
  verboseInstPrint(B);
  #endif

  Instruction &I = *B.begin();
  switch (I.getOpcode()) {
    const APInt *k;
    case Instruction::Add: {
      if (match(I.getOperand(1), m_APInt(k)) ) {
          Instruction *NewInst = BinaryOperator::Create( Instruction::Add, I.getOperand(0), I.getOperand(0) );
          NewInst->insertAfter(&I);
          I.replaceAllUsesWith(NewInst);
      }
    }
    case Instruction::Mul: {
      if (match(I.getOperand(1), m_APInt(k)) ) {
          //Instruction *NewInst = BinaryOperator::Create( Instruction::Add, I.getOperand(0), I.getOperand(0) );
          //NewInst->insertAfter(&I);
          //I.replaceAllUsesWith(NewInst);
      }
    }
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
  outs() << '\n'; 
  outs() << "PRE ottimizzazione\n";
  stampaContaIstruzioni(B,ist);
  outs() << ist << " istruzioni\n";

  helloOptimization(B);
  
  outs() << '\n'; 
  outs() << "POST ottimizzazione\n";
  stampaContaIstruzioni(B,ist);
  outs() << ist << " istruzioni\n";

  return true;
}


/**
 * @brief Conta argomenti e stampa se variadica
 * @param F 
 */
static void runOnFunArgs(Function::iterator &F) { int c=0;
  for ( auto i = F->getParent()->arg_begin() ; i != F->getParent()->arg_end() ; ++i )  { c++ ; }      
  outs() << "con argomenti " << c << "\n";
  outs() << "è variadica (se 0 falso) : " << F->getParent()->isVarArg() << "\n"; // ritorna se variadica
}

/**
 * @brief ritorna il nome, e gli usi nel sottoblocco, poi trasforma
 * @param F 
 * @return 
 */
static bool runOnFunction(Function &F) { 
  bool Transformed = false; //int usage=0;
  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    outs() << "("<<Iter->getParent()->getName() << ") "; // nome funzione
    outs() << " è usata  : " << Iter->getParent()->getNumUses() << " volte \n";
    //if (Iter->getParent()->isUsedInBasicBlock(0) ) usage++; // conta usi nel sottoblocco
    if ( !(Iter->getParent()->arg_empty()) )  runOnFunArgs(Iter) ; // Conta argomenti
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
PreservedAnalyses TransformPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &A) {
  outs() << "Nome Modulo File " << M.getSourceFileName() << "\n\n"; 
  for (Module::iterator Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) return PreservedAnalyses::none();
  }
  outs() << '\n';
  return PreservedAnalyses::none();
} 

