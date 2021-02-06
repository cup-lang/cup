const tokenKind = require('./lexer.js').tokenKind;

const exprKind = {
    TAG: 'tag',
    TYPE: 'type',
    CONSTR_TYPE: 'constr_type',
    TAG_DEF: 'tag_def',
    MOD: 'mod',
    USE: 'use',
    STRUCT: 'struct',
    UNION: 'union',
    FIELD: 'field',
    ENUM: 'enum',
    OPTION: 'option',
    OPTION_FIELD: 'option_field',
    PROP: 'prop',
    DEF: 'def',
    SUB_DEF: 'sub_def',
    ARG: 'arg',
    VAR_DEF: 'var_def',

    LOCAL_VAR_DEF: 'local_var_def',
    SUB_CALL: 'sub_call',
    VAR_USE: 'var_use',
    STRUCT_INST: 'struct_inst',
    FIELD_VAL: 'field_val',
    STRING_LIT: 'string_lit',
    CHAR_LIT: 'char_lit',
    ARR_LIT: 'arr_lit',
    INT_LIT: 'int_lit',
    FLOAT_LIT: 'float_lit',
    BOOL_LIT: 'bool_lit',
    NULL_LIT: 'null_lit',
    THIS_LIT: 'this_lit',
    TYPE_LIT: 'type_lit',
    WHERE: 'where',
    DO: 'do',
    BLOCK: 'block',
    IF: 'if',
    ELIF: 'elif',
    ELSE: 'else',
    WHILE: 'while',
    FOR: 'for',
    MATCH: 'match',
    CASE: 'case',
    BACK: 'back',
    NEXT: 'next',
    DELAY: 'delay',
    JUMP: 'jump',

    DEREF_OP: 'deref_op',
    ADDRESS_OP: 'address_op',
    NEGATION_OP: 'negation_op',
    COND_OP: 'cond_op',
    RANGE_OP: 'range_op',
    RANGE_INCL_OP: 'range_incl_op',
    ASSIGN_OP: 'assign_op',
    EQUAL_OP: 'equal_op',
    NOT_OP: 'not_op',
    NOT_EQUAL_OP: 'not_equal_op',
    AND_OP: 'and_op',
    OR_OP: 'or_op',
    LESS_OP: 'less_op',
    LESS_EQUAL_OP: 'less_equal_op',
    GREATER_OP: 'greater_op',
    GREATER_EQUAL_OP: 'greater_equal_op',
    ADD_OP: 'add_op',
    ADD_ASSIGN_OP: 'add_assign_op',
    SUBTRACT_OP: 'substract_op',
    SUBTRACT_ASSIGN_OP: 'substract_assign_op',
    MULTIPLY_OP: 'multiply_op',
    MULTIPLY_ASSIGN_OP: 'multiply_assign_op',
    DIVIDE_OP: 'divide_op',
    DIVIDE_ASSIGN_OP: 'divide_assign_op',
    MODULO_OP: 'modulo_op',
    MODULO_ASSIGN_OP: 'modulo_assign_op',
};

let tokens;
let index;

function parseTags() {
    let tags = [];

    while (tokens[index].kind === tokenKind.HASH) {
        let token = tokens[++index];
        if (token.kind === tokenKind.IDENT) {
            let tag = {
                kind: exprKind.TAG,
                name: token.value,
                args: []
            };

            token = tokens[++index];
            if (token.kind === tokenKind.LEFT_PAREN) {
                while (1) {
                    token = tokens[++index];
                    if (token.kind === tokenKind.RIGHT_PAREN) {
                        break;
                    }

                    let lit = { kind: -1 };

                    switch (token.kind) {
                        case tokenKind.STRING_LIT:
                            lit.kind = E_STRING_LIT;
                            lit.value = token.value;
                            break;
                        case tokenKind.CHAR_LIT:
                            lit.kind = E_CHAR_LIT;
                            lit.value = token.value;
                            break;
                        case tokenKind.INT_LIT:
                            lit.kind = E_INT_LIT;
                            lit.value = token.value;
                            break;
                        case tokenKind.FLOAT_LIT:
                            lit.kind = E_FLOAT_LIT;
                            lit.value = token.value;
                            break;
                        case tokenKind.LEFT_BRACKET:
                            throw `at ${token.index} unexpected array literal`;
                        case tokenKind.TRUE:
                        case tokenKind.FALSE:
                            lit.kind = E_BOOL_LIT;
                            lit.u.bool_lit.value = token.kind == _TRUE;
                            break;
                    }

                    if (lit.kind !== -1) {
                        tag.args.push(lit);

                        token = tokens[++index];
                        if (token.kind === tokenKind.COMMA) {
                            continue;
                        }
                        else if (token.kind === tokenKind.RIGHT_PAREN) {
                            break;
                        }
                    }
                    throw `at ${token.index} unexpected smth in 'tag' declaration`;
                }
                tags.push(tag);
                ++index;
            }
            else {
                tags.push(tag);
                continue;
            }
        }
        else {
            throw `at ${tokens[index - 1].index} expected identifier after tag declaration`;
        }
    }

    return tags;
}

function parseLocal() {

}

function parseLocalBlock() {
    let exprs = [];

    return exprs;
}

function parseGlobal() {
    let expr = {};
    expr.tags = parseTags();

    const token = tokens[index];
    ++index;

    switch (token.kind) {
        // case USE:
        //     expr.kind = E_USE;
        //     NEXT_TOKEN;
        //     EXPECT_TOKEN(IDENT, "expected identifier after 'use' keyword", expr.u.use.name = token.value);
        //     EXPECT_TOKEN(SEMICOLON, "expected ';' after 'use' path", {});
        //     break;
        // case MOD:
        //     expr.kind = E_MOD;
        //     NEXT_TOKEN;
        //     EXPECT_TOKEN(IDENT, "expected identifier after 'mod' keyword", expr.u.mod.name = token.value);
        //     EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'struct' name", {});
        //     expr.u.mod.body = parse_global_block(tokens, index);
        //     break;
        // case STRUCT:
        // case UNION:
        //     expr.kind = token.kind == STRUCT ? E_STRUCT : E_UNION;
        //     NEXT_TOKEN;
        //     EXPECT_TOKEN(IDENT, "expected identifier after 'struct' keyword", expr.u._struct.name = token.value);
        //     EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'struct' name", {});
        //     expr.u._struct.body = expr_vector_new(4);
        //     while (1)
        //     {
        //         OPTIONAL_TOKEN(RIGHT_BRACE, goto end_struct, {});
        //         Expr field;
        //         field.kind = E_FIELD;
        //         field.tags = parse_tags(tokens, index);
        //         token = tokens.array[*index];
        //         EXPECT_TOKEN(IDENT, "expected field name in 'struct' body", field.u.field.name = token.value);
        //         EXPECT_TOKEN(COLON, "expected ':' after field name", {});
        //         field.u.field.type = parse_type(tokens, index);
        //         token = tokens.array[*index];
        //         expr_vector_push(&expr.u._struct.body, field);
        //         OPTIONAL_TOKEN(COMMA, {}, break);
        //     }
        //     EXPECT_TOKEN(RIGHT_BRACE, "expected '}' after last field", {});
        // end_struct:
        //     break;
        // case ENUM:
        //     expr.kind = E_ENUM;
        //     NEXT_TOKEN;
        //     EXPECT_TOKEN(IDENT, "expected identifier after 'enum' keyword", expr.u._enum.name = token.value);
        //     EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'enum' name", {});
        //     expr.u._enum.body = expr_vector_new(4);
        //     while (1)
        //     {
        //         OPTIONAL_TOKEN(RIGHT_BRACE, goto end_enum, {});
        //         Expr opt;
        //         opt.kind = E_OPTION;
        //         opt.tags = parse_tags(tokens, index);
        //         token = tokens.array[*index];
        //         EXPECT_TOKEN(IDENT, "expected option name in 'struct' body", opt.u.option.name = token.value);
        //         OPTIONAL_TOKEN(
        //             LEFT_PAREN,
        //             opt.u.option.body = expr_vector_new(2);
        //             while (1) {
        //                 OPTIONAL_TOKEN(RIGHT_PAREN, goto end_option, {});
        //                 Expr field;
        //                 field.kind = E_OPTION_FIELD;
        //                 field.tags = parse_tags(tokens, index);
        //                 token = tokens.array[*index];
        //                 EXPECT_TOKEN(IDENT, "expected field name in 'option' body", field.u.option_field.name = token.value);
        //                 EXPECT_TOKEN(COLON, "expected ':' after field name", {});
        //                 field.u.option_field.type = parse_type(tokens, index);
        //                 token = tokens.array[*index];
        //                 expr_vector_push(&opt.u.option.body, field);
        //                 OPTIONAL_TOKEN(COMMA, {}, break);
        //             };
        //             EXPECT_TOKEN(RIGHT_PAREN, "expected ')' after last field", {});
        //             end_option:
        //                 , opt.u.option.body.size = 0);
        //         expr_vector_push(&expr.u._enum.body, opt);
        //         OPTIONAL_TOKEN(COMMA, {}, break);
        //     }
        //     EXPECT_TOKEN(RIGHT_BRACE, "expected '}' after last option", {});
        // end_enum:
        //     break;
        // case TAG:
        //     expr.kind = E_TAG_DEF;
        //     NEXT_TOKEN;
        //     EXPECT_TOKEN(IDENT, "expected identifier after 'tag' keyword", expr.u.tag_def.name = token.value);
        //     EXPECT_TOKEN(LEFT_PAREN, "expected '(' after 'tag' name", {});
        //     expr.u.tag_def.args = expr_vector_new(2);
        //     while (1)
        //     {
        //         OPTIONAL_TOKEN(RIGHT_PAREN, goto end_tag_def, {});
        //         Expr arg;
        //         arg.kind = E_ARG;
        //         arg.tags = parse_tags(tokens, index);
        //         token = tokens.array[*index];
        //         EXPECT_TOKEN(IDENT, "expected arg name in 'tag' args", arg.u.arg.name = token.value);
        //         EXPECT_TOKEN(COLON, "expected ':' after arg name", {});
        //         arg.u.arg.type = parse_type(tokens, index);
        //         token = tokens.array[*index];
        //         expr_vector_push(&expr.u.tag_def.args, arg);
        //         OPTIONAL_TOKEN(COMMA, {}, break);
        //     }
        //     EXPECT_TOKEN(RIGHT_PAREN, "expected ')' after last arg", {});
        // end_tag_def:
        //     EXPECT_TOKEN(SEMICOLON, "expected ';' after tag args", {});
        //     break;
        // case PROP:
        //     expr.kind = E_PROP;
        //     NEXT_TOKEN;
        //     EXPECT_TOKEN(IDENT, "expected identifier after 'prop' keyword", expr.u.prop.name = token.value);
        //     EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'prop' name", {});
        //     expr.u.prop.body = parse_global_block(tokens, index);
        //     break;
        // case DEF:
        //     expr.kind = E_DEF;
        //     NEXT_TOKEN;
        //     expr.u.def.prop = parse_type(tokens, index);
        //     token = tokens.array[*index];
        //     EXPECT_TOKEN(FOR, "expected 'for' after 'def' property", {});
        //     expr.u.def.target = parse_type(tokens, index);
        //     token = tokens.array[*index];
        //     EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'def' target", {});
        //     expr.u.def.body = parse_global_block(tokens, index);
        //     break;
        // case SUB:
        //     expr.kind = E_SUB_DEF;
        //     NEXT_TOKEN;
        //     EXPECT_TOKEN(IDENT, "expected identifier after 'sub' keyword", expr.u.sub_def.name = token.value);
        //     EXPECT_TOKEN(LEFT_PAREN, "expected '(' after 'sub' name", {});
        //     expr.u.sub_def.args = expr_vector_new(2);
        //     while (1)
        //     {
        //         OPTIONAL_TOKEN(RIGHT_PAREN, goto end_sub_def, {});
        //         Expr arg;
        //         arg.kind = E_ARG;
        //         arg.tags = parse_tags(tokens, index);
        //         token = tokens.array[*index];
        //         EXPECT_TOKEN(IDENT, "expected arg name in 'tag' args", arg.u.arg.name = token.value);
        //         EXPECT_TOKEN(COLON, "expected ':' after arg name", {});
        //         arg.u.arg.type = parse_type(tokens, index);
        //         token = tokens.array[*index];
        //         expr_vector_push(&expr.u.sub_def.args, arg);
        //         OPTIONAL_TOKEN(COMMA, {}, break);
        //     }
        //     EXPECT_TOKEN(RIGHT_PAREN, "expected ')' after last arg", {});
        // end_sub_def:
        //     OPTIONAL_TOKEN(
        //         ARROW,
        //         expr.u.sub_def.ret_type = parse_type(tokens, index);
        //         token = tokens.array[*index];
        //         , {});
        //     EXPECT_TOKEN(LEFT_BRACE, "expected '{' after 'sub' args", {});
        //     expr.u.sub_def.body = parse_global_block(tokens, index);
        //     break;
        // case VAR:
        //     expr.kind = E_VAR_DEF;
        //     NEXT_TOKEN;
        //     EXPECT_TOKEN(IDENT, "expected identifier after 'var' keyword", expr.u.var_def.name = token.value);
        //     EXPECT_TOKEN(COLON, "expected ':' after variable name", {});
        //     expr.u.var_def.type = parse_type(tokens, index);
        //     token = tokens.array[*index];
        //     expr.u.var_def.value = NULL;
        //     EXPECT_TOKEN(SEMICOLON, "expected ';' after variable declaration", {});
        //     break;
        default:
            throw `at ${token.index} expected item in global scope`;
    }

    return expr;
}

function parseGlobalBlock() {
    let exprs = [];

    const is_global = index === 0;

    while (index < tokens.length) {
        if (tokens[index].kind === tokenKind.RIGHT_BRACE) {
            if (!is_global) {
                ++index;
                return exprs;
            }
            else {
                throw `at ${tokens[index].index} unexpected item in global scope`;
            }
        }

        exprs.push(parseGlobal());
    }

    return exprs;
}

module.exports.parse = function (_tokens) {
    tokens = _tokens;
    index = 0;
    return parseGlobalBlock();
};