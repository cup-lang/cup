```cup
> comment
A Type $~ b $ This.arg, c $ Int
e $ D.None ~ f g + F'1.2'
if l = m, (), if not l = m, (), ()
loop o < p, (), q +~ r
a; ret "s u", a
y [ Z
```
# Naming Conventions
- objects - `lowerCase`
- types - `UpperCase`
# Operators
- `a . B` - member access (chaining)
---
- `@ A` - managed reference type
- `# A` - unmanaged reference type
- `? A` - optional type
- `A | B` - function type
- `A ! B` - error type
---
- `a [ B` - safe cast
- `a { B` - unsafe cast
- `A $ b` - object define
---
- `a + b` - addition
- `a - b` - subtraction
- `a * b` - multiplication
- `a / b` - division
- `a % b` - reminder
---
- `a = b` - equal (chaining)
- `a @= b` - reference equal (chaining)
- `a & b` - logical and (chaining)
- `a ^ b` - logical or (chaining)
- `a < b` - less (chaining)
- `a <= b` - less or equal (chaining)
---
Right-to-left
- `a : b` - argument
- `a ~ b` - value assign (chaining)
- `a $~ b` - object assign (chaining)
- `a @~ b` - reference assign (chaining)
- `a +~ b` - add assign (chaining)
- `a -~ b` - subtract assign (chaining)
- `a *~ b` - multiply assign (chaining)
- `a /~ b` - divide assign (chaining)
- `a %~ b` - reminder assign (chaining)
- `a, b` - block (chaining)
---
- `a ; b` - label
- `a \n b` - new-line block (chaining)
# Basic Types
- `Nat`, `N8`, `N16`, `N32`, `N64`
- `Int`, `I8`, `I16`, `I32`, `I64`
- `Real`, `R32`, `R64`
- `Char`, `C8`, `C16`, `C32`
- `Text`, `T8`, `T16`, `T32`
- `Logic`, `Logic.True`, `Logic.False`
# Expressions
- `Ident`
    - name - `Text`
- `Text`
    - value - `Text`
- `Number`
    - value - `Int/Real`
- `Operation`
    - exprs - `DArr Expr`
    - opKind - `TokenKind`
# Build Types
- `default` - all checks
- `-debug` - debug info
- `-fast` - no checks
- `-small` - no inline