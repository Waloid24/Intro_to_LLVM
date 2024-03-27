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

        bool isLogger(const StringRef& funcName)
        {
            return funcName == "funcStartLogger" || funcName == "funcEndLogger";
        }

        bool runOnFunction(Function &F) override {

            if (isLogger(F.getName()))
            {
                return false;
            }

            // std::error_code EC;
            // raw_fd_ostream file_stream("insrtuction_graph.dot", EC, sys::fs::OpenFlags::OF_Append);

            // file_stream << "In a function called " << F.getName() << "!\n";
            // file_stream << "Function body:\n";
            // F.print(file_stream);

            // for (auto &B : F) {
            //     file_stream << "Basic block:\n";
            //     B.print(file_stream);

            //     for (auto &I : B) {
            //         file_stream << "Instruction: \n";
            //         I.print(file_stream, true);
            //         file_stream << "\n";
            //     }   
            // }

            LLVMContext &Ctx = F.getContext();
            IRBuilder<> builder(Ctx);
            Type *retType = Type::getVoidTy(Ctx);

            // Prepare funcStartLogger function
            ArrayRef<Type *> funcStartParamTypes = 
                // {
                    builder.getInt8Ty()->getPointerTo();
                // };
            FunctionType *funcStartLogFuncType = 
                // {
                    FunctionType::get(retType, funcStartParamTypes, false);
                // };
                
            FunctionCallee funcStartLogFunc = 
                F.getParent()->getOrInsertFunction( "funcStartLogger"
                                                  , funcStartLogFuncType);

            // Prepare funcEndLogger function
            ArrayRef<Type *> funcEndParamTypes = 
                {
                    builder.getInt8Ty()->getPointerTo()
                    // Type::getInt32Ty(Ctx)
                };
            FunctionType *funcEndLogFuncType = 
                // {
                    FunctionType::get(retType, funcEndParamTypes, false);
                // };
            FunctionCallee funcEndLogFunc = 
                F.getParent()->getOrInsertFunction("funcEndLogger", funcEndLogFuncType);

            // Prepare to get return value
            for (auto &B : F)
            {
                outs() << "for (auto &B : F)\n";
                // Insert a call to funcStartLogger function in the function begin
                builder.SetInsertPoint(&B.front());
                Value *funcName = builder.CreateGlobalStringPtr(F.getName());
                Value *args[] = {funcName};
                builder.CreateCall(funcStartLogFunc, args);
                for (auto &I : B)
                {
                    if (auto *retInstr = dyn_cast<ReturnInst>(&I))
                    {
                        // Insert after retInstr
                        builder.SetInsertPoint(retInstr);

                        // Insert a call to funcEndLogFunc function
                        Value *funcName = builder.CreateGlobalStringPtr(F.getName());
                        // Value *returnValue = retInstr->getReturnValue();
                        // Value *args[] = {funcName, retInstr};
                        Value *args[] = {funcName};
                        builder.CreateCall(funcEndLogFunc, args);
                    }
                }
            }
            return true;
        }
    };

    
} /* anonymous namespace */

char MyPass::ID = 0;

static void registerMyPass(const PassManagerBuilder &,
                           legacy::PassManagerBase &PM) {
    PM.add(new MyPass());
}
static RegisterStandardPasses
    RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                   registerMyPass);