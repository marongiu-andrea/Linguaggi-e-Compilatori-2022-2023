#include "LoopInvariantCodeMotionPass.hpp"
#include <llvm/ADT/STLExtras.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Use.h>
#include <llvm/IR/Value.h>

using namespace llvm;

char LoopInvariantCodeMotionPass::ID = 0;

void LoopInvariantCodeMotionPass::analyze(Loop* loop)
{
    this->currentLoop = loop;
    this->dominatorTree = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();

    searchForLoopInvariantInstructionsAndLoopExitingBlocks();
    searchForHoistableInstructions();

    printAnalysisResult();
}

void LoopInvariantCodeMotionPass::searchForLoopInvariantInstructionsAndLoopExitingBlocks()
{
    for (auto* bb : this->currentLoop->getBlocks())
    {
        std::set<Instruction*> bbLoopInvariantInstructions = getLoopInvariantInstructions(bb);

        this->loopInvariantInstructions.insert(bbLoopInvariantInstructions.begin(), bbLoopInvariantInstructions.end());

        if (isLoopExiting(bb))
        {
            this->loopExitingBlocks.insert(bb);
        }
    }
}

std::set<Instruction*> LoopInvariantCodeMotionPass::getLoopInvariantInstructions(BasicBlock* bb)
{
    auto loopInvariantInstructionsRange = make_filter_range(*bb, [this](Instruction& instr) {
        return instr.isBinaryOp() && isLoopInvariant(instr);
    });

    // converto il range di Instruction& in un range di Instruction* per poterle aggiungere al set loopInvariantInstructions
    auto instructionPointersRange = map_range(loopInvariantInstructionsRange, [](Instruction& loopInvariantInstruction) {
        return &loopInvariantInstruction;
    });

    return {instructionPointersRange.begin(), instructionPointersRange.end()};
}

void LoopInvariantCodeMotionPass::searchForHoistableInstructions()
{
    auto hoistableInstructionsRange = make_filter_range(this->loopInvariantInstructions, [this](Instruction* instr) {
        return dominatesAllExits(instr) || isDeadAfterLoop(instr);
    });

    this->hoistableInstructions.insert(hoistableInstructionsRange.begin(), hoistableInstructionsRange.end());
}

bool LoopInvariantCodeMotionPass::isLoopInvariant(const Instruction& instr)
{
    // un'istruzione è loop invariant solo se tutti i suoi operandi sono loop invariant
    return all_of(instr.operands(), [this](auto& operand) {
        return isLoopInvariant(operand);
    });
}

bool LoopInvariantCodeMotionPass::isLoopInvariant(const Use& operand)
{
    Value* reachingDefinition = operand.get();

    if (dyn_cast<Constant>(reachingDefinition) || dyn_cast<Argument>(reachingDefinition))
    {
        // costanti e argomenti di funzione sono sempre loop invariant
        return true;
    }
    else if (Instruction* operandDefinitionInstr = dyn_cast<Instruction>(reachingDefinition))
    {
        /*
         * se l'operando è una variabile, è loop invariant se una delle seguenti condizioni è verificata:
          - la definizione dell'operando è fuori dal loop;
          - la definizione dell'operando è interna al loop ed è stata marcata come loop invariant
         */
        return !this->currentLoop->contains(operandDefinitionInstr) || this->loopInvariantInstructions.contains(operandDefinitionInstr);
    }

    errs() << "ATTENZIONE! Tipo di reaching definition non gestito. Reaching definition: " << *reachingDefinition << "\n";

    return false;
}

bool LoopInvariantCodeMotionPass::isLoopExiting(const BasicBlock* bb)
{
    // un basic block è un punto d'uscita del loop se il suo next è fuori dal loop
    return !this->currentLoop->contains(bb->getNextNode());
}

bool LoopInvariantCodeMotionPass::dominatesAllExits(const Instruction* instr)
{
    return all_of(this->loopExitingBlocks, [this, &instr](BasicBlock* exitingBlock) {
        return this->dominatorTree->dominates(instr, exitingBlock);
    });
}

bool LoopInvariantCodeMotionPass::isDeadAfterLoop(const Instruction* instr)
{
    // l'istruzione è dead fuori dal loop se e solo se tutti i suoi usi sono interni al loop
    return all_of(instr->uses(), [this](const Use& use) {
        Instruction* user = dyn_cast<Instruction>(use.getUser());

        if (user == nullptr)
        {
            // tutti gli usi di un'istruzione dovrebbero essere a loro volta delle istruzioni, ma per sicurezza faccio un check

            errs() << "ATTENZIONE! Ho trovato un uso che non è un'istruzione: " << *use << "\n";

            return false;
        }
        else
        {
            return this->currentLoop->contains(user);
        }
    });
}

void LoopInvariantCodeMotionPass::printAnalysisResult()
{
    outs() << "--- ANALYSIS RESULT: ---\n";

    outs() << "Loop invariant instructions:\n";
    for (auto* instr : loopInvariantInstructions)
    {
        outs() << *instr << '\n';
    }

    outs() << "Hoistable instructions:\n";
    for (auto* instr : hoistableInstructions)
    {
        outs() << *instr << '\n';
    }

    outs() << "-------------------------\n";
}

LoopInvariantCodeMotionPass::LoopInvariantCodeMotionPass() :
    LoopPass(ID)
{
}

void LoopInvariantCodeMotionPass::getAnalysisUsage(AnalysisUsage& AU) const
{
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}

bool LoopInvariantCodeMotionPass::runOnLoop(Loop* loop, LPPassManager& LPM)
{
    BasicBlock* preheader = loop->getLoopPreheader();
    bool changed = false;

    if (preheader == nullptr)
    {
        errs() << "Il loop non è in forma normalizzata. Non eseguo nessuna operazione.\n";

        return changed;
    }

    this->analyze(loop);

    IRBuilder<> builder(preheader, preheader->begin());

    for (auto* instr : hoistableInstructions)
    {
        instr->removeFromParent();
        builder.Insert(instr);

        changed = true;
    }

    return changed;
}

RegisterPass<LoopInvariantCodeMotionPass> X("custom-licm", "Custom LICM Pass");
