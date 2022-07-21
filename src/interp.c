#include <stdint.h>

typedef enum ValueKind {
	VAL_COMP,
	VAL_REF,
	VAL_N8,
	VAL_N16,
	VAL_N32,
	VAL_N64,
	VAL_I8,
	VAL_I16,
	VAL_I32,
	VAL_I64,
	VAL_R32,
	VAL_R64,
} ValueKind;

typedef struct Object Object;

typedef struct ObjectArr {
	Object* buf;
	int len;
	int cap;
} ObjectArr;

typedef union ValueUnion {
	/// TODO: Make this a ValueArr
	ObjectArr comp;
	Object* ref;
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

typedef struct Object {
	Str name;
	Value type;
	Value value;
} Object;

ObjectArr new_object_arr (int cap) {
	return (ObjectArr){
		.buf = malloc(cap * sizeof(Object)),
		.len = 0,
		.cap = cap
	};
}

void try_resize_object_arr (ObjectArr* stack) {
	if (stack->len >= stack->cap) {
		stack->cap *= 2;
		stack->buf = realloc(stack->buf, sizeof(Object) * stack->cap);
	}
}

Object* push_object_arr (ObjectArr* arr, Object obj) {
	arr->len++;
	try_resize_object_arr(arr);
	arr->buf[arr->len - 1] = obj;
	return arr->buf + arr->len - 1;
}

Object pop_object_arr (ObjectArr* arr) {
	if (arr->len == 0) {
		printf("Can't pop from empty array!");
		exit(1);
	}
	arr->len--;
	return arr->buf[arr->len];
}

Value get_deep_value (Value val) {
	switch (val.kind) {
		case VAL_REF:
			return get_deep_value(val.u.ref->value);
			break;
		default:
			return val; 
	}
}

Value add_values (Value lhs, Value rhs) {
	switch (lhs.kind) {
		case VAL_I8:
			return (Value) {
				.kind = VAL_I8,
				.u.i8 = lhs.u.i8 + rhs.u.i8,
			};
			break;
	}
}

Value interpret_expr (File file, ObjectArr* stack, Expr* expr) {
	switch (expr->kind) {
		case EX_EMPTY:
			break;
		case EX_IDENT:
			for (int i = stack->len - 1; i >= 0; --i) {
				if (strcmp(expr->u.ident.value.buf, stack->buf[i].name.buf) == 0) {
					return (Value) {
						.kind = VAL_REF,
						.u.ref = stack->buf + i,
					};
				}
			}
			break;
		case EX_TEXT:
			break;
		case EX_NUM:
			/// TODO: parse numbers
			return (Value) {
				.kind = VAL_I8,
				.u.i8 = atoi(expr->u.num.value.buf),
			};
		case EX_OP:
			ExprArr exprs = expr->u.op.exprs;
			switch (expr->u.op.kind) {
				case ADD: {
					Value lhs = interpret_expr(file, stack, exprs.buf);
					Value rhs = interpret_expr(file, stack, exprs.buf + 1);
					return add_values(get_deep_value(lhs), get_deep_value(rhs));
				}
				case BLOCK:
				case NEW_LINE:
					int stack_len = stack->len;
					for (int i = 0; i < exprs.len; ++i) {
						interpret_expr(file, stack, exprs.buf + i);
					}
					stack->len = stack_len;
					break;
				case OBJ:
					Object* obj = push_object_arr(stack, (Object) {
						.name = exprs.buf[0].u.ident.value.buf,
						.type = {},
						.value = {},
					});
					return (Value) {
						.kind = VAL_REF,
						.u.ref = obj,
					};
				case ASSIGN:
					Value lhs = interpret_expr(file, stack, exprs.buf);
					Value rhs = interpret_expr(file, stack, exprs.buf + 1);
					lhs.u.ref->value = rhs;
					return rhs;
				case ADD_ASSIGN: {
					Value lhs = interpret_expr(file, stack, exprs.buf);
					Value rhs = interpret_expr(file, stack, exprs.buf + 1);
					lhs.u.ref->value = add_values(get_deep_value(lhs), get_deep_value(rhs));
					return lhs;
				}
				case ARG: {
					Value rhs = interpret_expr(file, stack, exprs.buf + 1);
					if (strcmp(exprs.buf[0].u.ident.value.buf, "printInt") == 0) {
						printf("%i\n", rhs.u.i8);
					}
					return (Value) {
						.kind = VAL_COMP,
						.u.comp.len = 0,
					};
				}
				default:
					THROW(file, 0, "operator not yet implemented", 0);
			}
			break;
	}
}

void interpret (File file, Expr* expr) {
	ObjectArr stack = new_object_arr(32);
	interpret_expr(file, &stack, expr);
}