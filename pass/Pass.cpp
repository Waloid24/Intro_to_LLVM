#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"


using namespace llvm;

namespace {
    struct MyPass : public FunctionPass {
        static char ID;

        MyPass() : FunctionPass(ID) {}

        bool isLogger(const StringRef& func_name)
        {
            return func_name == "funcStartLogger" || func_name == "funcEndLogger";
        }

        bool runOnFunction(Function &F) override {

            if (isLogger(F.getName()))
            {
                return false;
            }

            std::error_code EC;
            raw_fd_ostream file_stream("vizualization/static.txt", EC, sys::fs::OpenFlags::OF_Append);

            file_stream << "function " << F.getName() << "\n";

            for (auto &B : F) {
                file_stream << "bb " << &B << "\n";
                // B.print(file_stream);

                for (auto &I : B) {
                    I.print(file_stream, true);
                    file_stream << "\n";
                }   
            }

            LLVMContext &Ctx = F.getContext();
            IRBuilder<> builder(Ctx);
            Type *retType = Type::getVoidTy(Ctx);

            // Prepare funcStartLogger function
            ArrayRef<Type *> funcStartParamTypes = 
                // {
                    builder.getInt8Ty()->getPointerTo();
                    builder.getInt64Ty()->getPointerTo();
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
                    builder.getInt8Ty()->getPointerTo(),    // func name
                    builder.getInt64Ty()->getPointerTo(),   // bb address
                    Type::getInt32Ty(Ctx)                   // return value
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
                if (F.getReturnType()->isIntegerTy())
                    outs() << "IT'S INTEGER TYPE\n";
                else
                    outs() << "IT ISN'T INTEGER TYPE\n";

                // Insert a call to funcStartLogger function in the function begin
                builder.SetInsertPoint(&B.front());
                Value *func_name = builder.CreateGlobalStringPtr(F.getName());
                // Value *bbAddress = static_cast<Value *>(&B);
                Value *bbAddress = ConstantInt::get(builder.getInt64Ty(), (int64_t)(&B));
                Value *args[] = {func_name, bbAddress};
                builder.CreateCall(funcStartLogFunc, args);
                for (auto &I : B)
                {
                    if (auto *retInstr = dyn_cast<ReturnInst>(&I))
                    {
                        // Insert after retInstr
                        builder.SetInsertPoint(retInstr);

                        // Insert a call to funcEndLogFunc function
                        Value *func_name = builder.CreateGlobalStringPtr(F.getName());
                        Value *bbAddress = ConstantInt::get(builder.getInt64Ty(), (int64_t)(&B));
                        Value *returnValue = retInstr->getReturnValue();

                        Value *args[] = {func_name, bbAddress, returnValue};
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