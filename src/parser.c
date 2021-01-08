/*
    literal = string_lit + arr_lit + num_lit + bool_lit + char_lit + null_lit + self_lit + this_lit + type_lit
    value = op + fn_call + var_use + literal
    local = if + elif + else + do + while + for + match + back + next + jump + delay + local_var_def + value
    global = use + mod + struct + enum + union + prop + def + sub_def + var_def

    tag:
        - name: string
        - data: arr<literal>

    type:
        - const: bool
        - name: string
        - child: arr<type>

    constr_type:
        - name: string
        - constr: arr<type>

    (pub) mod:
        - pub: bool 
        - name: string
        - body: arr<global>

    use:
        - name: string

    (pub) (generic) struct:
        - pub: bool
        - name: string
        - gen: arr<constr_type>
        - body: arr<field>
    
    (pub) field:
        - pub: bool 
        - name: string
        - type: type

    (pub) (generic) enum:
        - pub: bool 
        - name: string
        - gen: arr<constr_type>
        - body: arr<option>

    (pub) option:
        - pub: bool 
        - name: string
        - body: arr<field>

    (pub) (generic) union:
        - pub: bool 
        - name: string
        - gen: arr<constr_type>
        - body: arr<field>

    (pub) (generic) prop:
        - pub: bool
        - name: string
        - gen: arr<constr_type>
        - body: arr<fn_def>

    (pub) (generic) impl:
        - pub: bool
        - gen: arr<constr_type>
        - prop: type
        - target: type
        - body: arr<fn_def>

    (pub) (inl) (macro) (generic) fn_def:
        - pub: bool
        - inl: bool
        - macro: bool
        - name: string
        - gen: arr<constr_type>
        - args: arr<arg>
        - ret_type: type
        - body: arr<local>

    (rest) arg:
        rest: bool
        name: string
        type: type

    (pub) (inl) (combo) var_def:
        - pub: bool
        - inl: bool
        - name: string
        - type: type 
        - value: value

    (combo) local_var_def:
        - name: string
        - type: type
        - value: value

    fn_call:
        - type: type
        - args: arr<value>

    var_use:
        - name: string

    struct_inst:
        - type: type
        - fields: arr<field_val>

    field_val:
        - name: string
        - value: value

    do:
    block:
        - body: arr<local>

    string_lit:
    arr_lit:
        - value: arr<value>

    num_lit:
        - value: string

    bool_lit:
        - value: bool

    char_lit:
        - value: char

    null_lit:
    self_lit:

    if:
    elif:
    else:
    while:
        - con: value
        - body: arr<local>

    for:
        - loop_var: string
        - range: value

    switch:
        - value: value
        - body: arr<case>

    case:
        - value: value
        - body: arr<local>

    fall:

    break:
    next:
    goto:
        - label: string

    return:
        - value: value

    defer:
        - body: arr<local>

    cond_op:
        - con: value
        - lhs: value
        - rhs: value

    range_op:
    equal_op:
    not_equal_op:
    and_op:
    or_op:
    less_op:
    less_equal_op:
    greater_op:
    greater_equal_op:
    add_op:
    sub_op:
    mul_op:
    div_op:
    mod_op:
    bit_and:
    bit_or:
    bit_xor:
    left_shift:
    right_shift:
        - lhs: value
        - rhs: value

    deref_op:
    address_op:
    not_op:
    bit_not:
        - val: value

    assign_op:
    add_assign_op:
    sub_assign_op:
    mul_assign_op:
    div_assign_op:
    mod_assign_op:
    bit_and_assign:
    bit_or_assign:
    bit_xor_assign:
    left_shift_assign:
    right_shift_assign:
        - lhs: var_use
        - rhs: value
*/

typedef struct Expr Expr;

VECTOR_STRUCT(ExprVector, Expr);

typedef enum
{
    E_TAG,
    E_TYPE,
    E_CONSTR_TYPE,
    E_TAG_DEF,
    E_MOD,
    E_USE,
    E_STRUCT,
    E_UNION,
    E_FIELD,
    E_ENUM,
    E_OPTION,
    E_OPTION_FIELD,
    E_PROP,
    E_DEF,
    E_SUB_DEF,
    E_ARG,
    E_VAR_DEF,
    E_LOCAL_VAR_DEF,
    E_SUB_CALL,
    E_VAR_USE,
    E_STRUCT_INST,
    E_FIELD_VAL,
    E_STRING_LIT,
    E_CHAR_LIT,
    E_ARR_LIT,
    E_INT_LIT,
    E_FLOAT_LIT,
    E_BOOL_LIT,
    E_NULL_LIT,
    E_THIS_LIT,
    E_TYPE_LIT,
    E_WHERE,
    E_DO,
    E_BLOCK,
    E_AS,
    E_IF,
    E_ELIF,
    E_ELSE,
    E_WHILE,
    E_FOR,
    E_MATCH,
    E_CASE,
    E_BACK,
    E_NEXT,
    E_DELAY,
    E_JUMP,
    E_COND_OP,
    E_RANGE_OP,
    E_RANGE_INCL_OP,
    E_EQUAL_OP,
    E_NOT_EQUAL_OP,
    E_AND_OP,
    E_OR_OP,
    E_LESS_OP,
    E_LESS_EQUAL_OP,
    E_GREATER_OP,
    E_GREATER_EQUAL_OP,
    E_ADD_OP,
    E_SUBTRACT_OP,
    E_MUL_OP,
    E_DIV_OP,
    E_MOD_OP,
    E_BIT_AND,
    E_BIT_OR,
    E_BIT_XOR,
    E_LEFT_SHIFT,
    E_RIGHT_SHIFT,
    E_DEREF_OP,
    E_ADDRESS_OP,
    E_NEGATION_OP,
    E_NOT_OP,
    E_BIT_NOT_OP,
    E_ASSIGN_OP,
    E_ADD_ASSIGN_OP,
    E_SUBTRACT_ASSIGN_OP,
    E_MUL_ASSIGN_OP,
    E_DIV_ASSIGN_OP,
    E_MOD_ASSIGN_OP,
    E_BIT_AND_ASSIGN,
    E_BIT_OR_ASSIGN,
    E_BIT_XOR_ASSIGN,
    E_LEFT_SHIFT_ASSIGN,
    E_RIGHT_SHIFT_ASSIGN
} ExprKind;

typedef struct
{
    char *name;
    ExprVector args;
} Tag;

typedef struct
{
    char _const;
    char *name;
    ExprVector children;
} Type;

typedef struct
{
    char *name;
    ExprVector constr;
} ConstrType;

typedef struct
{
    char pub;
    char *name;
    ExprVector args;
} TagDef;

typedef struct
{
    char pub;
    char *name;
    ExprVector body;
} Mod;

typedef struct
{
    char *name;
} Use;

typedef struct
{
    char pub;
    char *name;
    ExprVector body;
} Struct;

typedef struct
{
    char pub;
    char *name;
    Expr *type;
} Field;

typedef struct
{
    char pub;
    char *name;
    ExprVector body;
} Enum;

typedef struct
{
    char *name;
    ExprVector body;
} Option;

typedef struct
{
    char *name;
    Expr *type;
} OptionField;

typedef struct
{
    char pub;
    char *name;
    ExprVector body;
} Prop;

typedef struct
{
    char pub;
    Expr *prop;
    Expr *target;
    ExprVector body;
} Def;

typedef struct
{
    char pub;
    char inl;
    char macro;
    char *name;
    ExprVector args;
    Expr *ret_type;
    ExprVector body;
} SubDef;

typedef struct
{
    char rest;
    char *name;
    Expr *type;
} Arg;

typedef struct
{
    char pub;
    char inl;
    char *name;
    Expr *type;
    Expr *value;
} VarDef;

typedef struct
{
    char *name;
    Expr *type;
    Expr *value;
} LocalVarDef;

typedef struct
{
    Expr *type;
    ExprVector args;
} SubCall;

typedef struct
{
    char *name;
} VarUse;

typedef struct
{
    Expr *type;
    ExprVector fields;
} StructInst;

typedef struct
{
    char *name;
    Expr *value;
} FieldVal;

typedef struct
{
    char *value;
} StringLit;

typedef struct
{
    ExprVector value;
} ArrLit;

typedef struct
{
    char value;
} BoolLit;

typedef struct
{
    Expr *con;
    ExprVector body;
} If;

typedef struct
{
    Expr *con;
    ExprVector body;
} Elif;

typedef struct
{
    Expr *con;
    ExprVector body;
} Else;

typedef struct
{
    ExprVector body;
} Do;

typedef struct
{
    ExprVector body;
} Block;

typedef struct
{
    Expr *con;
    ExprVector body;
} While;

typedef struct
{
    char *loop_var;
    Expr *range;
} For;

typedef struct
{
    Expr *value;
    ExprVector body;
} Match;

typedef struct
{
    Expr *value;
    ExprVector body;
} Case;

typedef struct
{
    char *label;
    Expr *value;
} Back;

typedef struct
{
    char *label;
} Next;

typedef struct
{
    ExprVector body;
} Delay;

typedef struct
{
    char *label;
} Jump;

typedef struct
{
    Expr *con;
    Expr *lhs;
    Expr *rhs;
} CondOp;

typedef struct
{
    Expr *lhs;
    Expr *rhs;
} BinaryOp;

typedef struct
{
    Expr *val;
} UnaryOp;

typedef union
{
    Tag tag;
    Type type;
    ConstrType constr_type;
    TagDef tag_def;
    Mod mod;
    Use use;
    Struct _struct;
    Struct _union;
    Field field;
    Enum _enum;
    Option option;
    OptionField option_field;
    Prop prop;
    Def def;
    SubDef sub_def;
    Arg arg;
    VarDef var_def;
    LocalVarDef local_var_def;
    SubCall sub_call;
    VarUse var_use;
    StructInst struct_inst;
    FieldVal field_val;
    Do _do;
    Block block;
    StringLit string_lit;
    StringLit char_lit;
    StringLit int_lit;
    StringLit float_lit;
    ArrLit arr_lit;
    BoolLit bool_lit;
    If _if;
    Elif elif;
    Else _else;
    While _while;
    For _for;
    Match match;
    Case _case;
    Back back;
    Next next;
    Jump jump;
    Delay delay;
    CondOp cond_op;
    BinaryOp binary_op;
    UnaryOp unary_op;
} ExprUnion;

typedef struct Expr
{
    ExprKind kind;
    ExprUnion u;
    ExprVector tags;
} Expr;

VECTOR_FUNC(ExprVector, expr_vector, Expr);

void indent(int depth)
{
    for (int i = 0; i <= depth; ++i)
    {
        printf("  ");
    }
}

void print_expr_vector(ExprVector exprs, int depth);

void print_expr(Expr expr, int depth)
{
#define PRINT_OPT_EXPR_VECTOR(vector, name, next) \
    if (vector.size)                              \
    {                                             \
        if (next)                                 \
        {                                         \
            printf(", ");                         \
        }                                         \
        printf(name " = [");                      \
        print_expr_vector(vector, depth + 1);     \
        indent(depth);                            \
        putchar(']');                             \
    }

    putchar('(');

    PRINT_OPT_EXPR_VECTOR(expr.tags, "tags", 0)
    if (expr.tags.size)
    {
        printf(", ");
    }

    switch (expr.kind)
    {
    case E_TAG:
        printf("name = %s", expr.u.tag.name);
        PRINT_OPT_EXPR_VECTOR(expr.u.tag.args, "args", 1)
        break;
    case E_TYPE:
        printf("const = %i, name = %s", expr.u.type._const, expr.u.type.name);
        if (expr.u.type.children.size)
        {
            printf(", children = [");
            print_expr_vector(expr.u.type.children, depth + 1);
            indent(depth);
            putchar(']');
        }
        break;
    case E_CONSTR_TYPE:
        printf("name = %s, constr = [", expr.u.constr_type.name);
        print_expr_vector(expr.u.constr_type.constr, depth + 1);
        indent(depth);
        putchar(']');
        break;
    case E_TAG_DEF:
        printf("pub = %i, name = %s", expr.u.tag_def.pub, expr.u.tag_def.name);
        PRINT_OPT_EXPR_VECTOR(expr.u.tag_def.args, "args", 1)
        break;
    case E_MOD:
        printf("pub = %i, name = %s", expr.u.mod.pub, expr.u.mod.name);
        PRINT_OPT_EXPR_VECTOR(expr.u.mod.body, "body", 1)
        break;
    case E_USE:
        printf("name = %s", expr.u.use.name);
        break;
    case E_STRUCT:
    case E_UNION:
        printf("pub = %i, name = %s", expr.u._struct.pub, expr.u._struct.name);
        PRINT_OPT_EXPR_VECTOR(expr.u._struct.body, "body", 1)
        break;
    case E_FIELD:
        printf("pub = %i, name = %s, type = ", expr.u.field.pub, expr.u.field.name);
        print_expr(*expr.u.field.type, depth);
        break;
    case E_ENUM:
        printf("pub = %i, name = %s", expr.u._enum.pub, expr.u._enum.name);
        PRINT_OPT_EXPR_VECTOR(expr.u._enum.body, "body", 1)
        break;
    case E_OPTION:
        printf("name = %s", expr.u.option.name);
        PRINT_OPT_EXPR_VECTOR(expr.u.option.body, "body", 1)
        break;
    case E_OPTION_FIELD:
        printf("name = %s, type = ", expr.u.option_field.name);
        print_expr(*expr.u.option_field.type, depth);
        break;
    case E_PROP:
        printf("pub = %i, name = %s", expr.u.prop.pub, expr.u.prop.name);
        PRINT_OPT_EXPR_VECTOR(expr.u.prop.body, "body", 1)
        break;
    case E_DEF:
        printf("pub = %i", expr.u.def.pub);
        printf(", type = ");
        print_expr(*expr.u.def.prop, depth);
        printf(", target = ");
        print_expr(*expr.u.def.target, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u.def.body, "body", 1)
        break;
    case E_SUB_DEF:
        printf("pub = %i, inl = %i, macro = %i, name = %s", expr.u.sub_def.pub, expr.u.sub_def.inl, expr.u.sub_def.macro, expr.u.sub_def.name);
        PRINT_OPT_EXPR_VECTOR(expr.u.sub_def.args, "args", 1)
        printf(", ret_type = ");
        print_expr(*expr.u.sub_def.ret_type, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u.sub_def.body, "body", 1)
        break;
    case E_ARG:
        printf("rest = %i, name = %s, type = ", expr.u.arg.rest, expr.u.arg.name);
        print_expr(*expr.u.arg.type, depth);
        break;
    case E_VAR_DEF:
        printf("pub = %i, inl = %i, name = %s", expr.u.var_def.pub, expr.u.var_def.inl, expr.u.var_def.name);
        if (expr.u.var_def.type)
        {
            printf(", type = ");
            print_expr(*expr.u.var_def.type, depth);
        }
        if (expr.u.var_def.value)
        {
            printf(", value = ");
            print_expr(*expr.u.var_def.value, depth);
        }
        break;
    case E_LOCAL_VAR_DEF:
        printf("name = %s", expr.u.local_var_def.name);
        if (expr.u.local_var_def.type)
        {
            printf(", type = ");
            print_expr(*expr.u.var_def.type, depth);
        }
        if (expr.u.local_var_def.value)
        {
            printf(", value = ");
            print_expr(*expr.u.local_var_def.value, depth);
        }
        break;
    case E_SUB_CALL:
        printf("type = ");
        print_expr(*expr.u.sub_call.type, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u.sub_call.args, "args", 1)
        break;
    case E_VAR_USE:
        printf("name = %s", expr.u.var_use.name);
        break;
    case E_STRUCT_INST:
        printf("type = ");
        print_expr(*expr.u.struct_inst.type, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u.struct_inst.fields, "fields", 1)
        break;
    case E_FIELD_VAL:
        printf("name = %s, value = ", expr.u.field_val.name);
        print_expr(*expr.u.field_val.value, depth);
        break;
    case E_ARR_LIT:
        PRINT_OPT_EXPR_VECTOR(expr.u.arr_lit.value, "value", expr.tags.size)
        break;
    case E_STRING_LIT:
    case E_CHAR_LIT:
    case E_INT_LIT:
    case E_FLOAT_LIT:
        printf("value = %s", expr.u.string_lit.value);
        break;
    case E_BOOL_LIT:
        printf("value = %i", expr.u.bool_lit.value);
        break;
    case E_DO:
    case E_BLOCK:
        PRINT_OPT_EXPR_VECTOR(expr.u._do.body, "body", expr.tags.size)
        break;
    case E_IF:
    case E_ELIF:
    case E_ELSE:
    case E_WHILE:
        printf("con = ");
        print_expr(*expr.u._if.con, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u._if.body, "body", 1)
        break;
    case E_FOR:
        if (expr.u._for.loop_var)
        {
            printf("loop_var = %s, ", expr.u._for.loop_var);
        }
        printf("range = ");
        print_expr(*expr.u._for.range, depth);
        break;
    case E_MATCH:
        printf("value = ");
        print_expr(*expr.u.match.value, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u.match.body, "body", 1)
        break;
    case E_CASE:
        printf("value = ");
        print_expr(*expr.u._case.value, depth);
        PRINT_OPT_EXPR_VECTOR(expr.u._case.body, "body", 1)
        break;
    case E_BACK:
        if (expr.u.back.label)
        {
            printf("label = %s", expr.u.back.label);
        }
        if (expr.u.back.value)
        {
            printf(", value = ");
            print_expr(*expr.u.back.value, depth);
        }
    case E_NEXT:
    case E_JUMP:
        if (expr.u.back.label)
        {
            printf("label = %s", expr.u.back.label);
        }
        break;
    case E_DELAY:
        PRINT_OPT_EXPR_VECTOR(expr.u.delay.body, "body", expr.tags.size)
        break;
    case E_COND_OP:
        printf("con = ");
        print_expr(*expr.u.cond_op.con, depth);
        printf(", lhs = ");
        print_expr(*expr.u.cond_op.lhs, depth);
        if (expr.u.cond_op.rhs)
        {
            printf(", rhs = ");
            print_expr(*expr.u.cond_op.rhs, depth);
        }
        break;
    case E_RANGE_OP:
    case E_RANGE_INCL_OP:
    case E_EQUAL_OP:
    case E_NOT_EQUAL_OP:
    case E_AND_OP:
    case E_OR_OP:
    case E_LESS_OP:
    case E_LESS_EQUAL_OP:
    case E_GREATER_OP:
    case E_GREATER_EQUAL_OP:
    case E_ADD_OP:
    case E_SUBTRACT_OP:
    case E_MUL_OP:
    case E_DIV_OP:
    case E_MOD_OP:
    case E_BIT_AND:
    case E_BIT_OR:
    case E_BIT_XOR:
    case E_LEFT_SHIFT:
    case E_RIGHT_SHIFT:
    case E_ASSIGN_OP:
    case E_ADD_ASSIGN_OP:
    case E_SUBTRACT_ASSIGN_OP:
    case E_MUL_ASSIGN_OP:
    case E_DIV_ASSIGN_OP:
    case E_MOD_ASSIGN_OP:
    case E_BIT_AND_ASSIGN:
    case E_BIT_OR_ASSIGN:
    case E_BIT_XOR_ASSIGN:
    case E_LEFT_SHIFT_ASSIGN:
    case E_RIGHT_SHIFT_ASSIGN:
        printf("lhs = ");
        print_expr(*expr.u.binary_op.lhs, depth);
        printf(", rhs = ");
        print_expr(*expr.u.binary_op.rhs, depth);
        break;
    case E_DEREF_OP:
    case E_ADDRESS_OP:
    case E_NEGATION_OP:
    case E_NOT_OP:
    case E_BIT_NOT_OP:
        printf("val = ");
        print_expr(*expr.u.unary_op.val, depth);
        break;
    }
    putchar(')');
}

void print_expr_vector(ExprVector exprs, int depth)
{
    const char *const expr_names[] =
        {
            [E_TAG] = "TAG",
            [E_TYPE] = "TYPE",
            [E_CONSTR_TYPE] = "CONSTR_TYPE",
            [E_TAG_DEF] = "TAG_DEF",
            [E_MOD] = "MOD",
            [E_USE] = "USE",
            [E_STRUCT] = "STRUCT",
            [E_UNION] = "UNION",
            [E_FIELD] = "FIELD",
            [E_ENUM] = "ENUM",
            [E_OPTION] = "OPTION",
            [E_OPTION_FIELD] = "OPTION_FIELD",
            [E_PROP] = "PROP",
            [E_DEF] = "DEF",
            [E_SUB_DEF] = "SUB_DEF",
            [E_ARG] = "ARG",
            [E_VAR_DEF] = "VAR_DEF",
            [E_LOCAL_VAR_DEF] = "LOCAL_VAR_DEF",
            [E_SUB_CALL] = "SUB_CALL",
            [E_VAR_USE] = "VAR_USE",
            [E_STRUCT_INST] = "STRUCT_INST",
            [E_FIELD_VAL] = "FIELD_VAL",
            [E_STRING_LIT] = "STRING_LIT",
            [E_CHAR_LIT] = "CHAR_LIT",
            [E_ARR_LIT] = "ARR_LIT",
            [E_INT_LIT] = "INT_LIT",
            [E_FLOAT_LIT] = "FLOAT_LIT",
            [E_BOOL_LIT] = "BOOL_LIT",
            [E_NULL_LIT] = "NULL_LIT",
            [E_THIS_LIT] = "THIS_LIT",
            [E_TYPE_LIT] = "TYPE_LIT",
            [E_WHERE] = "WHERE",
            [E_DO] = "DO",
            [E_BLOCK] = "BLOCK",
            [E_AS] = "AS",
            [E_IF] = "IF",
            [E_ELIF] = "ELIF",
            [E_ELSE] = "ELSE",
            [E_WHILE] = "WHILE",
            [E_FOR] = "FOR",
            [E_MATCH] = "MATCH",
            [E_CASE] = "CASE",
            [E_BACK] = "BACK",
            [E_NEXT] = "NEXT",
            [E_DELAY] = "DELAY",
            [E_JUMP] = "JUMP",
            [E_COND_OP] = "COND_OP",
            [E_RANGE_OP] = "RANGE_OP",
            [E_RANGE_INCL_OP] = "RANGE_INCL_OP",
            [E_EQUAL_OP] = "EQUAL_OP",
            [E_NOT_EQUAL_OP] = "NOT_EQUAL_OP",
            [E_AND_OP] = "AND_OP",
            [E_OR_OP] = "OR_OP",
            [E_LESS_OP] = "LESS_OP",
            [E_LESS_EQUAL_OP] = "LESS_EQUAL_OP",
            [E_GREATER_OP] = "GREATER_OP",
            [E_GREATER_EQUAL_OP] = "GREATER_EQUAL_OP",
            [E_ADD_OP] = "ADD_OP",
            [E_SUBTRACT_OP] = "SUBTRACT_OP",
            [E_MUL_OP] = "MUL_OP",
            [E_DIV_OP] = "DIV_OP",
            [E_MOD_OP] = "MOD_OP",
            [E_BIT_AND] = "BIT_AND",
            [E_BIT_OR] = "BIT_OR",
            [E_BIT_XOR] = "BIT_XOR",
            [E_LEFT_SHIFT] = "LEFT_SHIFT",
            [E_RIGHT_SHIFT] = "RIGHT_SHIFT",
            [E_DEREF_OP] = "DEREF_OP",
            [E_ADDRESS_OP] = "ADDRESS_OP",
            [E_NEGATION_OP] = "NEGATION_OP",
            [E_NOT_OP] = "NOT_OP",
            [E_BIT_NOT_OP] = "BIT_NOT_OP",
            [E_ASSIGN_OP] = "ASSIGN_OP",
            [E_ADD_ASSIGN_OP] = "ADD_ASSIGN_OP",
            [E_SUBTRACT_ASSIGN_OP] = "SUBTRACT_ASSIGN_OP",
            [E_MUL_ASSIGN_OP] = "MUL_ASSIGN_OP",
            [E_DIV_ASSIGN_OP] = "DIV_ASSIGN_OP",
            [E_MOD_ASSIGN_OP] = "MOD_ASSIGN_OP",
            [E_BIT_AND_ASSIGN] = "BIT_AND_ASSIGN",
            [E_BIT_OR_ASSIGN] = "BIT_OR_ASSIGN",
            [E_BIT_XOR_ASSIGN] = "BIT_XOR_ASSIGN",
            [E_LEFT_SHIFT_ASSIGN] = "LEFT_SHIFT_ASSIGN",
            [E_RIGHT_SHIFT_ASSIGN] = "RIGHT_SHIFT_ASSIGN"};

    if (depth == 0)
    {
        printf("Exprs:");
    }
    for (int i = 0; i < exprs.size; ++i)
    {
        putchar('\n');
        indent(depth);
        COLOR(GREEN);
        printf("%s", expr_names[exprs.array[i].kind]);
        COLOR(RESET);
        print_expr(exprs.array[i], depth);
    }
    putchar('\n');
    if (depth == 0)
    {
        putchar('\n');
    }
}

#define NEXT_TOKEN token = tokens.array[++*index]

#define EXPECT_TOKEN(_kind, error, code) \
    if (token.kind != _kind)             \
    {                                    \
        THROW(token.index, error, 0);    \
    }                                    \
    code;                                \
    NEXT_TOKEN

#define OPTIONAL_TOKEN(_kind, true_code, false_code) \
    if (token.kind == _kind)                         \
    {                                                \
        NEXT_TOKEN;                                  \
        true_code;                                   \
    }                                                \
    else                                             \
    {                                                \
        false_code;                                  \
    }

ExprVector parse_tags(TokenVector tokens, int *index)
{
    ExprVector tags = expr_vector_new(2);

main:
    while (tokens.array[*index].kind == HASH)
    {
        Token token = tokens.array[++*index];
        if (token.kind == IDENT)
        {
            Expr tag;
            tag.kind = E_TAG;
            tag.tags.size = 0;
            tag.u.tag.name = token.value;
            tag.u.tag.args = expr_vector_new(2);

            token = tokens.array[++*index];
            if (token.kind == LEFT_PAREN)
            {
                while (1)
                {
                    token = tokens.array[++*index];
                    if (token.kind == RIGHT_PAREN)
                    {
                        break;
                    }

                    Expr lit;
                    lit.kind = -1;
                    lit.tags.size = 0;

                    switch (token.kind)
                    {
                    case STRING_LIT:
                        lit.kind = E_STRING_LIT;
                        lit.u.string_lit.value = token.value;
                        break;
                    case CHAR_LIT:
                        lit.kind = E_CHAR_LIT;
                        lit.u.char_lit.value = token.value;
                        break;
                    case INT_LIT:
                        lit.kind = E_INT_LIT;
                        lit.u.char_lit.value = token.value;
                        break;
                    case FLOAT_LIT:
                        lit.kind = E_FLOAT_LIT;
                        lit.u.char_lit.value = token.value;
                        break;
                    case LEFT_BRACKET:
                        lit.kind = E_ARR_LIT;
                        THROW(token.index, "unexpected array literal", 0);
                        break;
                    case _TRUE:
                    case _FALSE:
                        lit.kind = E_BOOL_LIT;
                        lit.u.bool_lit.value = token.kind == _TRUE;
                        break;
                    }

                    if (lit.kind != -1)
                    {
                        expr_vector_push(&tag.u.tag.args, lit);

                        token = tokens.array[++*index];
                        if (token.kind == COMMA)
                        {
                            continue;
                        }
                        else if (token.kind == RIGHT_PAREN)
                        {
                            break;
                        }
                    }
                    THROW(token.index, "unexpected smth in 'tag' declaration", 0);
                }
                expr_vector_push(&tags, tag);
                ++*index;
            }
            else
            {
                expr_vector_push(&tags, tag);
                goto main;
            }
        }
        else
        {
            THROW(tokens.array[*index - 1].index, "expected identifier after tag declaration", 0);
        }
    }

    return tags;
}

Expr *parse_type(TokenVector tokens, int *index)
{
    Expr *expr = malloc(sizeof(Expr));
    expr->kind = E_TYPE;
    expr->tags = parse_tags(tokens, index);

    // actually parse the type
    expr->u.type._const = 0;
    expr->u.type.name = tokens.array[*index].value;
    expr->u.type.children = expr_vector_new(1);
    *index += 1;

    return expr;
}

Expr *parse_value(TokenVector tokens, int *index)
{
    return 0;
}

Expr parse_local(TokenVector tokens, int *index)
{
    Expr expr;
    expr.tags = parse_tags(tokens, index);

    Token token = tokens.array[*index];

    switch (token.kind)
    {
    case IF:
        break;
    case DO:
        break;
    case WHILE:
        break;
    case FOR:
        break;
    case MATCH:
        break;
    case BACK:
        break;
    case NEXT:
        break;
    case JUMP:
        break;
    case DELAY:
        break;
    case VAR:
        break;
    default:
        THROW(token.index, "expected item in local scope", 0);
    }

    return expr;
}

ExprVector parse_local_block(TokenVector tokens, int *index)
{
    ExprVector exprs = expr_vector_new(4);

    while (*index < tokens.size)
    {
        expr_vector_push(&exprs, parse_local(tokens, index));
    }

    return exprs;
}

ExprVector parse_global_block(TokenVector tokens, int *index);

Expr parse_global(TokenVector tokens, int *index)
{
    Expr expr;
    expr.tags = parse_tags(tokens, index);

    Token token = tokens.array[*index];

    switch (token.kind)
    {
    case USE:
        expr.kind = E_USE;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'use' keyword", expr.u.use.name = token.value);
        EXPECT_TOKEN(SEMICOLON, "expected ';' after 'use' path", {});
        break;
    case MOD:
        expr.kind = E_MOD;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'mod' keyword", expr.u.mod.name = token.value);
        EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'struct' name", {});
        expr.u.mod.body = parse_global_block(tokens, index);
        break;
    case STRUCT:
    case UNION:
        expr.kind = token.kind == STRUCT ? E_STRUCT : E_UNION;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'struct' keyword", expr.u._struct.name = token.value);
        EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'struct' name", {});
        expr.u._struct.body = expr_vector_new(4);
        while (1)
        {
            OPTIONAL_TOKEN(RIGHT_BRACE, goto end_struct, {});
            Expr field;
            field.kind = E_FIELD;
            field.tags = parse_tags(tokens, index);
            token = tokens.array[*index];
            EXPECT_TOKEN(IDENT, "expected field name in 'struct' body", field.u.field.name = token.value);
            EXPECT_TOKEN(COLON, "expected ':' after field name", {});
            field.u.field.type = parse_type(tokens, index);
            token = tokens.array[*index];
            expr_vector_push(&expr.u._struct.body, field);
            OPTIONAL_TOKEN(COMMA, {}, break);
        }
        EXPECT_TOKEN(RIGHT_BRACE, "expected '}' after last field", {});
    end_struct:
        break;
    case ENUM:
        expr.kind = E_ENUM;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'enum' keyword", expr.u._enum.name = token.value);
        EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'enum' name", {});
        expr.u._enum.body = expr_vector_new(4);
        while (1)
        {
            OPTIONAL_TOKEN(RIGHT_BRACE, goto end_enum, {});
            Expr opt;
            opt.kind = E_OPTION;
            opt.tags = parse_tags(tokens, index);
            token = tokens.array[*index];
            EXPECT_TOKEN(IDENT, "expected option name in 'struct' body", opt.u.option.name = token.value);
            OPTIONAL_TOKEN(
                LEFT_PAREN,
                opt.u.option.body = expr_vector_new(2);
                while (1) {
                    OPTIONAL_TOKEN(RIGHT_PAREN, goto end_option, {});
                    Expr field;
                    field.kind = E_OPTION_FIELD;
                    field.tags = parse_tags(tokens, index);
                    token = tokens.array[*index];
                    EXPECT_TOKEN(IDENT, "expected field name in 'option' body", field.u.option_field.name = token.value);
                    EXPECT_TOKEN(COLON, "expected ':' after field name", {});
                    field.u.option_field.type = parse_type(tokens, index);
                    token = tokens.array[*index];
                    expr_vector_push(&opt.u.option.body, field);
                    OPTIONAL_TOKEN(COMMA, {}, break);
                };
                EXPECT_TOKEN(RIGHT_PAREN, "expected ')' after last field", {});
                end_option:
                    , opt.u.option.body.size = 0);
            expr_vector_push(&expr.u._enum.body, opt);
            OPTIONAL_TOKEN(COMMA, {}, break);
        }
        EXPECT_TOKEN(RIGHT_BRACE, "expected '}' after last option", {});
    end_enum:
        break;
    case TAG:
        expr.kind = E_TAG_DEF;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'tag' keyword", expr.u.tag_def.name = token.value);
        EXPECT_TOKEN(LEFT_PAREN, "expected '(' after 'tag' name", {});
        expr.u.tag_def.args = expr_vector_new(2);
        while (1)
        {
            OPTIONAL_TOKEN(RIGHT_PAREN, goto end_tag_def, {});
            Expr arg;
            arg.kind = E_ARG;
            arg.tags = parse_tags(tokens, index);
            token = tokens.array[*index];
            EXPECT_TOKEN(IDENT, "expected arg name in 'tag' args", arg.u.arg.name = token.value);
            EXPECT_TOKEN(COLON, "expected ':' after arg name", {});
            arg.u.arg.type = parse_type(tokens, index);
            token = tokens.array[*index];
            expr_vector_push(&expr.u.tag_def.args, arg);
            OPTIONAL_TOKEN(COMMA, {}, break);
        }
        EXPECT_TOKEN(RIGHT_PAREN, "expected ')' after last arg", {});
    end_tag_def:
        EXPECT_TOKEN(SEMICOLON, "expected ';' after tag args", {});
        break;
    case PROP:
        expr.kind = E_PROP;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'prop' keyword", expr.u.prop.name = token.value);
        EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'prop' name", {});
        expr.u.prop.body = parse_global_block(tokens, index);
        break;
    case DEF:
        expr.kind = E_DEF;
        NEXT_TOKEN;
        expr.u.def.prop = parse_type(tokens, index);
        token = tokens.array[*index];
        EXPECT_TOKEN(FOR, "expected 'for' after 'def' property", {});
        expr.u.def.target = parse_type(tokens, index);
        token = tokens.array[*index];
        EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'def' target", {});
        expr.u.def.body = parse_global_block(tokens, index);
        break;
    case SUB:
        expr.kind = E_SUB_DEF;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'sub' keyword", expr.u.sub_def.name = token.value);
        EXPECT_TOKEN(LEFT_PAREN, "expected '(' after 'sub' name", {});
        expr.u.sub_def.args = expr_vector_new(2);
        while (1)
        {
            OPTIONAL_TOKEN(RIGHT_PAREN, goto end_sub_def, {});
            Expr arg;
            arg.kind = E_ARG;
            arg.tags = parse_tags(tokens, index);
            token = tokens.array[*index];
            EXPECT_TOKEN(IDENT, "expected arg name in 'tag' args", arg.u.arg.name = token.value);
            EXPECT_TOKEN(COLON, "expected ':' after arg name", {});
            arg.u.arg.type = parse_type(tokens, index);
            token = tokens.array[*index];
            expr_vector_push(&expr.u.sub_def.args, arg);
            OPTIONAL_TOKEN(COMMA, {}, break);
        }
        EXPECT_TOKEN(RIGHT_PAREN, "expected ')' after last arg", {});
    end_sub_def:
        OPTIONAL_TOKEN(
            ARROW,
            expr.u.sub_def.ret_type = parse_type(tokens, index);
            token = tokens.array[*index];
            , {});
        EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'sub' args", {});
        expr.u.sub_def.body = parse_global_block(tokens, index);
        break;
    case VAR:
        expr.kind = E_VAR_DEF;
        NEXT_TOKEN;
        EXPECT_TOKEN(IDENT, "expected identifier after 'var' keyword", expr.u.var_def.name = token.value);
        EXPECT_TOKEN(COLON, "expected ':' after variable name", {});
        expr.u.var_def.type = parse_type(tokens, index);
        token = tokens.array[*index];
        expr.u.var_def.value = NULL;
        EXPECT_TOKEN(SEMICOLON, "expected ';' after variable declaration", {});
        break;
    default:
        THROW(token.index, "expected item in global scope", 0);
    }

    return expr;
}

ExprVector parse_global_block(TokenVector tokens, int *index)
{
    ExprVector exprs = expr_vector_new(4);

    char is_global = *index == 0;

    while (*index < tokens.size)
    {
        if (tokens.array[*index].kind == RIGHT_BRACE)
        {
            if (!is_global)
            {
                *index += 1;
                return exprs;
            }
            else
            {
                THROW(tokens.array[*index].index, "unexpected item in global scope", 0);
            }
        }

        expr_vector_push(&exprs, parse_global(tokens, index));
    }

    if (!is_global)
    {
        THROW(current_file.size, "expected a closing brace '}'", 0);
    }

    return exprs;
}