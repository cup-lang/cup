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
    NONE_LIT: 'none_lit',
    THIS_LIT: 'this_lit',
    TYPE_LIT: 'type_lit',
    WHERE: 'where',
    BLOCK: 'block',
    IF: 'if',
    ELIF: 'elif',
    ELSE: 'else',
    LOOP: 'loop',
    WHILE: 'while',
    FOR: 'for',
    EACH: 'each',
    MATCH: 'match',
    CASE: 'case',
    BACK: 'back',
    NEXT: 'next',
    JUMP: 'jump',

    UNARY_OP: 'unary_op',
    BINARY_OP: 'binary_op',
    TERNARY_OP: 'ternary_op',
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

function nextOfKind(kind, start = index, end = tokens.length) {
    for (let i = start; i < end; ++i) {
        if (tokens[i].kind === kind) {
            return i;
        }
    }
}

function parseValueRange(start, end) {
    let opLevel = 0;
    let opCount = 0;
    let opIndex;
    let parenCount = 0;
    let opKind;
    for (let i = start; i < end; ++i) {
        const token = tokens[i];
        switch (token.kind) {
            case tokenKind.LEFT_PAREN:
                ++parenCount;
                break;
            case tokenKind.RIGHT_PAREN:
                if (parenCount-- == 0) {
                    throw "unexpected ')'";
                }
                break;
        }

        if (parenCount > 0) {
            continue;
        }

        ++opCount;

        switch (token.kind) {
            case tokenKind.ASSIGN:
            case tokenKind.ADD_ASSIGN:
            case tokenKind.SUBTRACT_ASSIGN:
            case tokenKind.MULTIPLY_ASSIGN:
            case tokenKind.DIVIDE_ASSIGN:
            case tokenKind.MODULO_ASSIGN:
                if (opLevel < 6) {
                    opLevel = 6;
                }
                if (opLevel == 6) {
                    opKind = token.kind;
                    opIndex = i;
                }
                break;
            case tokenKind.EQUAL:
            case tokenKind.NOT_EQUAL:
                if (opLevel < 5) {
                    opLevel = 5;
                }
                if (opLevel == 5) {
                    opKind = token.kind;
                    opIndex = i;
                }
                break;
            case tokenKind.LESS:
            case tokenKind.LESS_EQUAL:
            case tokenKind.GREATER:
            case tokenKind.GREATER_EQUAL:
                if (opLevel < 4) {
                    opLevel = 4;
                }
                if (opLevel == 4) {
                    opKind = token.kind;
                    opIndex = i;
                }
                break;
            case tokenKind.ADD:
            case tokenKind.SUBTRACT:
                if (opLevel < 3) {
                    opLevel = 3;
                }
                if (opLevel == 3) {
                    opKind = token.kind;
                    opIndex = i;
                }
                break;
            case tokenKind.MULTIPLY:
            case tokenKind.DIVIDE:
            case tokenKind.MODULO:
                if (opLevel < 2) {
                    opLevel = 2;
                }
                if (opLevel == 2) {
                    opKind = token.kind;
                    opIndex = i;
                }
                break;
            case tokenKind.NOT:
                console.log('asdasdsd');
                if (opLevel == 0) {
                    opLevel = 1;
                }
                if (opLevel == 1) {
                    opKind = token.kind;
                    opIndex = i;
                }
                break;
            default:
                --opCount;
                break;
        }
    }

    if (opCount > 0) {
        let expr = {
            kind: exprKind.BINARY_OP,
            type: opKind,
            lhs: parseValueRange(start, opIndex),
        };
        switch (opKind) {
            case tokenKind.ASSIGN:
            case tokenKind.ADD_ASSIGN:
            case tokenKind.SUBTRACT_ASSIGN:
            case tokenKind.MULTIPLY_ASSIGN:
            case tokenKind.DIVIDE_ASSIGN:
            case tokenKind.MODULO_ASSIGN:
                if (expr.lhs.kind !== exprKind.VAR_USE) {
                    throw "expected a mutable variable";
                }
                break;
        }
        expr.rhs = parseValueRange(opIndex + 1, end);
        return expr;
    } else {
        let expr = {};
        const token = tokens[start];

        if (token.kind === tokenKind.LEFT_PAREN) {
            if (tokens[end - 1].kind !== tokenKind.RIGHT_PAREN) {
                throw "expected ')' after '('";
            }
            return parseValueRange(start + 1, end - 1);
        }

        if (token.kind !== tokenKind.IDENT &&
            token.kind !== tokenKind.STRING_LIT &&
            token.kind !== tokenKind.CHAR_LIT &&
            token.kind !== tokenKind.INT_LIT &&
            token.kind !== tokenKind.FLOAT_LIT &&
            token.kind !== tokenKind.TRUE && 
            token.kind !== tokenKind.FALSE && 
            token.kind !== tokenKind.NONE) {
            throw "expected identifier or value";
        }

        switch (tokens[start + 1].kind) {
            case tokenKind.LEFT_PAREN:
                expr.kind = exprKind.SUB_CALL;
                expr.name = token.value;
                expr.args = [];
                const paren = nextOfKind(tokenKind.RIGHT_PAREN, start + 2);
                if (paren === undefined) {
                    throw "expected ')' after '('";
                }
                if (paren !== start + 2) {
                    expr.args = [];
                    let comma = nextOfKind(tokenKind.COMMA, start + 2, paren);
                    let index = start + 2;
                    while (comma !== undefined && comma + 1 < paren) {
                        expr.args.push(parseValueRange(index, comma));
                        index = comma + 1;
                        comma = nextOfKind(tokenKind.COMMA, index, paren);
                    }
                    expr.args.push(parseValueRange(index, paren));
                }
                return expr;
            default:
                switch (token.kind) {
                    case tokenKind.STRING_LIT:
                        expr.kind = exprKind.STRING_LIT;
                        expr.value = token.value;
                        break;
                    case tokenKind.CHAR_LIT:
                        expr.kind = exprKind.CHAR_LIT;
                        expr.value = token.value;
                        break;
                    case tokenKind.INT_LIT:
                        expr.kind = exprKind.INT_LIT;
                        expr.value = token.value;
                        break;
                    case tokenKind.FLOAT_LIT:
                        expr.kind = exprKind.FLOAT_LIT;
                        expr.value = token.value;
                        break;
                    case tokenKind.TRUE:
                    case tokenKind.FALSE:
                        expr.kind = exprKind.BOOL_LIT;
                        expr.value = token.kind === tokenKind.TRUE;
                        break;
                    case tokenKind.NONE:
                        expr.kind = exprKind.NONE_LIT;
                        break;
                    default:
                        expr.kind = exprKind.VAR_USE;
                        expr.name = token.value;
                        break;
                }
                return expr;
        }
    }
}

function parseValue(endTokenKind) {
    const end = nextOfKind(endTokenKind);
    let expr = parseValueRange(index, end);
    index = end;
    return expr;
}

function parseLocal() {
    let expr = {};
    expr.tags = parseTags();

    let token = tokens[index];

    switch (token.kind) {
        case tokenKind.IF:
            expr.kind = exprKind.IF;
            token = nextToken();
            expr.if = {
                cond: parseValue(tokenKind.LEFT_BRACE)
            };
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'if' condition");
            expr.if.body = parseBlock(true);
            expr.elif = [];
            while (1) {
                token = tokens[index];
                let should_break;
                token = optionalToken(tokenKind.ELIF, () => {
                    let elif = {
                        cond: parseValue(tokenKind.LEFT_BRACE)
                    };
                    token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'elif' condition");
                    elif.body = parseBlock(true);
                    expr.elif.push(elif);
                }, () => {
                    should_break = true;
                });
                if (should_break) { break; }
            }
            token = optionalToken(tokenKind.ELSE, () => {
                token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'else' condition");
                expr.else = parseBlock(true);
            });
            break;
        case tokenKind.LOOP:
            expr.kind = exprKind.LOOP;
            token = nextToken();
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'do' keyword");
            expr.body = parseBlock(true);
            break;
        case tokenKind.WHILE:
            expr.kind = exprKind.WHILE;
            token = nextToken();
            expr.cond = parseValue(tokenKind.LEFT_BRACE);
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'while' condition");
            expr.body = parseBlock(true);
            break;
        case tokenKind.FOR:
            expr.kind = exprKind.FOR;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected 'for' iteration variable name", () => {
                expr.loop_var = token.value;
            });
            token = expectToken(tokenKind.COMMA, "expected ',' after iteration variable name");
            expr.cond = parseValue(tokenKind.COMMA);
            token = expectToken(tokenKind.COMMA, "expected ',' after 'for' condition");
            expr.next = parseValue(tokenKind.LEFT_BRACE);
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'for' next");
            expr.body = parseBlock(true);
            break;
        case tokenKind.EACH:
            expr.kind = exprKind.EACH;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected 'each' iteration variable name", () => {
                expr.loop_var = token.value;
            });
            token = expectToken(tokenKind.COMMA, "expected ',' after iteration variable name");
            expr.iter = parseValue(tokenKind.LEFT_BRACE);
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'each' iterator");
            expr.body = parseBlock(true);
            break;
        case tokenKind.MATCH:
            break;
        case tokenKind.BACK:
            break;
        case tokenKind.NEXT:
            break;
        case tokenKind.JUMP:
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