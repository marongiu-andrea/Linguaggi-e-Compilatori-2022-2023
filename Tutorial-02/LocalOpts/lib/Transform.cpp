#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runOnBasicBlock(BasicBlock &B) 
{
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
    for (auto *Iter = Inst1st.op_begin(); Iter != Inst1st.op_end(); ++Iter) 
    {
        Value *Operand = *Iter;

        if (Argument *Arg = dyn_cast<Argument>(Operand)) 
        {
            outs() << "\t" << *Arg << ": SONO L'ARGOMENTO N. " << Arg->getArgNo() 
            <<" DELLA FUNZIONE " << Arg->getParent()->getName() << "\n";
        }
        if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) 
        {
            outs() << "\t" << *C << ": SONO UNA COSTANTE INTERA DI VALORE " << C->getValue() << "\n";
        }
    }

    outs() << "LA LISTA DEI MIEI USERS:\n";
    for (auto Iter = Inst1st.user_begin(); Iter != Inst1st.user_end(); ++Iter) 
    {
        outs() << "\t" << *(dyn_cast<Instruction>(*Iter)) << "\n";
    }

    outs() << "E DEI MIEI USI (CHE E' LA STESSA):\n";
    for (auto Iter = Inst1st.use_begin(); Iter != Inst1st.use_end(); ++Iter) 
    {
        outs() << "\t" << *(dyn_cast<Instruction>(Iter->getUser())) << "\n";
    }

    // Manipolazione delle istruzioni
    Instruction *NewInst = BinaryOperator::Create(
        Instruction::Add, Inst1st.getOperand(0), Inst1st.getOperand(0));

    NewInst->insertAfter(&Inst1st);

    // Si possono aggiornare le singole references separatamente?
    // Controlla la documentazione e prova a rispondere.  
    Inst1st.replaceAllUsesWith(NewInst);

    // Impostare option1 o option2 a true per le due rispettive 
    // modalità di aggiornamento delle singole references
    bool option1 = false;
    bool option2 = false;
    assert((option1 == false || option2 == false)||(option1 != option2));
    if (option1)
    {
        // Opzione 1: 
        Inst1st.user_begin()->setOperand(0, NewInst); // Aggiorna il primo uso di Inst1st a NewInst
    }
    if (option2)
    {
        // Opzione 2:
        unsigned int useCount = 0;
        for (auto useIt = Inst1st.use_begin(); useIt != Inst1st.use_end(); ++useIt) // Itera sugli usi di Inst1st
        {
            // Attenzione: gli usi di Inst1st memorizzati nella list puntata da Ist1st.use_begin()
            // iniziano dall'uso più vecchio dell'istruzione
            ++useCount;
            if (useCount == 1) // Se è il primo uso di Inst1st
            {   
                outs()<<"Rimpiazzo ";
                Inst1st.printAsOperand(outs(), false);
                outs()<<" nell'istruzione:"<<*(useIt->getUser())<<" con ";
                NewInst->printAsOperand(outs(), false);
                outs()<<"\n";

                useIt->set(NewInst);
            }
        }
    }

    int opConstPos;
    int otherOpPos;
    for (auto instIter = B.begin(); instIter != B.end(); ++instIter) // Itera sulle istruzioni del Basic Block
    {
        
        opConstPos = 0; // Posizione della costante negli operandi della mul
        otherOpPos = 0;  
        bool isMulConverted = false; // isMulConverted vale false se la instIter non è stata ancora convertita in una shift
        if ((*instIter).getOpcode() == Instruction::Mul) // Se l'istruzione instIter è una Mul
        {
            isMulConverted = false;
            for (auto *operandIter = (*instIter).op_begin(); operandIter != (*instIter).op_end(); ++operandIter) // Itera sugli operandi della Mul
            {
                Value *operand =  *operandIter;
                
                if (ConstantInt *constant = dyn_cast<ConstantInt>(operand)) // Se l'operando è una costante
                {
                    if (((*constant).getValue().exactLogBase2() != -1) && (isMulConverted == false)) // Se l'operando è una potenza del 2
                    {
                        // Controllo della posizione dell'operando del quale si effettuerà lo shift
                        if (opConstPos == 0)
                            otherOpPos = 1;
                        else
                            otherOpPos = 0;

                        // Crea una nuova istanza di un Value * contenente il valore newVal dello shift
                        LLVMContext& ctx = (*instIter).getOperand(opConstPos)->getContext();  
                        ConstantInt* newVal = ConstantInt::get(ctx, APInt(32, (*constant).getValue().logBase2()));  
                        Value *value = ConstantExpr::getCast(Instruction::CastOps::SExt, newVal, Type::getInt32Ty(ctx));

                        // Crea della nuova istruzione
                        Instruction *NewLeftShift = BinaryOperator::Create(
                            Instruction::Shl, (*instIter).getOperand(otherOpPos), value);

                        // Inserisce la nuova istruzione di shift dopo la Mul
                        NewLeftShift->insertAfter(&*instIter);

                        // Rimpiazza tutti gli usi del registro corrispondente alla Mul con il registro corrispondente allo shift
                        (*instIter).replaceAllUsesWith(NewLeftShift);
                        isMulConverted = true;
                    }
                }
                ++opConstPos;
            }
        }
    }
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

