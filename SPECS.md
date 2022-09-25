```cup
> comment
arg Type,
A:Type ~ b:arg, c:Int
e:D.None ~ f g + F '1.2'
if (l = m), (), if !(l = m), (), ()
loop o < p, (), q +~ r
a; ret "s u", a
```

# Naming Conventions
- objects - `lowerCase`
- types - `UpperCase`

# Operators
- 1st (chaining)
	- `a.b` - member access
- 2nd
	- `!a` - negation
	- `@A` - reference type
	- `?A` - optional type
	- `A$B` - error type
- 3rd (right-to-left)
	- `a b` - argument
	- `a: B` - object define
	- `a, b` - block (chaining)
- 4th
	- `a + b` - addition
	- `a - b` - subtraction
	- `a * b` - multiplication
	- `a / b` - division
	- `a % b` - reminder
- 5th (chaining)
	- `a < b` - less
	- `a <= b` - less or equal
- 6th (chaining)
	- `a = b` - equal
	- `a @= b` - reference equal
	- `a & b` - logical and
	- `a | b` - logical or
	- `a ^ b` - logical xor
- 7th (chaining, right-to-left)
	- `a ~ b` - value assign
	- `a @~ b` - reference assign
	- `a +~ b` - add assign
	- `a -~ b` - subtract assign
	- `a *~ b` - multiply assign
	- `a /~ b` - divide assign
	- `a %~ b` - reminder assign
- 8th
	- `a ; b` - label
	- `a # b` - argument
- 9th
	- `a \n b` - block (chaining)

# Basic Types
- `Type`, `Call`, `Expr`
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