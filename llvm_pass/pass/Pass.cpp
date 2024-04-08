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
            return func_name == "funcStartLogger" || func_name == "funcEndLogger" || func_name == "callLogger";
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
                {
                    builder.getInt8Ty()->getPointerTo(),
                    builder.getInt64Ty()->getPointerTo()
                };
            FunctionType *funcStartLogFuncType = 
                // {
                    FunctionType::get(retType, funcStartParamTypes, false);
                // };
                
            FunctionCallee funcStartLogFunc = 
                F.getParent()->getOrInsertFunction( "funcStartLogger"
                                                  , funcStartLogFuncType);

            //Prepare callLogger function
            ArrayRef<Type *> callParamTypes = { builder.getInt64Ty()->getPointerTo(),
                                                builder.getInt8Ty()->getPointerTo(),
                                                builder.getInt8Ty()->getPointerTo(),
                                                Type::getInt64Ty(Ctx),
                                                Type::getInt64Ty(Ctx)};
            FunctionType *callLogFuncType = FunctionType::get(retType, callParamTypes, false);
            FunctionCallee callLogFunc = F.getParent()->getOrInsertFunction("callLogger", callLogFuncType);

            // Prepare funcEndLogger function
            ArrayRef<Type *> funcEndParamTypes = 
                {
                    builder.getInt8Ty()->getPointerTo(),    // func name
                    builder.getInt64Ty()->getPointerTo(),   // bb address
                    Type::getInt32Ty(Ctx),                  // return value
                    Type::getInt64Ty(Ctx)                   // num instruction
                };
            FunctionType *funcEndLogFuncType = 
                // {
                    FunctionType::get(retType, funcEndParamTypes, false);
                // };
            FunctionCallee funcEndLogFunc = 
                F.getParent()->getOrInsertFunction("funcEndLogger", funcEndLogFuncType);

            // Prepare to get return value

            // for (auto &B : F)
            for (auto cur = F.begin(), end = F.end(); cur != end; ++cur)
            {

                // Insert a call to funcStartLogger function in the function begin

                // builder.SetInsertPoint(&B.front());
                builder.SetInsertPoint(&cur->front());
                Value *func_name = builder.CreateGlobalStringPtr(F.getName());
                // Value *bbAddress = ConstantInt::get(builder.getInt64Ty(), (int64_t)(&B));
                Value *bbAddress = ConstantInt::get(builder.getInt64Ty(), (int64_t)(&(*cur)));

                Value *args[] = {func_name, bbAddress};
                builder.CreateCall(funcStartLogFunc, args);

                size_t num_instruction = -1;
                for (auto &I : *cur)
                {
                    if (auto *retInstr = dyn_cast<ReturnInst>(&I))
                    {
                        // Insert after retInstr
                        builder.SetInsertPoint(retInstr);
                        

                        // Insert a call to funcEndLogFunc function
                        Value *func_name = builder.CreateGlobalStringPtr(F.getName());
                        Value *bbAddress = ConstantInt::get(builder.getInt64Ty(), (int64_t)(&(*cur)));

                        Value *returnValue = retInstr->getReturnValue();
                        Value *numInstr = ConstantInt::get(builder.getInt64Ty(), num_instruction);

                        Value *args[] = {func_name, bbAddress, returnValue, numInstr};
                        builder.CreateCall(funcEndLogFunc, args);
                    }
                    if (auto *call = dyn_cast<CallInst>(&I)) {

                        // Insert before call
                        builder.SetInsertPoint(call);

                        // Insert a call to callLogger function
                        Function *callee = call->getCalledFunction();
                        if (callee && !isLogger(callee->getName())) {

                            Value *bbAddress = ConstantInt::get(builder.getInt64Ty(), (int64_t)(&(*cur)));
                            Value *funcName = builder.CreateGlobalString(F.getName());
                            Value *calleeName = builder.CreateGlobalStringPtr(callee->getName());
                            Value *argument = call->getArgOperand(0);
                            Value *numInstr = ConstantInt::get(builder.getInt64Ty(), num_instruction);

                            Value *args[] = {bbAddress, funcName, calleeName, argument, numInstr};
                            builder.CreateCall(callLogFunc, args);
                        }
                    }
                    num_instruction++;
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