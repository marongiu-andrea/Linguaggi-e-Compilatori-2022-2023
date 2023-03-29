#include "LocalOpts.h"
#include <llvm/IR/InstrTypes.h>
#include <llvm/Support/Casting.h>
#include <llvm/ADT/APInt.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/IR/Instruction.h>
#include <cmath>

using namespace llvm;

bool StrengthReductionPass::runOnBasicBlock(BasicBlock &B) {
    
    std::vector<std::pair<Instruction*, Instruction*>> inst_to_replace;
    
    for (auto iter = B.begin(); iter != B.end(); ++iter)
    {
         // cerco le istruzioni di moltiplicazione 
        Instruction &i = *iter;
        outs() << "sto iterando..." << "\n";
        if (i.getOpcode() == Instruction::Mul)
        {
            outs() << "trovata moltiplicazione con " << i.getNumOperands() << " operandi: " << i << "\n";
            if (isa<Constant>(i.getOperand(1)))
                outs() << "trovata costante: " << *(i.getOperand(1)) << "\n";
            Value *other_op;
            ConstantInt *const_int = nullptr;
            unsigned int pos = 0;
            int cnt = 0;
            for (auto& it : i.operands())
            {
                outs() << "operando " << cnt << ": " << *it << "\n";
                if (isa<Constant>(&*it))
                    outs() << "trovata moltiplicazione con costante: " << *it << "\n";
                if (nullptr != (const_int = dyn_cast<ConstantInt>(it)))
                {
                    outs() << "trovata costante come operatore: " << *it << "\n";
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
                // TODO: studiare differenza tra flag nsw e nuw
                Instruction *shl = BinaryOperator::Create(Instruction::Shl, other_op, ConstantInt::get(const_int->getType(), const_val.logBase2()));
                shl->insertAfter(&i);
                outs() << "sto creando una nuova istruzione" << "\n";
                inst_to_replace.push_back(std::pair<Instruction*, Instruction*>(&i, shl));
            }
        }
    }
    
    for (auto& instrs : inst_to_replace)
    {
        outs() << "sostituisco istruzione (" << *(instrs.first) << ") con (" << *(instrs.second) << ")\n";
        instrs.first->replaceAllUsesWith(instrs.second);
        instrs.first->eraseFromParent();
    }

    return true;
  }

