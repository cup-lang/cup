/// TODO
// 0. add keywords 
// 'where'
// 'proc'
// 1. parse more globals
// a) use (make use have pub)
// use abc;
// b) mod
// mod foo {
//     use bar;
// }
// c) struct
// struct SomeStruct {
//     foo: i32,
//     foo: i32,
// }
// d) union
// union SomeUnion {
//     foo: i32,
//     foo: i32,
// }
// e) enum, option
// enum SomeEnum {
//     Option0,
//     Option1(
//         a: i32, 
//         b: f32,
//     ),
//     Option2,
// }
// f) impl
// g) fn_def, arg
// h) var_def
// 2. generics

prop SomeProp {
    fn add() -> i32, f64 {
        var a = 5;
        a += 5;
        ret 5, 6.1;
    }
}

/// array constructing [,,,]

/// basic types
// isize i8 i16 i32 i64, int = i32
// usize u8 u16 u32 u64, uint = u32
// bsize b8 b16 b32 b64, bool = b8
// f32 f64, float = f64
// c8 c16 c32, char = c32
// s8 s16 s32, str = s32

/// generics
// fn some_func<T, U, V>(arg: T) {}
// struct SomeStruct<T, U, V>
// enum SomeEnum<T, U, V>

/// pointers
// ptr<T>
// deref with @
// addr with $

/// switch
// switch number {
//     case -1..1 { println!("Some range!"); },
//     case 2 || 3 { println!("Two or three!"); },
//     case _ {  },
// }

/// for in
// for item in items {
//     print(item);
// }

/// do/while
// standalone "do" for a infinite loop

/// null

/// C constants

/// self, Self

/// unions

/// struct inititialization
// foo := v2 {
//    x = 1.0,
//    y = 1.0,
// };

/// inl keyword
// for vars
// for functions

/// conversion/casting
// into<T> trait for conversion (when SOMETHING is done only)
// functions named like types for (unsafe) casting

/// `labels`

/// rest
// is an arr<void> by default
// the type can be specified to be arr<something>

/// closures
// |x| { print(x); }

/// #tags
// with_values(value)
// key_pair(key = value)
// built in:
// #os("WIN")
// #def("foo", 10)
// #if("name") 
// #full // exhaustive switch
// #bind("printf")
// #gen("c_name")
// #req("stdlib.h")

/// '_'
// number literals readability
// switch default case
// unused and UNUSABLE field/variable (can be used multiple times as a field)

/// did you mean 'some command' ?

/// try compile with tcc, gcc, clang, msvc
// notify what compiler is used
// error when no compiler found

/// restrict expr to certain scopes
// struct, enum, fn - only globals
// rest only in functions

/// duplicate imports
/// 'type' keyword
/// don't allow keywords in paths
/// type aliases
/// raw string literals
/// greek question mark ;

/// operator presedence
// fn(), a.b, a[], a@, a$
// -a, !a, ~a
// *, /, %
// +, -
// <<, >>
// &
// ^
// |
// <, <=, >, >=
// ==, !=
// &&
// ||
// ?
// =, +=, -=, *=, /=, %=, <<=, >>=, &=, ^=, |=