/**
 * This file is part of Rellume.
 *
 * (c) 2016-2019, Alexis Engelke <alexis.engelke@googlemail.com>
 *
 * Rellume is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation, either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Rellume is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Rellume.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file
 **/

#ifndef RELLUME_LIFTER_PRIVATE_H
#define RELLUME_LIFTER_PRIVATE_H

#include "basicblock.h"
#include "config.h"
#include "facet.h"
#include "function-info.h"
#include "regfile.h"
#include "rellume/instr.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Operator.h>
#include <vector>


namespace rellume {

enum Alignment {
    /// Implicit alignment -- MAX for SSE operand, 1 otherwise
    ALIGN_IMP = -1,
    /// Maximum alignment, alignment is set to the size of the value
    ALIGN_MAX = 0,
    /// No alignment (1 byte)
    ALIGN_NONE = 1,
    ALIGN_MAXIMUM = ALIGN_MAX,
    ALIGN_1 = ALIGN_NONE,
};

enum class Condition {
    O = 0, NO = 1, C = 2, NC = 3, Z = 4, NZ = 5, BE = 6, A = 7,
    S = 8, NS = 9, P = 10, NP = 11, L = 12, GE = 13, LE = 14, G = 15,
};

/**
 * \brief The LLVM state of the back-end.
 **/
class LifterBase {
protected:
    LifterBase(FunctionInfo& fi, const LLConfig& cfg, ArchBasicBlock& ab)
            : fi(fi), cfg(cfg), ablock(ab),
              regfile(ablock.GetInsertBlock()->GetRegFile()),
              irb(regfile->GetInsertBlock()) {
        // Set fast-math flags. Newer LLVM supports FastMathFlags::getFast().
        if (cfg.enableFastMath) {
            llvm::FastMathFlags fmf;
            fmf.setFast();
            irb.setFastMathFlags(fmf);
        }
    }

public:
    LifterBase(LifterBase&& rhs);
    LifterBase& operator=(LifterBase&& rhs);

    LifterBase(const LifterBase&) = delete;
    LifterBase& operator=(const LifterBase&) = delete;

protected:
    FunctionInfo& fi;
    const LLConfig& cfg;
private:
    ArchBasicBlock& ablock;

protected:
    /// Current register file
    RegFile* regfile;

    llvm::IRBuilder<> irb;


    llvm::Module* GetModule() {
        return irb.GetInsertBlock()->getModule();
    }

    X86Reg MapReg(const LLReg reg);

    llvm::Value* GetReg(X86Reg reg, Facet facet) {
        return regfile->GetReg(reg, facet);
    }
    void SetReg(X86Reg reg, Facet facet, llvm::Value* value) {
        fi.ModifyReg(reg);
        regfile->SetReg(reg, facet, value, true); // clear all other facets
    }
    void SetRegFacet(X86Reg reg, Facet facet, llvm::Value* value) {
        // TODO: be more accurate about flags
        // Currently, when a single flag is modified, all other flags are marked
        // as modified as well.
        fi.ModifyReg(reg);
        regfile->SetReg(reg, facet, value, false);
    }
    llvm::Value* GetFlag(Facet facet) {
        return GetReg(X86Reg::EFLAGS, facet);
    }
    void SetFlag(Facet facet, llvm::Value* value) {
        SetRegFacet(X86Reg::EFLAGS, facet, value);
    }
    void SetFlagUndef(std::initializer_list<Facet> facets) {
        llvm::Value* undef = llvm::UndefValue::get(irb.getInt1Ty());
        for (const auto facet : facets)
            SetRegFacet(X86Reg::EFLAGS, facet, undef);
    }

    void SetInsertBlock(BasicBlock* block) {
        ablock.SetInsertBlock(block);
        regfile = block->GetRegFile();
        irb.SetInsertPoint(regfile->GetInsertBlock());
    }

    // Operand handling implemented in lloperand.cc
private:
    llvm::Value* OpAddrConst(uint64_t addr, llvm::PointerType* ptr_ty);
protected:
    llvm::Value* OpAddr(const LLInstrOp& op, llvm::Type* element_type);
    llvm::Value* OpLoad(const LLInstrOp& op, Facet dataType, Alignment alignment = ALIGN_NONE);
    void OpStoreGp(const LLInstrOp& op, llvm::Value* value, Alignment alignment = ALIGN_NONE);
    void OpStoreVec(const LLInstrOp& op, llvm::Value* value, bool avx = false, Alignment alignment = ALIGN_IMP);
    void StackPush(llvm::Value* value);
    llvm::Value* StackPop(const X86Reg sp_src_reg = X86Reg::GP(LL_RI_SP));

    // llflags.cc
    void FlagCalcZ(llvm::Value* value) {
        auto zero = llvm::Constant::getNullValue(value->getType());
        SetFlag(Facet::ZF, irb.CreateICmpEQ(value, zero));
    }
    void FlagCalcS(llvm::Value* value) {
        auto zero = llvm::Constant::getNullValue(value->getType());
        SetFlag(Facet::SF, irb.CreateICmpSLT(value, zero));
    }
    void FlagCalcP(llvm::Value* value);
    void FlagCalcA(llvm::Value* res, llvm::Value* lhs, llvm::Value* rhs);
    void FlagCalcCAdd(llvm::Value* res, llvm::Value* lhs, llvm::Value* rhs) {
        SetFlag(Facet::CF, irb.CreateICmpULT(res, lhs));
    }
    void FlagCalcCSub(llvm::Value* res, llvm::Value* lhs, llvm::Value* rhs) {
        SetFlag(Facet::CF, irb.CreateICmpULT(lhs, rhs));
    }
    void FlagCalcOAdd(llvm::Value* res, llvm::Value* lhs, llvm::Value* rhs);
    void FlagCalcOSub(llvm::Value* res, llvm::Value* lhs, llvm::Value* rhs);
    void FlagCalcAdd(llvm::Value* res, llvm::Value* lhs, llvm::Value* rhs) {
        FlagCalcZ(res);
        FlagCalcS(res);
        FlagCalcP(res);
        FlagCalcA(res, lhs, rhs);
        FlagCalcCAdd(res, lhs, rhs);
        FlagCalcOAdd(res, lhs, rhs);
    }
    void FlagCalcSub(llvm::Value* res, llvm::Value* lhs, llvm::Value* rhs) {
        SetFlag(Facet::ZF, irb.CreateICmpEQ(lhs, rhs));
        FlagCalcS(res);
        FlagCalcP(res);
        FlagCalcA(res, lhs, rhs);
        FlagCalcCSub(res, lhs, rhs);
        FlagCalcOSub(res, lhs, rhs);
    }

    llvm::Value* FlagCond(Condition cond);
    llvm::Value* FlagAsReg(unsigned size);
    void FlagFromReg(llvm::Value* val);

    struct RepInfo {
        enum RepMode { NO_REP, REP, REPZ, REPNZ };
        RepMode mode;
        BasicBlock* loop_block;
        BasicBlock* cont_block;

        llvm::Value* di;
        llvm::Value* si;
    };
    RepInfo RepBegin(const LLInstr& inst);
    void RepEnd(RepInfo info);

    // Helper function for older LLVM versions
    llvm::Value* CreateUnaryIntrinsic(llvm::Intrinsic::ID id, llvm::Value* v) {
        // TODO: remove this helper function
        return irb.CreateUnaryIntrinsic(id, v);
    }
};

class Lifter : public LifterBase {
public:
    Lifter(FunctionInfo& fi, const LLConfig& cfg, ArchBasicBlock& ab) :
            LifterBase(fi, cfg, ab) {}

    // llinstruction-gp.cc
    bool Lift(const LLInstr&);

private:
    void LiftOverride(const LLInstr&, llvm::Function* override);

    void LiftMovgp(const LLInstr&, llvm::Instruction::CastOps cast);
    void LiftAdd(const LLInstr&);
    void LiftAdc(const LLInstr&);
    void LiftXadd(const LLInstr&);
    void LiftSub(const LLInstr&);
    void LiftSbb(const LLInstr&);
    void LiftCmp(const LLInstr&);
    void LiftCmpxchg(const LLInstr&);
    void LiftXchg(const LLInstr&);
    void LiftAndOrXor(const LLInstr& inst, llvm::Instruction::BinaryOps op,
                      bool writeback = true);
    void LiftNot(const LLInstr&);
    void LiftNeg(const LLInstr&);
    void LiftIncDec(const LLInstr&);
    void LiftShift(const LLInstr&, llvm::Instruction::BinaryOps);
    void LiftRotate(const LLInstr&);
    void LiftShiftdouble(const LLInstr&);
    void LiftMul(const LLInstr&);
    void LiftDiv(const LLInstr&);
    void LiftLea(const LLInstr&);
    void LiftXlat(const LLInstr&);
    void LiftCmovcc(const LLInstr& inst, Condition cond);
    void LiftSetcc(const LLInstr& inst, Condition cond);
    void LiftCext(const LLInstr& inst);
    void LiftCsep(const LLInstr& inst);
    void LiftBitscan(const LLInstr& inst, bool trailing);
    void LiftBittest(const LLInstr& inst);
    void LiftMovbe(const LLInstr& inst);
    void LiftBswap(const LLInstr& inst);

    void LiftLahf(const LLInstr& inst) {
        OpStoreGp(LLInstrOp(LLReg(LL_RT_GP8Leg, LL_RI_AH)), FlagAsReg(8));
    }
    void LiftSahf(const LLInstr& inst) {
        FlagFromReg(OpLoad(LLReg(LL_RT_GP8Leg, LL_RI_AH), Facet::I8));
    }
    void LiftPush(const LLInstr& inst) {
        StackPush(OpLoad(inst.ops[0], Facet::I));
    }
    void LiftPushf(const LLInstr& inst) {
        StackPush(FlagAsReg(inst.operand_size * 8));
    }
    void LiftPop(const LLInstr& inst) {
        OpStoreGp(inst.ops[0], StackPop());
    }
    void LiftPopf(const LLInstr& inst) {
        FlagFromReg(StackPop());
    }
    void LiftLeave(const LLInstr& inst) {
        llvm::Value* val = StackPop(X86Reg::GP(LL_RI_BP));
        OpStoreGp(LLInstrOp(LLReg(LL_RT_GP64, LL_RI_BP)), val);
    }

    void LiftJmp(const LLInstr& inst);
    void LiftJcc(const LLInstr& inst, Condition cond);
    void LiftJcxz(const LLInstr& inst);
    void LiftLoop(const LLInstr& inst);
    void LiftCall(const LLInstr& inst);
    void LiftRet(const LLInstr& inst);
    void LiftUnreachable(const LLInstr& inst);

    void LiftClc(const LLInstr& inst) { SetFlag(Facet::CF, irb.getFalse()); }
    void LiftStc(const LLInstr& inst) { SetFlag(Facet::CF, irb.getTrue()); }
    void LiftCmc(const LLInstr& inst) {
        SetFlag(Facet::CF, irb.CreateNot(GetFlag(Facet::CF)));
    }

    void LiftCld(const LLInstr& inst) { SetFlag(Facet::DF, irb.getFalse()); }
    void LiftStd(const LLInstr& inst) { SetFlag(Facet::DF, irb.getTrue()); }
    void LiftLods(const LLInstr& inst);
    void LiftStos(const LLInstr& inst);
    void LiftMovs(const LLInstr& inst);
    void LiftScas(const LLInstr& inst);
    void LiftCmps(const LLInstr& inst);

    // llinstruction-sse.cc
    void LiftFence(const LLInstr&);
    void LiftPrefetch(const LLInstr&, unsigned rw, unsigned locality);
    void LiftFxsave(const LLInstr&);
    void LiftFxrstor(const LLInstr&);
    void LiftFstcw(const LLInstr&);
    void LiftFstsw(const LLInstr&);
    void LiftStmxcsr(const LLInstr&);
    void LiftSseMovq(const LLInstr&, Facet type);
    void LiftSseBinOp(const LLInstr&, llvm::Instruction::BinaryOps op,
                      Facet type);
    void LiftSseMovScalar(const LLInstr&, Facet);
    void LiftSseMovdq(const LLInstr&, Facet, Alignment);
    void LiftSseMovntStore(const LLInstr&, Facet);
    void LiftSseMovlp(const LLInstr&);
    void LiftSseMovhps(const LLInstr&);
    void LiftSseMovhpd(const LLInstr&);
    void LiftSseAndn(const LLInstr&, Facet op_type);
    void LiftSseComis(const LLInstr&, Facet);
    void LiftSseCmp(const LLInstr&, Facet op_type);
    void LiftSseMinmax(const LLInstr&, llvm::CmpInst::Predicate, Facet);
    void LiftSseSqrt(const LLInstr&, Facet op_type);
    void LiftSseCvt(const LLInstr&, Facet src_type, Facet dst_type);
    void LiftSseUnpck(const LLInstr&, Facet type);
    void LiftSseShufpd(const LLInstr&);
    void LiftSseShufps(const LLInstr&);
    void LiftSsePshufd(const LLInstr&);
    void LiftSsePshufw(const LLInstr&, unsigned off);
    void LiftSseInsertps(const LLInstr&);
    void LiftSsePinsr(const LLInstr&, Facet, Facet, unsigned);
    void LiftSsePextr(const LLInstr&, Facet, unsigned);
    void LiftSsePshiftElement(const LLInstr&, llvm::Instruction::BinaryOps op, Facet op_type);
    void LiftSsePshiftBytes(const LLInstr&);
    void LiftSsePavg(const LLInstr&, Facet);
    void LiftSsePmulhw(const LLInstr&, llvm::Instruction::CastOps cast);
    void LiftSsePaddsubSaturate(const LLInstr& inst,
                                llvm::Instruction::BinaryOps calc_op, bool sign,
                                Facet op_ty);
    void LiftSsePack(const LLInstr&, Facet, bool sign);
    void LiftSsePcmp(const LLInstr&, llvm::CmpInst::Predicate, Facet);
    void LiftSsePminmax(const LLInstr&, llvm::CmpInst::Predicate, Facet);
    void LiftSseMovmsk(const LLInstr&, Facet op_type);
};

} // namespace

#endif
