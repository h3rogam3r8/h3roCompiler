# H3ro Compiler

Hero is a small tensor language and the compiler that turns it into fast code.
It's built on MLIR and LLVM, and it targets three things: a CPU, an NVIDIA GPU,
and a made-up accelerator called Anvil that I also have to write a simulator for.

**Status:** Nothing works yet. Will update as I make improvements.

## Why

I want to understand how ML compilers actually work. MLIR dialects, LLVM
codegen, GPU kernels, and other important concepts like dataflow analysis and
register allocation.

The Anvil backend is the part I'm most interested in. CPUs and GPUs schedule
instructions for you. A systolic accelerator doesn't, so the compiler has to do
instruction selection, register allocation, and scheduling itself.

## What the language looks like

```hero
fn mlp(x:  tensor<[B, 768],    f16>,
       w1: tensor<[768, 3072], f16>, b1: tensor<[3072], f16>,
       w2: tensor<[3072, 768], f16>, b2: tensor<[768], f16>)
    -> tensor<[B, 768], f16>
{
    let h = matmul(x, w1) + b1;
    let a = gelu(h);
    matmul(a, w2) + b2
}
```

Shapes are part of the type, so mismatches are compile errors. `B` is symbolic,
which means one compiled function handles any batch size. The full spec goes in
`docs/spec/`.

## How it fits together

```
.hero file
    |
    v
lexer -> parser -> type and shape checking
    |
    v
`hero` dialect        graph level: fusion, layout, constant folding
    |
    v
linalg / affine       loop level: tiling, vectorization, memory planning
    |
    +--------------+--------------+
    v              v              v
  LLVM          NVVM/PTX        Anvil
  CPU           RTX 2060        simulator
```

ONNX and PyTorch models get imported at the `hero` dialect level later on, so
they share every optimization below that point.

## Roadmap

Rough order. Each phase will end with something I can actually run. It will be verifiable through a modular testing suite.

1. **Foundations** — language spec, CMake build, test harness, CI.
2. **Front end** — lexer, parser, error messages that point at the right token.
3. **Types and shapes** — dtype rules, broadcasting, symbolic shape inference.
4. **The `hero` dialect** — MLIR ops in TableGen, AST lowered to IR.
5. **Graph optimization** — fusion, layout assignment, a dataflow framework I
   write myself instead of importing.
6. **Down to loops** — linalg, tiling, bufferization, and memory planning by
   graph coloring, which is register allocation wearing a costume.
7. **CPU backend** — vectorize, JIT through LLVM, benchmark against OpenBLAS.
8. **Autotuning** — search over tile sizes and schedules, cache what wins.
9. **GPU backend** — PTX, shared memory tiling, tensor cores, benchmark against
   cuBLAS and Triton.
10. **Real models** — ONNX and StableHLO importers, a `torch.compile` backend,
    run GPT-2 small and ResNet-18.
11. **Anvil** — design the ISA, then write the backend, assembler, and
    cycle simulator for it.
12. **Extras** — int8 quantization, fused attention, autodiff.

## Not doing

No general-purpose language features. Single device only.

## Machines

I develop on an M2 Pro Mac (ARM NEON, no CUDA) and do GPU work on an RTX 2060
laptop through WSL2. The 2060 is Turing, so `mma.sync` tensor cores work but
`cp.async` doesn't — that one needs sm_80. GPU code is tested by running
FileCheck over the generated PTX, which doesn't need a GPU at all.

## License

MIT.