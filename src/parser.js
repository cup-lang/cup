const tokenKind = require('./lexer.js').tokenKind;

const exprKind = {
    TAG: 'tag',
    TYPE: 'type',
    CONSTR_TYPE: 'constr_type',
    TAG_DEF: 'tag_def',
    MOD: 'mod',
    USE: 'use',
    COMP: 'comp',
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
    COMP_INST: 'comp_inst',
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

function nextToken() { return tokens[++index]; }

function expectToken(kind, error, code) {
    let token = tokens[index];
    if (token.kind !== kind) { throw `at ${token.index} ${error}`; }
    if (code) { code(); }
    return nextToken();
}

function optionalToken(kind, true_code, false_code) {
    let token = tokens[index];
    if (token.kind === kind) {
        token = nextToken();
        if (true_code) { true_code(); }
    }
    else if (false_code) {
        false_code();
    }
    return token;
}

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
                            lit.kind = exprKind.STRING_LIT;
                            lit.value = token.value;
                            break;
                        case tokenKind.CHAR_LIT:
                            lit.kind = exprKind.CHAR_LIT;
                            lit.value = token.value;
                            break;
                        case tokenKind.INT_LIT:
                            lit.kind = exprKind.INT_LIT;
                            lit.value = token.value;
                            break;
                        case tokenKind.FLOAT_LIT:
                            lit.kind = exprKind.FLOAT_LIT;
                            lit.value = token.value;
                            break;
                        case tokenKind.LEFT_BRACKET:
                            throw `at ${token.index} unexpected array literal`;
                        case tokenKind.TRUE:
                        case tokenKind.FALSE:
                            lit.kind = exprKind.BOOL_LIT;
                            lit.value = token.kind === TtokenKind.RUE;
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

function parseType() {
    let type = { kind: exprKind.TYPE };

    let token = tokens[index];

    if (token.kind === tokenKind.IDENT) {
        type.name = token.value;
        nextToken();
    } else {
        throw `at ${token.index} expected a type`;
    }

    return type;
}

function parseValue(endTokenKind) {

}

function parseLocal() {
    let expr = {};
    expr.tags = parseTags();

    let token = tokens[index];

    switch (token.kind) {
        case tokenKind.IF:
            expr.kind = exprKind.IF;
            expr.if = parseValue(tokenKind.LEFT_BRACE);
            token = nextToken();
            expr.if
            break;
        case tokenKind.DO:
            break;
        case tokenKind.WHILE:
            break;
        case tokenKind.FOR:
            break;
        case tokenKind.MATCH:
            break;
        case tokenKind.BACK:
            break;
        case tokenKind.NEXT:
            break;
        case tokenKind.JUMP:
            break;
        case tokenKind.DELAY:
            break;
        default:
            throw `at ${token.index} expected item in local scope`;
    }

    return expr;
}

function parseGlobal() {
    let expr = {};
    expr.tags = parseTags();

    let token = tokens[index];

    end:
    switch (token.kind) {
        case tokenKind.USE:
            expr.kind = exprKind.USE;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'use' keyword", () => {
                expr.name = token.value;
            });
            token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'use' path");
            break;
        case tokenKind.MOD:
            expr.kind = exprKind.MOD;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'mod' keyword", () => {
                expr.name = token.value;
            });
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'comp' name");
            expr.body = parseBlock();
            break;
        case tokenKind.COMP:
            expr.kind = exprKind.COMP;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'comp' keyword", () => {
                expr.name = token.value;
            });
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'comp' name");
            expr.body = [];
            while (1) {
                let should_end;
                token = optionalToken(tokenKind.RIGHT_BRACE, () => {
                    should_end = true;
                });
                if (should_end) { break end; }
                let field = {
                    kind: exprKind.FIELD,
                    tags: parseTags()
                };
                token = tokens[index];
                token = expectToken(tokenKind.IDENT, "expected field name in 'comp' body", () => {
                    field.name = token.value;
                });
                token = expectToken(tokenKind.COLON, "expected ':' after field name");
                field.type = parseType();
                expr.body.push(field);
                let should_break;
                token = optionalToken(tokenKind.COMMA, null, () => {
                    should_break = true;
                });
                if (should_break) { break; }
            }
            token = expectToken(tokenKind.RIGHT_BRACE, "expected '}' after last field");
            break;
        case tokenKind.ENUM:
            expr.kind = exprKind.ENUM;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'enum' keyword", () => {
                expr.name = token.value;
            });
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'enum' name");
            expr.body = [];
            while (1) {
                let should_end;
                token = optionalToken(tokenKind.RIGHT_BRACE, () => {
                    should_end = true;
                });
                if (should_end) { break end; }
                let opt = {
                    kind: exprKind.OPTION,
                    tags: parseTags(),
                    body: []
                };
                token = expectToken(tokenKind.IDENT, "expected option name in 'enum' body", () => {
                    opt.name = token.value;
                });
                token = optionalToken(tokenKind.LEFT_PAREN, () => {
                    while (1) {
                        let should_return;
                        token = optionalToken(tokenKind.RIGHT_PAREN, () => {
                            should_return = true;
                        });
                        if (should_return) { return; }
                        let field = {
                            kind: exprKind.OPTION_FIELD,
                            tags: parseTags()
                        };
                        token = expectToken(tokenKind.IDENT, "expected field name in 'option' body", () => {
                            field.name = token.value;
                        });
                        token = expectToken(tokenKind.COLON, "expected ':' after field name");
                        field.type = parseType();
                        opt.body.push(field);
                        let should_break;
                        token = optionalToken(tokenKind.COMMA, null, () => {
                            should_break = true;
                        });
                        if (should_break) { break; }
                    };
                    token = expectToken(tokenKind.RIGHT_PAREN, "expected ')' after last field");
                });
                expr.body.push(opt);
                let should_break;
                token = optionalToken(tokenKind.COMMA, null, () => {
                    should_break = true;
                });
                if (should_break) { break; }
            }
            token = expectToken(tokenKind.RIGHT_BRACE, "expected '}' after last option");
            break;
        case tokenKind.TAG:
            expr.kind = exprKind.TAG_DEF;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'tag' keyword", () => {
                expr.name = token.value;
            });
            token = expectToken(tokenKind.LEFT_PAREN, "expected '(' after 'tag' name");
            expr.args = [];
            while (1) {
                let should_return;
                token = optionalToken(tokenKind.RIGHT_PAREN, () => {
                    should_return = true;
                });
                if (should_return) {
                    token = expectToken(tokenKind.SEMICOLON, "expected ';' after tag args");
                    return;
                }
                let arg = {
                    kind: exprKind.ARG,
                    tags: parseTags()
                };
                token = expectToken(tokenKind.IDENT, "expected arg name in 'tag' args", () => {
                    arg.name = token.value;
                });
                token = expectToken(tokenKind.COLON, "expected ':' after arg name");
                arg.type = parseType();
                expr.args.push(arg);
                let should_break;
                token = optionalToken(tokenKind.COMMA, null, () => {
                    should_break = true;
                });
                if (should_break) { break; }
            }
            token = expectToken(tokenKind.RIGHT_PAREN, "expected ')' after last arg");
            token = expectToken(tokenKind.SEMICOLON, "expected ';' after tag args");
            break;
        case tokenKind.PROP:
            expr.kind = exprKind.PROP;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'prop' keyword", () => {
                expr.name = token.value;
            });
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'prop' name");
            expr.body = parseBlock();
            break;
        case tokenKind.DEF:
            expr.kind = exprKind.DEF;
            token = nextToken();
            expr.prop = parseType();
            token = expectToken(tokenKind.FOR, "expected 'for' after 'def' property");
            expr.target = parseType();
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'def' target");
            expr.body = parseBlock();
            break;
        case tokenKind.SUB:
            expr.kind = exprKind.SUB_DEF;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'sub' keyword", () => {
                expr.name = token.value;
            });
            token = expectToken(tokenKind.LEFT_PAREN, "expected '(' after 'sub' name");
            expr.args = [];
            args:
            do {
                while (1) {
                    let should_break_args;
                    token = optionalToken(tokenKind.RIGHT_PAREN, () => {
                        should_break_args = true;
                    });
                    if (should_break_args) {
                        break args;
                    }
                    let arg = {
                        kind: exprKind.ARG,
                        tags: parseTags()
                    };
                    token = expectToken(tokenKind.IDENT, "expected arg name in 'tag' args", () => {
                        arg.name = token.value;
                    });
                    token = expectToken(tokenKind.COLON, "expected ':' after arg name");
                    arg.type = parseType();
                    expr.args.push(arg);
                    let should_break;
                    token = optionalToken(tokenKind.COMMA, null, () => {
                        should_break = true;
                    });
                    if (should_break) { break; }
                }
                token = expectToken(tokenKind.RIGHT_PAREN, "expected ')' after last arg");
            } while (0);
            token = optionalToken(tokenKind.ARROW, () => {
                expr.ret_type = parseType();
            });
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'sub' args");
            expr.body = parseBlock(true);
            break;
        // case VAR:
        // expr.kind = E_VAR_DEF;
        // NEXT_TOKEN;
        // EXPECT_TOKEN(IDENT, "expected identifier after 'var' keyword", expr.u.var_def.name = token.value);
        // EXPECT_TOKEN(COLON, "expected ':' after variable name", {});
        // expr.u.var_def.type = parse_type(tokens, index);
        // token = tokens.array[*index];
        // expr.u.var_def.value = NULL;
        // EXPECT_TOKEN(SEMICOLON, "expected ';' after variable declaration", {});
        // break;
        default:
            throw `at ${token.index} expected item in global scope`;
    }

    return expr;
}

function parseBlock(local) {
    let exprs = [];

    while (index < tokens.length) {
        if (tokens[index].kind === tokenKind.RIGHT_BRACE) {
            ++index;
            return exprs;
        }

        exprs.push(local ? parseLocal() : parseGlobal());
    }

    return exprs;
}

module.exports.parse = function (_tokens) {
    tokens = _tokens;
    index = 0;
    return parseBlock();
};