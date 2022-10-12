#include <stdint.h>

typedef enum ValueKind {
	VAL_EMPTY,
	VAL_TYPE,
	VAL_COMP,
	VAL_REF,
	VAL_N8, VAL_N16, VAL_N32, VAL_N64,
	VAL_I8, VAL_I16, VAL_I32, VAL_I64,
	VAL_R32, VAL_R64,
} ValueKind;

typedef struct Value Value;
ARRAY_STRUCT(Value)

typedef union ValueUnion {
	ValueArr comp;
	struct Object* ref;
	uint8_t n8;
	uint16_t n16;
	uint32_t n32;
	uint64_t n64;
	int8_t i8;
	int16_t i16;
	int32_t i32;
	int64_t i64;
	float r32;
	double r64;
} ValueUnion;

typedef struct Value {
	ValueKind kind;
	ValueUnion u;
} Value;

ARRAY_FUNCS(Value, value)

typedef struct Object {
	Str name;
	Value type;
	Value value;
} Object;

ARRAY(Object, object)

Value get_deep_value (Value val) {
	switch (val.kind) {
		case VAL_REF:
			/// TODO: Dereference if "val.u.ref->type" is a reference
			return get_deep_value(val.u.ref->value);
			break;
		default:
			return val; 
	}
}

Value add_values (Value lhs, Value rhs) {
	switch (lhs.kind) {
		case VAL_I32:
			return (Value) {
				.kind = VAL_I32,
				.u.i32 = lhs.u.i32 + rhs.u.i32,
			};
			break;
	}
}

Value interpret_ident(ObjectArr* stack, Expr* expr) {
	if (strcmp(expr->u.ident.value.buf, "Type") == 0) {
		return (Value) { .kind = VAL_TYPE };
	}
	// Search for object on stack
	for (int i = stack->len - 1; i >= 0; --i) {
		if (strcmp(expr->u.ident.value.buf, stack->buf[i].name.buf) == 0) {
			return (Value) {
				.kind = VAL_REF,
				.u.ref = stack->buf + i,
			};
		}
	}
	THROW(expr->file, expr->index, "object not defined", 0);
}

Value interpret_expr (ObjectArr* stack, Expr* expr) {
	switch (expr->kind) {
		case EX_EMPTY:
			return (Value) { .kind = VAL_EMPTY };
			break;
		case EX_IDENT:
			return interpret_ident(stack, expr);
		case EX_TEXT:
			/// TODO: Allocate text on heap
			break;
		case EX_NUM:
			return (Value) {
				.kind = VAL_I32,
				.u.i32 = atoi(expr->u.num.value.buf),
			};
		case EX_OP:
			ExprArr exprs = expr->u.op.exprs;
			switch (expr->u.op.kind) {
				case MEMBER: {
					break;
				}
				// case ADD: {
				// 	Value lhs = interpret_expr(stack, exprs.buf);
				// 	Value rhs = interpret_expr(stack, exprs.buf + 1);
				// 	return add_values(get_deep_value(lhs), get_deep_value(rhs));
				// }
				// case BLOCK:
				// case NEW_LINE: {
				// 	int stack_len = stack->len;
				// 	for (int i = 0; i < exprs.len; ++i) {
				// 		interpret_expr(stack, exprs.buf + i);
				// 	}
				// 	stack->len = stack_len;
				// 	/// FIX: return value
				// 	break;
				// }
				// case OBJ: {
				// 	Value rhs = interpret_expr(stack, exprs.buf + 1);
				// 	Object* obj = push_object_arr(stack, (Object) {
				// 		.name = exprs.buf[0].u.ident.value.buf,
				// 		.type = rhs,
				// 	});
				// 	return (Value) {
				// 		.kind = VAL_REF,
				// 		.u.ref = obj,
				// 	};
				// }
				// case ASSIGN: {
				// 	Value lhs = interpret_expr(stack, exprs.buf);
				// 	Value rhs = interpret_expr(stack, exprs.buf + 1);
				// 	lhs.u.ref->value = rhs;
				// 	return rhs;
				// }
				// case REF_ASSIGN: {
				// 	Value lhs = interpret_expr(stack, exprs.buf);
				// 	Value rhs = interpret_expr(stack, exprs.buf + 1);
				// 	/// TODO: Assert that lhs is a reference type
				// 	lhs.u.ref->value = rhs.u.ref->value;
				// 	return lhs;
				// }
				// case ADD_ASSIGN: {
				// 	Value lhs = interpret_expr(stack, exprs.buf);
				// 	Value rhs = interpret_expr(stack, exprs.buf + 1);
				// 	lhs.u.ref->value = add_values(get_deep_value(lhs), get_deep_value(rhs));
				// 	return lhs;
				// }
				// case ARG: {
				// 	Value lhs = get_deep_value(interpret_expr(stack, exprs.buf));
				// 	Value rhs = get_deep_value(interpret_expr(stack, exprs.buf + 1));
					
				// 	if (lhs.kind == VAL_TYPE) {
				// 		push_object_arr(stack, (Object) {
				// 			.name = ""
				// 		});
				// 	}
					
				// 	if (strcmp(exprs.buf[0].u.ident.value.buf, "allocInt") == 0) {
				// 		return (Value) {
				// 			.kind = VAL_I32,
				// 			.u.i32 = (size_t)malloc(sizeof(int)),
				// 		};
				// 	} else if (strcmp(exprs.buf[0].u.ident.value.buf, "printInt") == 0) {
				// 		printf("%i\n", rhs.u.i32);
				// 	}
				// 	/// FIX: return value
				// 	break;
				// }
				default:
					THROW(expr->file, expr->index, "operator not yet implemented", 0);
			}
			break;
	}
}

void interpret (Expr* expr) {
	ObjectArr stack = new_object_arr(32);
	interpret_expr(&stack, expr);
}