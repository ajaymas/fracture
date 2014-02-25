//===--- Decompiler - Decompiles machine basic blocks -----------*- C++ -*-===//
//
//              Fracture: The Draper Decompiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class uses the disassembler and inverse instruction selector classes
// to decompile target specific code into LLVM IR and return function objects
// back to the user.
//
//===----------------------------------------------------------------------===//

#ifndef DECOMPILER_H
#define DECOMPILER_H

#include "llvm/ADT/IndexedMap.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/GCMetadata.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/CFG.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/PromoteMemToReg.h"
#include "llvm/PassManager.h"

#include "CodeInv/InvISelDAG.h"
#include "CodeInv/Disassembler.h"
#include "Transforms/TypeRecovery.h"

using namespace llvm;

namespace fracture {

class Decompiler {
public:

  Decompiler(Disassembler *NewDis, Module *NewMod = NULL,
    raw_ostream &InfoOut = nulls(), raw_ostream &ErrOut = nulls());
  ~Decompiler();

  void printInstructions(formatted_raw_ostream &Out, unsigned Address);


  Function* decompileFunction(unsigned Address);
  BasicBlock* decompileBasicBlock(MachineBasicBlock *MBB, Function *F);

  BasicBlock* getOrCreateBasicBlock(unsigned Address, Function *F);
  BasicBlock* getOrCreateBasicBlock(StringRef BBName, Function *F);

  void sortBasicBlock(BasicBlock *BB);
  void splitBasicBlockIntoBlock(Function::iterator Src,
    BasicBlock::iterator FirstInst, BasicBlock *Tgt);

  SelectionDAG* createDAGFromMachineBasicBlock(MachineBasicBlock *MBB);

  uint64_t getBasicBlockAddress(BasicBlock *BB);

  SelectionDAG* getCurrentDAG() { return DAG; }
  Disassembler* getDisassembler() { return Dis; }
  void setViewMCDAGs(bool Setting) { ViewMCDAGs = Setting; }
  void setViewIRDAGs(bool Setting) { ViewIRDAGs = Setting; }
  Module* getModule() { return Mod; }
private:
  Disassembler *Dis;
  Module *Mod;
  LLVMContext *Context;
  InvISelDAG *InvISel;
  SelectionDAG* DAG;
  bool ViewMCDAGs;
  bool ViewIRDAGs;
  IREmitter *Emitter;

  static void printSDNode(std::map<SDValue, std::string> &OpMap,
    std::stack<SDNode *> &NodeStack, SDNode *CurNode, SelectionDAG *DAG);
  static void printDAG(SelectionDAG *DAG);

  /// Error printing
  raw_ostream &Infos, &Errs;
  void printInfo(std::string Msg) const {
    Infos << "Disassembler: " << Msg << "\n";
  }
  void printError(std::string Msg) const {
    Errs << "Disassembler: " << Msg << "\n";
    Errs.flush();
  }
};

} // end namespace fracture

#endif /* DECOMPILER_H */
