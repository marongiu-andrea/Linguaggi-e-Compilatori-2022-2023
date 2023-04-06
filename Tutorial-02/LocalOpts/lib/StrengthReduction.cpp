#include "LocalOpts.h"
#include <llvm/IR/InstrTypes.h>
#include <llvm/Support/Casting.h>
#include <llvm/ADT/APInt.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/IRBuilder.h>
#include <cmath>

using namespace llvm;

bool StrengthReductionPass::runOnBasicBlock(BasicBlock &B) {
    
    std::vector<std::pair<Instruction*, Instruction*>> inst_to_replace;
    
    for (auto iter = B.begin(); iter != B.end(); ++iter)
    {
         // cerco le istruzioni di moltiplicazione 
        Instruction &i = *iter;
        if (i.getOpcode() == Instruction::Mul)
        {
            Value *other_op;
            ConstantInt *const_int = nullptr;
            unsigned int pos = 0;
            int cnt = 0;
            for (auto& it : i.operands())
            {
                if (nullptr != (const_int = dyn_cast<ConstantInt>(it)))
                {
                    break;
                }
                ++pos;
                ++cnt;
            }
            // lavoro con solo due operandi, quindi se pos è 0, l'altro operando è alla posizione 1 e viceversa. per trovare la posizione dell'altro faccio 1-pos
            if (const_int)
            {
                
                other_op = i.getOperand(1-pos);
            }
            else
                continue;
                
            // ora controllo se il valore costante è multiplo di 2 oppure è a distanza 1 dal multiplo di 2
            const APInt const_val = const_int->getValue();
            if (const_val.isNegative() || const_val.isZero() || const_val.isOne())
            {
                // caso negativo, 0 o 1 non sono utili da ottimizzare
                continue;
            }
            else if (const_val.isPowerOf2())
            {
                // utilizzo classe builder di LLVM: l'istruzione passata diventa il punto di inserimento per nuove istruzioni
                IRBuilder builder(&i);
                // considero consistenza dei flag nsw e nuw della nuova istruzione con quella che andrò a sostituire
                Value *shl = builder.CreateShl(other_op, const_val.logBase2(), "sr", i.hasNoUnsignedWrap(), i.hasNoSignedWrap());
                inst_to_replace.push_back(std::pair<Instruction*, Instruction*>(&i, cast<Instruction>(shl)));
            }
            
            else if (const_val == (1 << const_val.nearestLogBase2()) + 1)
            {
                IRBuilder builder(&i);
                Value *shl = builder.CreateShl(other_op, const_val.logBase2(), "sr", i.hasNoUnsignedWrap(), i.hasNoSignedWrap());
                // la costante per cui viene moltiplicato il valore è 1 in più di una potenza di 2 -> ottengo il risultato della moltiplicazione
                // aggiungendo il valore dell'operando non costante al risultato della shift
                Value *addV = builder.CreateAdd(other_op, shl, "addv", i.hasNoUnsignedWrap(), i.hasNoSignedWrap());
                outs() << "sto creando una nuova istruzione" << "\n";
                inst_to_replace.push_back(std::pair<Instruction*, Instruction*>(&i, cast<Instruction>(addV)));
            }
            else if (const_val == (1 << const_val.nearestLogBase2()) - 1)
            {
                IRBuilder builder(&i);
                // considero consistenza dei flag nsw e nuw della nuova istruzione con quella che andrò a sostituire
                Value *shl = builder.CreateShl(other_op, const_val.logBase2(), "sr", i.hasNoUnsignedWrap(), i.hasNoSignedWrap());
                // la costante per cui viene moltiplicato il valore è 1 meno di una potenza di 2 -> ottengo il risultato della moltiplicazione
                // sottraendo il valore dell'operando non costante al risultato della shift
                Value *subV = builder.CreateSub(other_op, shl, "subv", i.hasNoUnsignedWrap(), i.hasNoSignedWrap());
                outs() << "sto creando una nuova istruzione" << "\n";
                inst_to_replace.push_back(std::pair<Instruction*, Instruction*>(&i, cast<Instruction>(subV)));
            }
        }
    }
    
    for (auto& instrs : inst_to_replace)
    {
        instrs.first->replaceAllUsesWith(instrs.second);
        instrs.first->eraseFromParent();
    }

    return true;
  }

