
sources = [
  'basicblock.cc',
  'callconv.cc',
  'facet.cc',
  'instr.cc',
  'function.cc',
  'lldecoder.cc',
  'lifter.cc',
  'lifter-flags.cc',
  'lifter-gp.cc',
  'lifter-sse.cc',
  'lifter-operand.cc',
  'regfile.cc',
  'rellume.cc',
  'transforms.cc',
]

rellume_inc_priv = include_directories('.')
rellume_flags = [
  '-DLL_LLVM_MAJOR='+libllvm.version().split('.')[0],
  '-DRELLUME_ENABLE_CPP_HEADER',
  '-fvisibility=hidden',
  '-fno-exceptions',
  '-fno-unwind-tables',
  '-fno-rtti',
]
librellume_lib = library('rellume', sources, cpustruct_priv,
                         include_directories: [rellume_inc, rellume_inc_priv],
                         dependencies: [libllvm, fadec],
                         c_args: rellume_flags,
                         cpp_args: rellume_flags,
                         cpp_pch: 'pch/llvm.h',
                         install: true)
