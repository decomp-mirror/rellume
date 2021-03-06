/**
 * This file is part of Rellume.
 *
 * (c) 2019, Alexis Engelke <alexis.engelke@googlemail.com>
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

DEF_IT(NOP,)
DEF_IT(RDSSP,)
// Intel MPX, behave as NOP on processors without support (SDM Vol 1, 17.4)
DEF_IT(BNDLDX,)
DEF_IT(BNDMOV,)
DEF_IT(BNDCU,)
DEF_IT(BNDCL,)
DEF_IT(BNDSTX,)
DEF_IT(BNDCN,)
DEF_IT(BNDMK,)

DEF_IT(PUSH, LiftPush(inst))
DEF_IT(PUSHFQ, LiftPushf(inst))
DEF_IT(POPFQ, LiftPopf(inst))
DEF_IT(POP, LiftPop(inst))
DEF_IT(LEAVE, LiftLeave(inst))
DEF_IT(CALL, LiftCall(inst))
DEF_IT(RET, LiftRet(inst))
DEF_IT(SYSCALL, goto not_implemented)
DEF_IT(CPUID, goto not_implemented)
DEF_IT(RDTSC, goto not_implemented)
DEF_IT(CRC32, goto not_implemented)
DEF_IT(UD2, LiftUnreachable(inst))

DEF_IT(LAHF, LiftLahf(inst))
DEF_IT(SAHF, LiftSahf(inst))

// Defined in llinstruction-gp.c
DEF_IT(MOV, LiftMovgp(inst, llvm::Instruction::SExt))
DEF_IT(MOVZX, LiftMovgp(inst, llvm::Instruction::ZExt))
DEF_IT(MOVSX, LiftMovgp(inst, llvm::Instruction::SExt))
// TODO: set non-temporal hint
DEF_IT(MOVNTI, LiftMovgp(inst, llvm::Instruction::SExt))
DEF_IT(MOVBE, LiftMovbe(inst))
DEF_IT(ADD, LiftAdd(inst))
DEF_IT(ADC, LiftAdc(inst))
DEF_IT(XADD, LiftXadd(inst))
DEF_IT(SUB, LiftSub(inst))
DEF_IT(SBB, LiftSbb(inst))
DEF_IT(CMP, LiftCmp(inst))
DEF_IT(CMPXCHG, LiftCmpxchg(inst))
DEF_IT(XCHG, LiftXchg(inst))
DEF_IT(LEA, LiftLea(inst))
DEF_IT(XLAT, LiftXlat(inst))
DEF_IT(NOT, LiftNot(inst))
DEF_IT(NEG, LiftNeg(inst))
DEF_IT(INC, LiftIncDec(inst))
DEF_IT(DEC, LiftIncDec(inst))
DEF_IT(AND, LiftAndOrXor(inst, llvm::Instruction::And))
DEF_IT(OR, LiftAndOrXor(inst, llvm::Instruction::Or))
DEF_IT(XOR, LiftAndOrXor(inst, llvm::Instruction::Xor))
DEF_IT(TEST, LiftAndOrXor(inst, llvm::Instruction::And, /*wb=*/false))
DEF_IT(IMUL, LiftMul(inst))
DEF_IT(MUL, LiftMul(inst))
DEF_IT(IDIV, LiftDiv(inst))
DEF_IT(DIV, LiftDiv(inst))
DEF_IT(SHL, LiftShift(inst, llvm::Instruction::Shl))
DEF_IT(SHR, LiftShift(inst, llvm::Instruction::LShr))
DEF_IT(SAR, LiftShift(inst, llvm::Instruction::AShr))
DEF_IT(ROL, LiftRotate(inst))
DEF_IT(ROR, LiftRotate(inst))
DEF_IT(SHLD, LiftShiftdouble(inst))
DEF_IT(SHRD, LiftShiftdouble(inst))
DEF_IT(BSF, LiftBitscan(inst, /*trailing=*/true))
DEF_IT(TZCNT, LiftBitscan(inst, /*trailing=*/true)) // TODO: support TZCNT
DEF_IT(BSR, LiftBitscan(inst, /*trailing=*/false))
DEF_IT(LZCNT, LiftBitscan(inst, /*trailing=*/false)) // TODO: support LZCNT
DEF_IT(BT, LiftBittest(inst))
DEF_IT(BTC, LiftBittest(inst))
DEF_IT(BTR, LiftBittest(inst))
DEF_IT(BTS, LiftBittest(inst))
DEF_IT(BSWAP, LiftBswap(inst))
DEF_IT(CEXT, LiftCext(inst))
DEF_IT(CSEP, LiftCsep(inst))

DEF_IT(CLC, LiftClc(inst))
DEF_IT(STC, LiftStc(inst))
DEF_IT(CMC, LiftCmc(inst))

DEF_IT(CLD, LiftCld(inst))
DEF_IT(STD, LiftStd(inst))
DEF_IT(LODS, LiftLods(inst))
DEF_IT(REP_LODS, LiftLods(inst))
DEF_IT(STOS, LiftStos(inst))
DEF_IT(REP_STOS, LiftStos(inst))
DEF_IT(MOVS, LiftMovs(inst))
DEF_IT(REP_MOVS, LiftMovs(inst))
DEF_IT(SCAS, LiftScas(inst))
DEF_IT(REPZ_SCAS, LiftScas(inst))
DEF_IT(REPNZ_SCAS, LiftScas(inst))
DEF_IT(CMPS, LiftCmps(inst))
DEF_IT(REPZ_CMPS, LiftCmps(inst))
DEF_IT(REPNZ_CMPS, LiftCmps(inst))

DEF_IT(CMOVO, LiftCmovcc(inst, Condition::O))
DEF_IT(CMOVNO, LiftCmovcc(inst, Condition::NO))
DEF_IT(CMOVC, LiftCmovcc(inst, Condition::C))
DEF_IT(CMOVNC, LiftCmovcc(inst, Condition::NC))
DEF_IT(CMOVZ, LiftCmovcc(inst, Condition::Z))
DEF_IT(CMOVNZ, LiftCmovcc(inst, Condition::NZ))
DEF_IT(CMOVBE, LiftCmovcc(inst, Condition::BE))
DEF_IT(CMOVA, LiftCmovcc(inst, Condition::A))
DEF_IT(CMOVS, LiftCmovcc(inst, Condition::S))
DEF_IT(CMOVNS, LiftCmovcc(inst, Condition::NS))
DEF_IT(CMOVP, LiftCmovcc(inst, Condition::P))
DEF_IT(CMOVNP, LiftCmovcc(inst, Condition::NP))
DEF_IT(CMOVL, LiftCmovcc(inst, Condition::L))
DEF_IT(CMOVGE, LiftCmovcc(inst, Condition::GE))
DEF_IT(CMOVLE, LiftCmovcc(inst, Condition::LE))
DEF_IT(CMOVG, LiftCmovcc(inst, Condition::G))

DEF_IT(SETO, LiftSetcc(inst, Condition::O))
DEF_IT(SETNO, LiftSetcc(inst, Condition::NO))
DEF_IT(SETC, LiftSetcc(inst, Condition::C))
DEF_IT(SETNC, LiftSetcc(inst, Condition::NC))
DEF_IT(SETZ, LiftSetcc(inst, Condition::Z))
DEF_IT(SETNZ, LiftSetcc(inst, Condition::NZ))
DEF_IT(SETBE, LiftSetcc(inst, Condition::BE))
DEF_IT(SETA, LiftSetcc(inst, Condition::A))
DEF_IT(SETS, LiftSetcc(inst, Condition::S))
DEF_IT(SETNS, LiftSetcc(inst, Condition::NS))
DEF_IT(SETP, LiftSetcc(inst, Condition::P))
DEF_IT(SETNP, LiftSetcc(inst, Condition::NP))
DEF_IT(SETL, LiftSetcc(inst, Condition::L))
DEF_IT(SETGE, LiftSetcc(inst, Condition::GE))
DEF_IT(SETLE, LiftSetcc(inst, Condition::LE))
DEF_IT(SETG, LiftSetcc(inst, Condition::G))

// Defined in llinstruction-sse.c
DEF_IT(LFENCE, LiftFence(inst))
DEF_IT(SFENCE, LiftFence(inst))
DEF_IT(MFENCE, LiftFence(inst))
DEF_IT(PREFETCHT0, LiftPrefetch(inst, 0, 3))
DEF_IT(PREFETCHT1, LiftPrefetch(inst, 0, 2))
DEF_IT(PREFETCHT2, LiftPrefetch(inst, 0, 1))
DEF_IT(PREFETCHNTA, LiftPrefetch(inst, 0, 0))
DEF_IT(PREFETCHW, LiftPrefetch(inst, 1, 1))
DEF_IT(FXSAVE, LiftFxsave(inst))
DEF_IT(FXRSTOR, LiftFxrstor(inst))
DEF_IT(FSTCW, LiftFstcw(inst))
DEF_IT(FLDCW, goto not_implemented)
DEF_IT(FSTSW, LiftFstsw(inst))
DEF_IT(STMXCSR, LiftStmxcsr(inst))
DEF_IT(LDMXCSR, goto not_implemented)
DEF_IT(MOVD, LiftSseMovq(inst, Facet::I32))
DEF_IT(MOVQ, LiftSseMovq(inst, Facet::I64))
DEF_IT(MOVSS, LiftSseMovScalar(inst, Facet::F32))
DEF_IT(MOVSD, LiftSseMovScalar(inst, Facet::F64))
DEF_IT(MOVUPS, LiftSseMovdq(inst, Facet::V4F32, ALIGN_NONE))
DEF_IT(MOVUPD, LiftSseMovdq(inst, Facet::V2F64, ALIGN_NONE))
DEF_IT(MOVAPS, LiftSseMovdq(inst, Facet::V4F32, ALIGN_MAX))
DEF_IT(MOVAPD, LiftSseMovdq(inst, Facet::V2F64, ALIGN_MAX))
DEF_IT(MOVDQU, LiftSseMovdq(inst, Facet::I128, ALIGN_NONE))
DEF_IT(MOVDQA, LiftSseMovdq(inst, Facet::I128, ALIGN_MAX))
DEF_IT(MOVNTPS, LiftSseMovntStore(inst, Facet::VF32))
DEF_IT(MOVNTPD, LiftSseMovntStore(inst, Facet::VF64))
DEF_IT(MOVNTDQ, LiftSseMovntStore(inst, Facet::VI64))
// TODO: set non-temporal hint
DEF_IT(MOVNTDQA, LiftSseMovdq(inst, Facet::I128, ALIGN_MAX))
DEF_IT(MOVLPS, LiftSseMovlp(inst))
DEF_IT(MOVLPD, LiftSseMovlp(inst))
DEF_IT(MOVHPS, LiftSseMovhps(inst))
DEF_IT(MOVHPD, LiftSseMovhpd(inst))
DEF_IT(PUNPCKLBW, LiftSseUnpck(inst, Facet::V16I8))
DEF_IT(PUNPCKLWD, LiftSseUnpck(inst, Facet::V8I16))
DEF_IT(PUNPCKLDQ, LiftSseUnpck(inst, Facet::V4I32))
DEF_IT(PUNPCKLQDQ, LiftSseUnpck(inst, Facet::V2I64))
DEF_IT(PUNPCKHBW, LiftSseUnpck(inst, Facet::V16I8))
DEF_IT(PUNPCKHWD, LiftSseUnpck(inst, Facet::V8I16))
DEF_IT(PUNPCKHDQ, LiftSseUnpck(inst, Facet::V4I32))
DEF_IT(PUNPCKHQDQ, LiftSseUnpck(inst, Facet::V2I64))
DEF_IT(UNPCKLPS, LiftSseUnpck(inst, Facet::V4F32))
DEF_IT(UNPCKLPD, LiftSseUnpck(inst, Facet::V2F64))
DEF_IT(UNPCKHPS, LiftSseUnpck(inst, Facet::V4F32))
DEF_IT(UNPCKHPD, LiftSseUnpck(inst, Facet::V2F64))
DEF_IT(SHUFPD, LiftSseShufpd(inst))
DEF_IT(SHUFPS, LiftSseShufps(inst))
DEF_IT(PSHUFD, LiftSsePshufd(inst))
DEF_IT(PSHUFLW, LiftSsePshufw(inst, 0))
DEF_IT(PSHUFHW, LiftSsePshufw(inst, 4))
DEF_IT(INSERTPS, LiftSseInsertps(inst))
DEF_IT(ADDSS, LiftSseBinOp(inst, llvm::Instruction::FAdd, Facet::F32))
DEF_IT(ADDSD, LiftSseBinOp(inst, llvm::Instruction::FAdd, Facet::F64))
DEF_IT(ADDPS, LiftSseBinOp(inst, llvm::Instruction::FAdd, Facet::VF32))
DEF_IT(ADDPD, LiftSseBinOp(inst, llvm::Instruction::FAdd, Facet::VF64))
DEF_IT(SUBSS, LiftSseBinOp(inst, llvm::Instruction::FSub, Facet::F32))
DEF_IT(SUBSD, LiftSseBinOp(inst, llvm::Instruction::FSub, Facet::F64))
DEF_IT(SUBPS, LiftSseBinOp(inst, llvm::Instruction::FSub, Facet::VF32))
DEF_IT(SUBPD, LiftSseBinOp(inst, llvm::Instruction::FSub, Facet::VF64))
DEF_IT(MULSS, LiftSseBinOp(inst, llvm::Instruction::FMul, Facet::F32))
DEF_IT(MULSD, LiftSseBinOp(inst, llvm::Instruction::FMul, Facet::F64))
DEF_IT(MULPS, LiftSseBinOp(inst, llvm::Instruction::FMul, Facet::VF32))
DEF_IT(MULPD, LiftSseBinOp(inst, llvm::Instruction::FMul, Facet::VF64))
DEF_IT(DIVSS, LiftSseBinOp(inst, llvm::Instruction::FDiv, Facet::F32))
DEF_IT(DIVSD, LiftSseBinOp(inst, llvm::Instruction::FDiv, Facet::F64))
DEF_IT(DIVPS, LiftSseBinOp(inst, llvm::Instruction::FDiv, Facet::VF32))
DEF_IT(DIVPD, LiftSseBinOp(inst, llvm::Instruction::FDiv, Facet::VF64))
DEF_IT(MINSS, LiftSseMinmax(inst, llvm::CmpInst::FCMP_OLT, Facet::F32))
DEF_IT(MINSD, LiftSseMinmax(inst, llvm::CmpInst::FCMP_OLT, Facet::F64))
DEF_IT(MINPS, LiftSseMinmax(inst, llvm::CmpInst::FCMP_OLT, Facet::VF32))
DEF_IT(MINPD, LiftSseMinmax(inst, llvm::CmpInst::FCMP_OLT, Facet::VF64))
DEF_IT(MAXSS, LiftSseMinmax(inst, llvm::CmpInst::FCMP_OGT, Facet::F32))
DEF_IT(MAXSD, LiftSseMinmax(inst, llvm::CmpInst::FCMP_OGT, Facet::F64))
DEF_IT(MAXPS, LiftSseMinmax(inst, llvm::CmpInst::FCMP_OGT, Facet::VF32))
DEF_IT(MAXPD, LiftSseMinmax(inst, llvm::CmpInst::FCMP_OGT, Facet::VF64))
DEF_IT(ORPS, LiftSseBinOp(inst, llvm::Instruction::Or, Facet::VI32))
DEF_IT(ORPD, LiftSseBinOp(inst, llvm::Instruction::Or, Facet::VI64))
DEF_IT(ANDPS, LiftSseBinOp(inst, llvm::Instruction::And, Facet::VI32))
DEF_IT(ANDPD, LiftSseBinOp(inst, llvm::Instruction::And, Facet::VI64))
DEF_IT(XORPS, LiftSseBinOp(inst, llvm::Instruction::Xor, Facet::VI32))
DEF_IT(XORPD, LiftSseBinOp(inst, llvm::Instruction::Xor, Facet::VI64))
DEF_IT(ANDNPS, LiftSseAndn(inst, Facet::VI32))
DEF_IT(ANDNPD, LiftSseAndn(inst, Facet::VI64))
DEF_IT(COMISS, LiftSseComis(inst, Facet::F32))
DEF_IT(COMISD, LiftSseComis(inst, Facet::F64))
DEF_IT(UCOMISS, LiftSseComis(inst, Facet::F32))
DEF_IT(UCOMISD, LiftSseComis(inst, Facet::F64))
DEF_IT(CMPSS, LiftSseCmp(inst, Facet::F32))
DEF_IT(CMPSD, LiftSseCmp(inst, Facet::F64))
DEF_IT(CMPPS, LiftSseCmp(inst, Facet::VF32))
DEF_IT(CMPPD, LiftSseCmp(inst, Facet::VF64))
DEF_IT(SQRTSS, LiftSseSqrt(inst, Facet::F32))
DEF_IT(SQRTSD, LiftSseSqrt(inst, Facet::F64))
DEF_IT(SQRTPS, LiftSseSqrt(inst, Facet::VF32))
DEF_IT(SQRTPD, LiftSseSqrt(inst, Facet::VF64))
DEF_IT(CVTDQ2PD, LiftSseCvt(inst, Facet::V2I32, Facet::V2F64))
DEF_IT(CVTDQ2PS, LiftSseCvt(inst, Facet::V4I32, Facet::V4F32))
DEF_IT(CVTPD2DQ, goto not_implemented) // non-truncating, same types as below
DEF_IT(CVTTPD2DQ, LiftSseCvt(inst, Facet::V2F64, Facet::V2I32))
DEF_IT(CVTPS2DQ, goto not_implemented) // non-truncating, same types as below
DEF_IT(CVTTPS2DQ, LiftSseCvt(inst, Facet::V4F32, Facet::V4I32))
DEF_IT(CVTPD2PS, LiftSseCvt(inst, Facet::V2F64, Facet::V2F32))
DEF_IT(CVTPS2PD, LiftSseCvt(inst, Facet::V2F32, Facet::V2F64))
DEF_IT(CVTSD2SS, LiftSseCvt(inst, Facet::F64, Facet::F32))
DEF_IT(CVTSS2SD, LiftSseCvt(inst, Facet::F32, Facet::F64))
DEF_IT(CVTSD2SI, goto not_implemented) // non-truncating, same types as below
DEF_IT(CVTTSD2SI, LiftSseCvt(inst, Facet::F64, Facet::I))
DEF_IT(CVTSS2SI, goto not_implemented) // non-truncating, same types as below
DEF_IT(CVTTSS2SI, LiftSseCvt(inst, Facet::F32, Facet::I))
DEF_IT(CVTSI2SD, LiftSseCvt(inst, Facet::I, Facet::F64))
DEF_IT(CVTSI2SS, LiftSseCvt(inst, Facet::I, Facet::F32))

DEF_IT(PXOR, LiftSseBinOp(inst, llvm::Instruction::Xor, Facet::VI64))
DEF_IT(POR, LiftSseBinOp(inst, llvm::Instruction::Or, Facet::VI64))
DEF_IT(PAND, LiftSseBinOp(inst, llvm::Instruction::And, Facet::VI64))
DEF_IT(PANDN, LiftSseAndn(inst, Facet::VI64))
DEF_IT(PADDB, LiftSseBinOp(inst, llvm::Instruction::Add, Facet::V16I8))
DEF_IT(PADDW, LiftSseBinOp(inst, llvm::Instruction::Add, Facet::V8I16))
DEF_IT(PADDD, LiftSseBinOp(inst, llvm::Instruction::Add, Facet::V4I32))
DEF_IT(PADDQ, LiftSseBinOp(inst, llvm::Instruction::Add, Facet::V2I64))
DEF_IT(PSUBB, LiftSseBinOp(inst, llvm::Instruction::Sub, Facet::V16I8))
DEF_IT(PSUBW, LiftSseBinOp(inst, llvm::Instruction::Sub, Facet::V8I16))
DEF_IT(PSUBD, LiftSseBinOp(inst, llvm::Instruction::Sub, Facet::V4I32))
DEF_IT(PSUBQ, LiftSseBinOp(inst, llvm::Instruction::Sub, Facet::V2I64))
DEF_IT(PADDSB, LiftSsePaddsubSaturate(inst, llvm::Instruction::Add, /*sign=*/true, Facet::V16I8))
DEF_IT(PADDSW, LiftSsePaddsubSaturate(inst, llvm::Instruction::Add, /*sign=*/true, Facet::V8I16))
DEF_IT(PADDUSB, LiftSsePaddsubSaturate(inst, llvm::Instruction::Add, /*sign=*/false, Facet::V16I8))
DEF_IT(PADDUSW, LiftSsePaddsubSaturate(inst, llvm::Instruction::Add, /*sign=*/false, Facet::V8I16))
DEF_IT(PSUBSB, LiftSsePaddsubSaturate(inst, llvm::Instruction::Sub, /*sign=*/true, Facet::V16I8))
DEF_IT(PSUBSW, LiftSsePaddsubSaturate(inst, llvm::Instruction::Sub, /*sign=*/true, Facet::V8I16))
DEF_IT(PSUBUSB, LiftSsePaddsubSaturate(inst, llvm::Instruction::Sub, /*sign=*/false, Facet::V16I8))
DEF_IT(PSUBUSW, LiftSsePaddsubSaturate(inst, llvm::Instruction::Sub, /*sign=*/false, Facet::V8I16))
DEF_IT(PMULLW, LiftSseBinOp(inst, llvm::Instruction::Mul, Facet::V8I16))
DEF_IT(PMULLD, LiftSseBinOp(inst, llvm::Instruction::Mul, Facet::V4I32))
DEF_IT(PSLLW, LiftSsePshiftElement(inst, llvm::Instruction::Shl, Facet::VI16))
DEF_IT(PSLLD, LiftSsePshiftElement(inst, llvm::Instruction::Shl, Facet::VI32))
DEF_IT(PSLLQ, LiftSsePshiftElement(inst, llvm::Instruction::Shl, Facet::VI64))
DEF_IT(PSRLW, LiftSsePshiftElement(inst, llvm::Instruction::LShr, Facet::VI16))
DEF_IT(PSRLD, LiftSsePshiftElement(inst, llvm::Instruction::LShr, Facet::VI32))
DEF_IT(PSRLQ, LiftSsePshiftElement(inst, llvm::Instruction::LShr, Facet::VI64))
DEF_IT(PSRAW, LiftSsePshiftElement(inst, llvm::Instruction::AShr, Facet::VI16))
DEF_IT(PSRAD, LiftSsePshiftElement(inst, llvm::Instruction::AShr, Facet::VI32))
DEF_IT(PSLLDQ, LiftSsePshiftBytes(inst))
DEF_IT(PSRLDQ, LiftSsePshiftBytes(inst))
DEF_IT(PACKSSWB, LiftSsePack(inst, Facet::VI16, /*sign=*/true))
DEF_IT(PACKSSDW, LiftSsePack(inst, Facet::VI32, /*sign=*/true))
DEF_IT(PACKUSWB, LiftSsePack(inst, Facet::VI16, /*sign=*/false))
DEF_IT(PACKUSDW, LiftSsePack(inst, Facet::VI32, /*sign=*/false))
DEF_IT(PINSRB, LiftSsePinsr(inst, Facet::VI8, Facet::I8, 0x0f))
DEF_IT(PINSRW, LiftSsePinsr(inst, Facet::VI16, Facet::I16, 0x07))
DEF_IT(PINSRD, LiftSsePinsr(inst, Facet::VI32, Facet::I32, 0x03))
DEF_IT(PINSRQ, LiftSsePinsr(inst, Facet::VI64, Facet::I64, 0x01))
DEF_IT(PEXTRB, LiftSsePextr(inst, Facet::VI8, 0x0f))
DEF_IT(PEXTRW, LiftSsePextr(inst, Facet::VI16, 0x07))
DEF_IT(PEXTRD, LiftSsePextr(inst, Facet::VI32, 0x03))
DEF_IT(PEXTRQ, LiftSsePextr(inst, Facet::VI64, 0x01))
DEF_IT(PAVGB, LiftSsePavg(inst, Facet::VI8))
DEF_IT(PAVGW, LiftSsePavg(inst, Facet::VI16))
DEF_IT(PMULHW, LiftSsePmulhw(inst, llvm::Instruction::SExt))
DEF_IT(PMULHUW, LiftSsePmulhw(inst, llvm::Instruction::ZExt))
DEF_IT(PCMPEQB, LiftSsePcmp(inst, llvm::CmpInst::ICMP_EQ, Facet::VI8))
DEF_IT(PCMPEQW, LiftSsePcmp(inst, llvm::CmpInst::ICMP_EQ, Facet::VI16))
DEF_IT(PCMPEQD, LiftSsePcmp(inst, llvm::CmpInst::ICMP_EQ, Facet::VI32))
DEF_IT(PCMPEQQ, LiftSsePcmp(inst, llvm::CmpInst::ICMP_EQ, Facet::VI64))
DEF_IT(PCMPGTB, LiftSsePcmp(inst, llvm::CmpInst::ICMP_SGT, Facet::VI8))
DEF_IT(PCMPGTW, LiftSsePcmp(inst, llvm::CmpInst::ICMP_SGT, Facet::VI16))
DEF_IT(PCMPGTD, LiftSsePcmp(inst, llvm::CmpInst::ICMP_SGT, Facet::VI32))
DEF_IT(PCMPGTQ, LiftSsePcmp(inst, llvm::CmpInst::ICMP_SGT, Facet::VI64))
DEF_IT(PMINUB, LiftSsePminmax(inst, llvm::CmpInst::ICMP_ULT, Facet::VI8))
DEF_IT(PMINUW, LiftSsePminmax(inst, llvm::CmpInst::ICMP_ULT, Facet::VI16))
DEF_IT(PMINUD, LiftSsePminmax(inst, llvm::CmpInst::ICMP_ULT, Facet::VI32))
DEF_IT(PMINSB, LiftSsePminmax(inst, llvm::CmpInst::ICMP_SLT, Facet::VI8))
DEF_IT(PMINSW, LiftSsePminmax(inst, llvm::CmpInst::ICMP_SLT, Facet::VI16))
DEF_IT(PMINSD, LiftSsePminmax(inst, llvm::CmpInst::ICMP_SLT, Facet::VI32))
DEF_IT(PMAXUB, LiftSsePminmax(inst, llvm::CmpInst::ICMP_UGT, Facet::VI8))
DEF_IT(PMAXUW, LiftSsePminmax(inst, llvm::CmpInst::ICMP_UGT, Facet::VI16))
DEF_IT(PMAXUD, LiftSsePminmax(inst, llvm::CmpInst::ICMP_UGT, Facet::VI32))
DEF_IT(PMAXSB, LiftSsePminmax(inst, llvm::CmpInst::ICMP_SGT, Facet::VI8))
DEF_IT(PMAXSW, LiftSsePminmax(inst, llvm::CmpInst::ICMP_SGT, Facet::VI16))
DEF_IT(PMAXSD, LiftSsePminmax(inst, llvm::CmpInst::ICMP_SGT, Facet::VI32))
DEF_IT(PMOVMSKB, LiftSseMovmsk(inst, Facet::VI8))
DEF_IT(MOVMSKPS, LiftSseMovmsk(inst, Facet::VI32))
DEF_IT(MOVMSKPD, LiftSseMovmsk(inst, Facet::VI64))

// Jumps are handled in the basic block generation code.
DEF_IT(JMP, LiftJmp(inst))
DEF_IT(JO, LiftJcc(inst, Condition::O))
DEF_IT(JNO, LiftJcc(inst, Condition::NO))
DEF_IT(JC, LiftJcc(inst, Condition::C))
DEF_IT(JNC, LiftJcc(inst, Condition::NC))
DEF_IT(JZ, LiftJcc(inst, Condition::Z))
DEF_IT(JNZ, LiftJcc(inst, Condition::NZ))
DEF_IT(JBE, LiftJcc(inst, Condition::BE))
DEF_IT(JA, LiftJcc(inst, Condition::A))
DEF_IT(JS, LiftJcc(inst, Condition::S))
DEF_IT(JNS, LiftJcc(inst, Condition::NS))
DEF_IT(JP, LiftJcc(inst, Condition::P))
DEF_IT(JNP, LiftJcc(inst, Condition::NP))
DEF_IT(JL, LiftJcc(inst, Condition::L))
DEF_IT(JGE, LiftJcc(inst, Condition::GE))
DEF_IT(JLE, LiftJcc(inst, Condition::LE))
DEF_IT(JG, LiftJcc(inst, Condition::G))
DEF_IT(JCXZ, LiftJcxz(inst))
DEF_IT(LOOP, LiftLoop(inst))
DEF_IT(LOOPE, LiftLoop(inst))
DEF_IT(LOOPNE, LiftLoop(inst))
