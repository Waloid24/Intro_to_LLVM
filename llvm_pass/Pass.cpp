#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

#include "graph/instruction_graph.hpp"

using namespace llvm;

namespace {
    struct MyPass : public FunctionPass {
        static char ID;

        MyPass() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {

            std::error_code EC;
            raw_fd_ostream file_stream("insrtuction_graph.dot", EC, sys::fs::OpenFlags::OF_Append);

            file_stream << "In a function called " << F.getName() << "!\n";
            file_stream << "Function body:\n";
            F.print(file_stream);

            for (auto &B : F) {
                file_stream << "Basic block:\n";
                B.print(file_stream);

                for (auto &I : B) {
                    file_stream << "Instruction: \n";
                    I.print(file_stream, true);
                    file_stream << "\n";
                }   
            }

            LLVMContext &Ctx = F.getContext();
            IRBuilder<> builder(Ctx);
            Type *retType = Type::getVoidTy(Ctx);

            // Prepare funcStartLogger function
            ArrayRef<Type *> funcStartParamTypes = {
                builder.getInt8Ty()->getPointerTo()};
            FunctionType *funcStartLogFuncType = 
                FunctionType::get(retType, funcStartParamTypes, false);
            FunctionCallee funcStartLogFunc = F.getParent()->getOrInsertFunction(
                "funcStartLogger", funcStartLogFuncType);

            // Insert a call to funcStartLogger function in the function begin
            BasicBlock &entryBB = F.getEntryBlock();
            builder.SetInsertPoint(&entryBB.front());
            Value *funcName = builder.CreateGlobalStringPtr(F.getName());
            Value *args[] = {funcName};
            builder.CreateCall(funcStartLogFunc, args);

            return false;
        }
    };
}

char MyPass::ID = 0;

static void registerMyPass(const PassManagerBuilder &,
                           legacy::PassManagerBase &PM) {
    PM.add(new MyPass());
}
static RegisterStandardPasses
    RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                   registerMyPass);