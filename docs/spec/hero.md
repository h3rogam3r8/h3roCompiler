# The Hero Language

Inspiration of this spec comes from my other compiler project, Alpha. 

**Status:** Nothing implements this yet.

## 0. What this document is

A description of what a Hero program looks like and what it means. The compiler
gets written against this and the tests check the compiler against it.

So far, no control flow, no user-defined types, no I/O. It
is just barely enough to write a feed-forward network, because that is enough
to exercise every interesting part of the compiler.

## 1. Design goals

- **Shapes belong in the type system.** A shape mistake should be a compile
  error, not a wrong number at runtime.
- **No hidden work.** Nothing implicitly copies, converts, or reorders. If the
  compiler does something expensive, it's because the program said to.
- **Small enough to finish.** Every feature here has to earn its place by
  being needed for a real network.

## 2. Lexical structure

This section is about *tokens* — the words the compiler chops source text into
before it tries to understand any structure. Source is UTF-8. Whitespace
separates tokens and is otherwise ignored.

### Comments

```
// runs to the end of the line
/* block comment, does not nest */
```

### Identifiers

`[A-Za-z_][A-Za-z0-9_]*`, case-sensitive.

By convention, values and functions are lowercase (`x`, `matmul`) and symbolic
dimensions are uppercase (`B`, `SEQ`). The compiler does not enforce this.

### Keywords

`fn` `let` `cast` `true` `false` `tensor`, plus the dtype names `f32` `f16`
`bf16` `i32` `i8` `bool`.

Reserved but unused, so any future modification to this spec can claim them without breaking anything:
`return` `schedule` `kernel` `if` `else` `while` `for`.

### Literals

| Kind | Examples |
|---|---|
| Integer | `0`, `42`, `1024` |
| Float | `1.0`, `3.14`, `1e-5` |
| Bool | `true`, `false` |

There are no negative literals. `-3` is the unary minus operator applied to
`3`, which keeps the lexer from having to guess whether `a-3` is a subtraction.

A numeric literal has no fixed dtype on its own. It takes the dtype of whatever
it is combined with, so in `x * 2` the `2` becomes `f16` if `x` is. A literal
that never meets a typed operand defaults to `i32` if it's an integer and `f32`
if it has a decimal point or exponent.

### Operators and punctuation

`+` `-` `*` `/` `=` `;` `,` `:` `->` `(` `)` `{` `}` `[` `]` `<` `>`

`<` and `>` are also the brackets in `tensor<...>`. That would be ambiguous in a
language with comparison operators, which is one reason there are no comparison operators yet.

## 3. Types

### Scalars

`f32` `f16` `bf16` `i32` `i8` `bool`

### Tensors

```
tensor<[d0, d1, ...], dtype>
```

The number of dimensions is the tensor's **rank**. For now, rank 1 through 4.
Each dimension is either an integer literal (at least 1) or a symbolic
dimension.

```hero
tensor<[128, 768], f16>     // rank 2, both dims known
tensor<[B, 512], f32>       // rank 2, first dim symbolic
tensor<[64], i8>            // rank 1
```

### Symbolic dimensions

A dimension can be an identifier instead of a number. It stands for a value not
known at compile time, and every use of the same name inside one function must
be the same value at runtime.

```hero
fn add(a: tensor<[B, N], f32>, b: tensor<[B, N], f32>) -> tensor<[B, N], f32>
```

Both arguments must agree on both dimensions. The compiler proves what it can
and emits a runtime check for the rest.

The compiler will never assume two *different* symbols are equal. `B` and `N`
might happen to both be 512, but nothing in the program says so, so a rule that
needs them equal is a compile error. This is the correct behavior even though
it will occasionally annoy you.

### Dtype conversion

There is none, implicitly. `f16 + f32` is an error. Write `cast(x, f32)`.

Silent numeric conversion is where hard-to-find precision bugs come from, and
in a compiler the conversion is also a real operation with a real cost. Making
it visible in the source keeps both honest.

## 4. Program structure

A source file is a sequence of function definitions. There are no imports, no
globals, and no recursion.

```
fn name(param: Type, param: Type) -> Type { body }
```

A function body is zero or more `let` statements followed by exactly one
**result expression** with no semicolon on the end:

```hero
fn scale(x: tensor<[N], f32>) -> tensor<[N], f32> {
    let doubled = x * 2.0;
    doubled + 1.0
}
```

The semicolon is what distinguishes them. `let ... ;` is a statement; the last
line has no semicolon, so it's the value the function produces.

`let` bindings are immutable and their types are inferred. Rebinding a name
that already exists in scope is an error.

A function may call any function defined earlier in the same file.

## 5. Expressions

Operators, tightest binding first:

| Level | Operators | Associativity |
|---|---|---|
| 1 | `f(...)`, `(expr)` | — |
| 2 | unary `-` | right |
| 3 | `*` `/` | left |
| 4 | `+` `-` | left |

So `a + b * c` means `a + (b * c)`, and `a - b - c` means `(a - b) - c`.

All four binary operators are elementwise and broadcast.

## 6. Broadcasting

When a binary operator gets two tensors of different shapes, the shapes are
matched up from the **right**, and missing leading dimensions are treated as 1.
Two dimensions are compatible if they are equal, or if one of them is 1. The
result takes the larger of each pair.

| Left | Right | Result |
|---|---|---|
| `[4, 3]` | `[3]` | `[4, 3]` |
| `[4, 3]` | `[4, 1]` | `[4, 3]` |
| `[B, 768]` | `[768]` | `[B, 768]` |
| `[B, N]` | `[B, 1]` | `[B, N]` |
| `[4, 3]` | `[4]` | error — 3 and 4 are neither equal nor 1 |
| `[B, 4]` | `[N, 4]` | error — `B` and `N` can't be proven equal |

Both operands must have the same dtype.

## 7. Built-in operations

`axis` arguments must be integer literals, since the compiler needs them at
compile time. Negative axes aren't allowed yet.

| Operation | Shape rule | Notes |
|---|---|---|
| `matmul(a, b)` | `[M, K]` and `[K, N]` produce `[M, N]` | rank 2 only for now |
| `transpose(x)` | `[M, N]` produces `[N, M]` | rank 2 only |
| `sum(x, axis)` | `axis` becomes size 1 | rank is unchanged |
| `max(x, axis)` | `axis` becomes size 1 | rank is unchanged |
| `exp(x)` `sqrt(x)` `tanh(x)` | unchanged | float dtypes only |
| `relu(x)` `gelu(x)` | unchanged | float dtypes only |
| `cast(x, dtype)` | unchanged | the only way to change dtype |

Reductions keep the reduced axis at size 1 rather than removing it, so the
result still broadcasts against the input. That's what makes softmax read the
way it does in `examples/softmax.hero`.

`matmul` on `f16` or `bf16` inputs accumulates internally in `f32` and returns
the input dtype. Accumulating 16-bit sums in 16 bits loses accuracy fast, and
every piece of real hardware does it this way.

For `matmul`, the two `K` dimensions must match: both the same literal, or both
the same symbol.

## 8. Errors the compiler must report

These get error codes so tests can name them precisely. Helps when designing the test suite.

| Code | Condition |
|---|---|
| `E001` | unknown identifier |
| `E002` | name bound twice in the same scope |
| `E003` | unknown built-in or function |
| `E004` | wrong number of arguments |
| `E005` | dtype mismatch between operands |
| `E006` | shapes not broadcast-compatible |
| `E007` | rank not allowed for this operation |
| `E008` | `matmul` inner dimensions disagree |
| `E009` | function body has no result expression |
| `E010` | `axis` out of range or not an integer literal |

Every error must name a source line and column, and point at the operand that
caused it rather than the whole statement.

## 9. Not in the spec so far

Deliberately deferred, listed so it's clear they were considered and not
forgotten: control flow, batched `matmul`, convolution, `reshape`, indexing and
slicing, comparison and boolean operators, user-defined structs, multiple
return values, the `schedule` sublanguage, and autodiff.

## 10. Grammar

Written in **EBNF**, which is the standard shorthand for grammars. `::=` means
"is defined as", `|` separates alternatives, `*` means zero or more, `?` means
optional, and anything in quotes is literal text that must appear.

```ebnf
program      ::= function*
function     ::= "fn" ident "(" params? ")" "->" type block
params       ::= param ("," param)*
param        ::= ident ":" type

block        ::= "{" statement* expr "}"
statement    ::= "let" ident "=" expr ";"

type         ::= scalar_type | tensor_type
scalar_type  ::= "f32" | "f16" | "bf16" | "i32" | "i8" | "bool"
tensor_type  ::= "tensor" "<" "[" dims "]" "," scalar_type ">"
dims         ::= dim ("," dim)*
dim          ::= int_lit | ident

expr         ::= add_expr
add_expr     ::= mul_expr (("+" | "-") mul_expr)*
mul_expr     ::= unary_expr (("*" | "/") unary_expr)*
unary_expr   ::= "-" unary_expr | primary
primary      ::= call | ident | literal | "(" expr ")"
call         ::= ident "(" args? ")"
args         ::= expr ("," expr)*
literal      ::= int_lit | float_lit | "true" | "false"
```

The nesting of `add_expr` inside `mul_expr` inside `unary_expr` is how the
precedence table in section 5 gets encoded structurally: `+` can contain a `*`,
but not the other way around. The parser will end up with roughly one function
per rule here.