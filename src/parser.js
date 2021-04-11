const tokenKind = require('./lexer.js').tokenKind;

const exprKind = module.exports.exprKind = {
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
    RET: 'ret',
    NEXT: 'next',
    JUMP: 'jump',
    TRY: 'try',

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
        let token = nextToken();
        if (token.kind === tokenKind.IDENT) {
            let tag = {
                kind: exprKind.TAG,
                name: token.value,
                args: []
            };

            token = nextToken();
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

function parseLabel(optional) {
    let label;
    if (optional) {
        optionalToken(tokenKind.TILDE, () => {
            label = tokens[index].value;
            nextToken();
        });
    } else {
        expectToken(tokenKind.TILDE, "expected a label", () => {
            nextToken();
            label = tokens[index].value;
        });
    }
    return label;
}

function parseGenerics() {
    let gens = [];
    if (tokens[index].kind === tokenKind.LESS) {
        nextToken();
        while (1) {
            let should_break;
            optionalToken(tokenKind.GREATER, () => {
                should_break = true;
            }, null);
            if (should_break) { break; }
            expectToken(tokenKind.IDENT, "expected a ident for a generic thing", () => {
                gens.push({
                    kind: exprKind.CONSTR_TYPE,
                    name: tokens[index].value,
                });
            });
            optionalToken(tokenKind.COMMA, null, null);
        }
    }
    return gens;
}

function parsePath(start) {
    let path = [];
    let gens = [];
    let need_colon = false;
    let i = start;
    while (1) {
        const token = tokens[i];
        if (need_colon) {
            if (token.kind === tokenKind.DOUBLE_COLON) {
                need_colon = false;
            } else if (token.kind === tokenKind.LESS) {
                index = i;
                nextToken();
                while (1) {
                    let should_break;
                    optionalToken(tokenKind.GREATER, () => {
                        should_break = true;
                    }, null);
                    if (should_break) { break; }
                    gens.push(parseType());
                    optionalToken(tokenKind.COMMA, null, null);
                }
                i = index;
                return [path, gens, i];
            }
            else {
                return [path, gens, i];
            }
        } else {
            if (token.kind === tokenKind.IDENT) {
                path.push(token.value);
                need_colon = true;
            } else if (path.length) {
                throw "expected identifier after '::'";
            } else {
                return [path, gens, i];
            }
        }
        ++i;
    }
}

function parseType() {
    if (tokens[index].kind === tokenKind.IDENT) {
        const [path, gens, i] = parsePath(index);
        index = i;
        return {
            kind: exprKind.TYPE,
            path: path,
            gens: gens,
        };
    } else {
        throw `at ${tokens[index].index} expected a type`;
    }
}

function nextOfKind(target, start = index, end = tokens.length) {
    let scopes = [0, 0, 0];
    for (let i = start; i < end; ++i) {
        const kind = tokens[i].kind;

        if (kind === target && scopes[0] === 0 && scopes[1] === 0 && scopes[2] === 0) {
            return i;
        }

        if (kind === tokenKind.LEFT_PAREN) {
            ++scopes[0]; continue;
        } else if (kind === tokenKind.RIGHT_PAREN) {
            --scopes[0]; continue;
        }

        if (kind === tokenKind.LEFT_BRACE) {
            ++scopes[1]; continue;
        } else if (kind === tokenKind.RIGHT_BRACE) {
            --scopes[1]; continue;
        }

        if (kind === tokenKind.LEFT_BRACKET) {
            ++scopes[2]; continue;
        } else if (kind === tokenKind.RIGHT_BRACKET) {
            --scopes[2]; continue;
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
            case tokenKind.DEREF_ASSIGN:
            case tokenKind.ADD_ASSIGN:
            case tokenKind.SUBTRACT_ASSIGN:
            case tokenKind.MULTIPLY_ASSIGN:
            case tokenKind.DIVIDE_ASSIGN:
            case tokenKind.MODULO_ASSIGN:
                if (opLevel < 7) {
                    opLevel = 7;
                }
                if (opLevel == 7) {
                    opKind = token.kind;
                    opIndex = i;
                }
                break;
            case tokenKind.AND:
            case tokenKind.OR:
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
            case tokenKind.DOT:
            case tokenKind.NOT:
            case tokenKind.DEREF:
            case tokenKind.ADDRESS:
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
        let expr = { type: opKind };
        if (opKind === tokenKind.NOT || opKind === tokenKind.DEREF || opKind === tokenKind.ADDRESS) {
            expr.kind = exprKind.UNARY_OP;
            expr.value = parseValueRange(start, opIndex);
        } else {
            expr.kind = exprKind.BINARY_OP;
            expr.lhs = parseValueRange(start, opIndex);
            switch (opKind) {
                case tokenKind.ASSIGN:
                case tokenKind.ADD_ASSIGN:
                case tokenKind.SUBTRACT_ASSIGN:
                case tokenKind.MULTIPLY_ASSIGN:
                case tokenKind.DIVIDE_ASSIGN:
                case tokenKind.MODULO_ASSIGN:
                    if (expr.lhs.kind !== exprKind.VAR_USE && expr.lhs.type !== tokenKind.COLON) {
                        throw "expected a mutable variable";
                    }
                    break;
            }
            expr.rhs = parseValueRange(opIndex + 1, end);
        }
        return expr;
    } else {
        if (start === end) {
            throw `at ${tokens[start].index} expected at least something`;
        }

        let expr = {};
        let token = tokens[index = start];

        if (token.kind === tokenKind.LEFT_PAREN) {
            if (tokens[end - 1].kind !== tokenKind.RIGHT_PAREN) {
                throw "expected ')' after '('";
            }
            return parseValueRange(start + 1, end - 1);
        }

        switch (token.kind) {
            case tokenKind.IF:
                expr.kind = exprKind.IF;
                token = nextToken();
                expr.if = {
                    cond: parseValue(tokenKind.LEFT_BRACE)
                };
                token = nextToken();
                expr.if.body = parseBlock(true);
                expr.elif = [];
                while (1) {
                    token = tokens[index];
                    let should_break;
                    token = optionalToken(tokenKind.ELIF, () => {
                        let elif = {
                            cond: parseValue(tokenKind.LEFT_BRACE)
                        };
                        token = nextToken();
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
                token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'loop' body");
                break;
            case tokenKind.WHILE:
                expr.kind = exprKind.WHILE;
                token = nextToken();
                expr.cond = parseValue(tokenKind.LEFT_BRACE);
                token = nextToken();
                expr.body = parseBlock(true);
                token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'while' body");
                break;
            case tokenKind.FOR:
                expr.kind = exprKind.FOR;
                token = nextToken();
                token = expectToken(tokenKind.IDENT, "expected 'for' iteration variable name", () => {
                    expr.loop_var = token.value;
                });
                token = expectToken(tokenKind.COMMA, "expected ',' after iteration variable name");
                expr.cond = parseValue(tokenKind.COMMA);
                token = nextToken();
                expr.next = parseValue(tokenKind.LEFT_BRACE);
                token = nextToken();
                expr.body = parseBlock(true);
                token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'for' body");
                break;
            case tokenKind.EACH:
                expr.kind = exprKind.EACH;
                token = nextToken();
                token = expectToken(tokenKind.IDENT, "expected 'each' iteration variable name", () => {
                    expr.loop_var = token.value;
                });
                token = expectToken(tokenKind.COMMA, "expected ',' after iteration variable name");
                expr.iter = parseValue(tokenKind.LEFT_BRACE);
                token = nextToken();
                expr.body = parseBlock(true);
                token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'each' body");
                break;
            case tokenKind.MATCH:
                expr.kind = exprKind.MATCH;
                token = nextToken();
                expr.value = parseValue(tokenKind.LEFT_BRACE);
                token = nextToken();
                expr.body = [];
                while (1) {
                    let should_end;
                    token = optionalToken(tokenKind.RIGHT_BRACE, () => {
                        should_end = true;
                    });
                    if (should_end) { break; }
                    let _case = {
                        kind: exprKind.CASE,
                        tags: parseTags()
                    };
                    _case.value = parseValue(tokenKind.LEFT_BRACE);
                    token = nextToken();
                    _case.body = parseBlock(true);
                    expr.body.push(_case);
                    let should_break;
                    token = optionalToken(tokenKind.COMMA, null, () => {
                        should_break = true;
                    });
                    if (should_break) { break; }
                }
                token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'match' body");
                break;
            case tokenKind.RET:
                expr.kind = exprKind.RET;
                token = nextToken();
                expr.target = parseLabel(true);
                var semicolon = nextOfKind(tokenKind.SEMICOLON);
                if (semicolon === undefined) {
                    throw "expected ';' after 'ret' value"
                }
                expr.value = parseValueRange(index, semicolon);
                index = semicolon;
                break;
            case tokenKind.NEXT:
                expr.kind = exprKind.NEXT;
                token = nextToken();
                expr.target = parseLabel();
                token = expectToken(tokenKind.SEMICOLON, "expected a ';' after 'next' label");
                break;
            case tokenKind.JUMP:
                expr.kind = exprKind.JUMP;
                token = nextToken();
                expr.target = parseLabel();
                token = expectToken(tokenKind.SEMICOLON, "expected a ';' after 'jump' label");
                break;
            case tokenKind.TRY:
                expr.kind = exprKind.TRY;
                token = nextToken();
                expr.target = parseLabel(true);
                var semicolon = nextOfKind(tokenKind.SEMICOLON);
                if (semicolon === undefined) {
                    throw "expected ';' after 'try' value"
                }
                expr.value = parseValueRange(index, semicolon);
                index = semicolon;
                token = nextToken();
                break;

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
            case tokenKind.IDENT:
                let i = start + 1;
                [expr.path, expr.gens, i] = parsePath(start);
                if (tokens[i].kind === tokenKind.LEFT_PAREN) {
                    expr.kind = exprKind.SUB_CALL;
                    expr.args = [];
                    const paren = nextOfKind(tokenKind.RIGHT_PAREN, ++i);
                    if (paren === undefined) {
                        throw "expected ')' after '('";
                    }
                    if (paren !== i) {
                        expr.args = [];
                        let comma = nextOfKind(tokenKind.COMMA, i + 1, paren);
                        while (comma !== undefined && comma + 1 < paren) {
                            expr.args.push(parseValueRange(i, comma));
                            i = comma + 1;
                            comma = nextOfKind(tokenKind.COMMA, i, paren);
                        }
                        expr.args.push(parseValueRange(i, paren));
                    }
                    i = paren + 1;
                } else {
                    expr.kind = exprKind.VAR_USE;
                }

                if (i < end) {
                    throw "expected operator or ';'";
                }
                break;
            default:
                throw "expected identifier or value";
        }

        return expr;
    }
}

function parseValue(endTokenKind) {
    const end = nextOfKind(endTokenKind);
    if (end === undefined) {
        throw "expected endToken at the end";
    }
    let expr = parseValueRange(index, end);
    index = end;
    return expr;
}

function parseLocal() {
    let expr = {};
    if (tokens[index + 1].kind === tokenKind.COLON || tokens[index + 1].kind === tokenKind.WALRUS) {
        expr.kind = exprKind.LOCAL_VAR_DEF;
        let token = tokens[index];
        expr.name = token.value;
        token = nextToken();
        switch (token.kind) {
            case tokenKind.COLON:
                token = nextToken();
                expr.type = parseType();
                token = optionalToken(tokenKind.ASSIGN, () => {
                    expr.value = parseValue(tokenKind.SEMICOLON);
                });
                break;
            case tokenKind.WALRUS:
                token = nextToken();
                expr.value = parseValue(tokenKind.SEMICOLON);
                break;
        }
        token = nextToken();
        return expr;
    }

    expr = parseValue(tokenKind.SEMICOLON);
    nextToken();
    return expr;
}

function parseGlobal() {
    let expr = {};
    expr.tags = parseTags();

    let token = tokens[index];

    end: switch (token.kind) {
        case tokenKind.USE:
            expr.kind = exprKind.USE;
            token = nextToken();
            [expr.path, expr.gens, index] = parsePath(index);
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
            token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'mod' body");
            break;
        case tokenKind.COMP:
            expr.kind = exprKind.COMP;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'comp' keyword", () => {
                expr.name = token.value;
            });
            expr.gen = parseGenerics();
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
            expr.gen = parseGenerics();
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
            break;
        case tokenKind.PROP:
            expr.kind = exprKind.PROP;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'prop' keyword", () => {
                expr.name = token.value;
            });
            expr.gen = parseGenerics();
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
            expr.gen = parseGenerics();
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
            let parse_block;
            token = optionalToken(tokenKind.ARROW, () => {
                expr.retTypes = [];
                while (1) {
                    expr.retTypes.push(parseType());
                    if (tokens[index].kind === tokenKind.COMMA) {
                        token = nextToken();
                        if (token.kind === tokenKind.LEFT_BRACE) {
                            parse_block = true;
                            break;
                        }
                        continue;
                    } else if (tokens[index].kind === tokenKind.LEFT_BRACE) {
                        parse_block = true;
                        break;
                    } else {
                        throw "expected '{' after 'sub' back types";
                    }
                }
            });
            if (parse_block === undefined) {
                token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'sub' args");
            } else {
                token = nextToken();
            }
            expr.body = parseBlock(true);
            break;
        case tokenKind.IDENT:
            expr.kind = exprKind.VAR_DEF;
            expr.name = token.value;
            token = nextToken();
            const hasType = token.kind === tokenKind.COLON;
            if (token.kind === tokenKind.COLON || token.kind === tokenKind.WALRUS) {
                token = nextToken();
                if (hasType) {
                    expr.type = parseType();
                    token = optionalToken(tokenKind.ASSIGN, () => {
                        expr.value = parseValue(tokenKind.SEMICOLON);
                    });
                } else {
                    expr.value = parseValue(tokenKind.SEMICOLON);
                }
                token = nextToken();
                break;
            }
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