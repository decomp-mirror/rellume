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

#include "lifter-private.h"

#include "callconv.h"
#include "facet.h"
#include "rellume/instr.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm-c/Core.h>
#include <cstddef>
#include <cstdint>


/**
 * \defgroup LLInstrOp LLInstrOp
 * \brief Handling of instruction operands
 *
 * @{
 **/

namespace rellume {

X86Reg
LifterBase::MapReg(const LLReg reg) {
    if (reg.IsGp())
        return X86Reg::GP(reg.ri - (reg.IsGpHigh() ? LL_RI_AH : 0));
    else if (reg.rt == LL_RT_IP)
        return X86Reg::IP;
    else if (reg.rt == LL_RT_EFLAGS)
        return X86Reg::EFLAGS;
    else if (reg.IsVec())
        return X86Reg::VEC(reg.ri);
    return X86Reg();
}

llvm::Value*
LifterBase::OpAddrConst(uint64_t addr, llvm::PointerType* ptr_ty)
{
    if (addr == 0)
        return llvm::ConstantPointerNull::get(ptr_ty);

    if (cfg.global_base_value != nullptr)
    {
        uintptr_t offset = addr - cfg.global_base_addr;
        auto ptr = irb.CreateGEP(cfg.global_base_value, irb.getInt64(offset));
        return irb.CreatePointerCast(ptr, ptr_ty);
    }

    return irb.CreateIntToPtr(irb.getInt64(addr), ptr_ty);
}

llvm::Value*
LifterBase::OpAddr(const LLInstrOp& op, llvm::Type* element_type)
{
    if (op.seg == LL_RI_FS || op.seg == LL_RI_GS || op.addrsize != 8) {
        // For segment offsets, use inttoptr because the pointer base is stored
        // in the segment register. (And LLVM has some problems with addrspace
        // casts between pointers.) For 32-bit address size, we can't normal
        // pointers, so use integer arithmetic directly.
        Facet addrsz_facet = op.addrsize == 8 ? Facet::I64 : Facet::I32;

        llvm::Value* res = irb.getIntN(8*op.addrsize, op.val);
        if (op.reg.rt != LL_RT_None)
            res = irb.CreateAdd(res, GetReg(MapReg(op.reg), addrsz_facet));
        if (op.scale != 0) {
            llvm::Value* ireg = GetReg(MapReg(op.ireg), addrsz_facet);
            llvm::Value* scaled_val = irb.getIntN(8*op.addrsize, op.scale);
            res = irb.CreateAdd(res, irb.CreateMul(ireg, scaled_val));
        }

        int addrspace = 0;
        if (op.seg == LL_RI_FS || op.seg == LL_RI_GS) {
            if (cfg.use_native_segment_base) {
                addrspace = op.seg == LL_RI_FS ? 257 : 256;
            } else {
                unsigned idx = op.seg == LL_RI_FS ? SptrIdx::FSBASE
                                                  : SptrIdx::GSBASE;
                res = irb.CreateAdd(res, irb.CreateLoad(fi.sptr[idx]));
            }
        }

        res = irb.CreateZExt(res, irb.getInt64Ty());
        return irb.CreateIntToPtr(res, element_type->getPointerTo(addrspace));
    }

    llvm::PointerType* elem_ptr_ty = element_type->getPointerTo();

    llvm::PointerType* scale_type = nullptr;
    if (op.scale * 8u == element_type->getPrimitiveSizeInBits())
        scale_type = elem_ptr_ty;
    else if (op.scale != 0)
        scale_type = irb.getIntNTy(op.scale*8)->getPointerTo();

    llvm::Value* base;
    if (op.reg.rt != LL_RT_None)
    {
        base = GetReg(MapReg(op.reg), Facet::PTR);
        if (llvm::isa<llvm::Constant>(base))
        {
            auto base_addr = llvm::cast<llvm::ConstantInt>(GetReg(MapReg(op.reg), Facet::I64));
            base = OpAddrConst(base_addr->getZExtValue() + op.val, elem_ptr_ty);
        }
        else if (op.val != 0)
        {
            if (op.scale != 0 && (op.val % op.scale) == 0)
            {
                base = irb.CreatePointerCast(base, scale_type);
                base = irb.CreateGEP(base, irb.getInt64(op.val/op.scale));
            }
            else
            {
                base = irb.CreatePointerCast(base, irb.getInt8PtrTy());
                base = irb.CreateGEP(base, irb.getInt64(op.val));
            }
        }
    }
    else
    {
        base = OpAddrConst(op.val, elem_ptr_ty);
    }

    if (op.scale != 0)
    {
        // TODO: only do GEP in some sort of performance mode, it is unsafe.
        // A GEP with base "null" always resolves to "null". The base might only
        // later (during optimizations) be resolved to "null", causing the GEP
        // to be removed entirely. Note that this only happens, if the pointer
        // is solely constructed *from an scaled index* and therefore is very
        // unlikely to occur in compiler-generated code.

        bool use_mul = false;
        if (auto constval = llvm::dyn_cast<llvm::Constant>(base))
            use_mul = constval->isNullValue();

        llvm::Value* offset = GetReg(MapReg(op.ireg), Facet::I64);
        if (use_mul) {
            base = irb.CreateMul(offset, irb.getInt64(op.scale));
            base = irb.CreateIntToPtr(base, elem_ptr_ty);
        } else {
            base = irb.CreatePointerCast(base, scale_type);
            base = irb.CreateGEP(base, offset);
        }
    }

    return irb.CreatePointerCast(base, elem_ptr_ty);
}

static void
ll_operand_set_alignment(llvm::Instruction* value, Alignment alignment, bool sse = false)
{
    if (alignment == ALIGN_IMP)
        alignment = sse ? ALIGN_MAX : ALIGN_NONE;
    if (llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(value))
        load->setAlignment(alignment == ALIGN_NONE ? 1 : load->getPointerOperandType()->getPrimitiveSizeInBits() / 8);
    else if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(value))
        store->setAlignment(alignment == ALIGN_NONE ? 1 : store->getPointerOperandType()->getPrimitiveSizeInBits() / 8);
}

llvm::Value*
LifterBase::OpLoad(const LLInstrOp& op, Facet facet, Alignment alignment)
{
    facet = facet.Resolve(op.size * 8);
    if (op.type == LL_OP_IMM)
    {
        llvm::Type* type = facet.Type(irb.getContext());
        return llvm::ConstantInt::get(type, op.val);
    }
    else if (op.type == LL_OP_REG)
    {
        if (op.reg.IsGpHigh() && facet == Facet::I8)
            facet = Facet::I8H;
        return GetReg(MapReg(op.reg), facet);
    }
    else if (op.type == LL_OP_MEM)
    {
        llvm::Type* type = facet.Type(irb.getContext());
        llvm::Value* addr = OpAddr(op, type);
        llvm::LoadInst* result = irb.CreateLoad(type, addr);
        // FIXME: forward SSE information to increase alignment.
        ll_operand_set_alignment(result, alignment, false);
        return result;
    }

    assert(false);
    return nullptr;
}

void
LifterBase::OpStoreGp(const LLInstrOp& op, llvm::Value* value, Alignment alignment)
{
    if (op.type == LL_OP_MEM)
    {
        llvm::Value* addr = OpAddr(op, value->getType());
        llvm::StoreInst* store = irb.CreateStore(value, addr);
        ll_operand_set_alignment(store, alignment);
        return;
    }

    assert(op.type == LL_OP_REG && "gp-store to non-mem/non-reg");
    assert(op.reg.IsGp() || op.reg.rt == LL_RT_IP);
    assert(op.size == op.reg.Size());
    assert(value->getType() == irb.getIntNTy(op.size * 8));

    if (op.reg.rt == LL_RT_GP64 || op.reg.rt == LL_RT_IP)
    {
        SetReg(MapReg(op.reg), Facet::I64, value);
        return;
    }

    llvm::Value* value64 = irb.CreateZExt(value, irb.getInt64Ty());

    if (op.reg.rt == LL_RT_GP32)
    {
        SetReg(MapReg(op.reg), Facet::I64, value64);
        SetRegFacet(MapReg(op.reg), Facet::I32, value);
        return;
    }

    uint64_t mask;
    Facet store_facet;
    if (op.reg.IsGpHigh())
    {
        mask = 0xff00;
        store_facet = Facet::I8H;
        value64 = irb.CreateShl(value64, 8);
    }
    else if (op.size == 1)
    {
        mask = 0xff;
        store_facet = Facet::I8;
    }
    else if (op.size == 2)
    {
        mask = 0xffff;
        store_facet = Facet::I16;
    }
    else
    {
        assert(false);
    }

    llvm::Value* masked = irb.CreateAnd(GetReg(MapReg(op.reg), Facet::I64), ~mask);
    SetReg(MapReg(op.reg), Facet::I64, irb.CreateOr(value64, masked));
    SetRegFacet(MapReg(op.reg), store_facet, value);
}

void
LifterBase::OpStoreVec(const LLInstrOp& op, llvm::Value* value, bool avx,
                        Alignment alignment)
{
    if (op.type == LL_OP_MEM)
    {
        llvm::Value* addr = OpAddr(op, value->getType());
        llvm::StoreInst* store = irb.CreateStore(value, addr);
        ll_operand_set_alignment(store, alignment, !avx);
        return;
    }

    assert(op.type == LL_OP_REG && "vec-store to non-mem/non-reg");

    size_t operandWidth = value->getType()->getPrimitiveSizeInBits();
    // assert(operandWidth == Facet::Type(dataType, state->irb.getContext())->getPrimitiveSizeInBits());

    llvm::Type* iVec = irb.getIntNTy(LL_VECTOR_REGISTER_SIZE);
    llvm::Value* current = irb.getIntN(LL_VECTOR_REGISTER_SIZE, 0);
    if (!avx)
        current = GetReg(MapReg(op.reg), Facet::IVEC);

    llvm::Type* value_type = value->getType();
    if (value_type->isVectorTy())
    {
        llvm::Value* full_vec = value;
        if (operandWidth < LL_VECTOR_REGISTER_SIZE)
        {
            unsigned element_count = value_type->getVectorNumElements();
            unsigned total_count = element_count * LL_VECTOR_REGISTER_SIZE / operandWidth;
            llvm::Type* element_type = value_type->getVectorElementType();
            llvm::Type* full_type = llvm::VectorType::get(element_type, total_count);
            llvm::Value* current_vector = irb.CreateBitCast(current, full_type);

            // First, we enlarge the input vector to the full register length.
            llvm::SmallVector<uint32_t, 16> mask;
            for (unsigned i = 0; i < total_count; i++)
                mask.push_back(i < element_count ? i : element_count);
            full_vec = irb.CreateShuffleVector(value, llvm::Constant::getNullValue(value_type), mask);

            // Now shuffle the two vectors together
            for (unsigned i = 0; i < total_count; i++)
                mask[i] = i + (i < element_count ? 0 : total_count);
            full_vec = irb.CreateShuffleVector(full_vec, current_vector, mask);
        }

        SetReg(MapReg(op.reg), Facet::IVEC, irb.CreateBitCast(full_vec, iVec));
#if LL_VECTOR_REGISTER_SIZE >= 256
        // Induce some common facets via i128 for better SSE support
        if (operandWidth == 128)
        {
            llvm::Value* sse = irb.CreateBitCast(value, irb.getInt128Ty());
            SetRegFacet(MapReg(op.reg), Facet::I128, sse);
        }
#endif
    }
    else
    {
        unsigned total_count = LL_VECTOR_REGISTER_SIZE / operandWidth;
        llvm::Type* full_type = llvm::VectorType::get(value_type, total_count);
        llvm::Value* full_vector = irb.CreateBitCast(current, full_type);
        full_vector = irb.CreateInsertElement(full_vector, value, 0ul);
        SetReg(MapReg(op.reg), Facet::IVEC, irb.CreateBitCast(full_vector, iVec));

#if LL_VECTOR_REGISTER_SIZE >= 256
        // Induce some common facets via i128 for better SSE support
        llvm::Type* sse_type = llvm::VectorType::get(value_type, 128 / operandWidth);
        llvm::Value* sse_vector = irb.CreateBitCast(current128, sse_type);
        sse_vector = irb.CreateInsertElement(sse_vector, value, 0ul);
        llvm::Value* sse = irb.CreateBitCast(sse_vector, irb.getInt128Ty());
        SetRegFacet(MapReg(op.reg), Facet::I128, sse);
#endif
    }
}

void LifterBase::StackPush(llvm::Value* value) {
    llvm::Value* rsp = GetReg(X86Reg::GP(LL_RI_SP), Facet::PTR);
    rsp = irb.CreatePointerCast(rsp, value->getType()->getPointerTo());
    rsp = irb.CreateConstGEP1_64(rsp, -1);
    irb.CreateStore(value, rsp);

    llvm::Value* rsp_int = irb.CreatePtrToInt(rsp, irb.getInt64Ty());
    SetReg(X86Reg::GP(LL_RI_SP), Facet::I64, rsp_int);
    SetRegFacet(X86Reg::GP(LL_RI_SP), Facet::PTR, rsp);
}

llvm::Value* LifterBase::StackPop(const X86Reg sp_src_reg) {
    llvm::Value* rsp = GetReg(sp_src_reg, Facet::PTR);
    rsp = irb.CreatePointerCast(rsp, irb.getInt64Ty()->getPointerTo());

    llvm::Value* new_rsp = irb.CreateConstGEP1_64(rsp, 1);
    llvm::Value* new_rsp_int = irb.CreatePtrToInt(new_rsp, irb.getInt64Ty());
    SetReg(X86Reg::GP(LL_RI_SP), Facet::I64, new_rsp_int);
    SetRegFacet(X86Reg::GP(LL_RI_SP), Facet::PTR, new_rsp);

    return irb.CreateLoad(rsp);
}

} // namespace

/**
 * @}
 **/
