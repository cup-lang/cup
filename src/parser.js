const tokenKind = require('./lexer.js').tokenKind;

const exprKind = module.exports.exprKind = {
    PATH_PART: 'path_part',
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

function parsePath(start) {
    let path = [];
    let need_colon = false;
    let i = start;
    while (1) {
        const token = tokens[i];
        if (need_colon) {
            if (token.kind === tokenKind.COLON) {
                need_colon = false;
            } else {
                return [path, i];
            }
        } else {
            if (token.kind === tokenKind.IDENT) {
                let part = {
                    kind: exprKind.PATH_PART,
                    name: token.value,
                    gens: [],
                };
                if (tokens[i + 1].kind === tokenKind.LESS) {
                    index = i + 1;
                    nextToken();
                    while (1) {
                        let should_break;
                        optionalToken(tokenKind.GREATER, () => {
                            should_break = true;
                        }, null);
                        if (should_break) { break; }
                        part.gens.push(parseType());
                        optionalToken(tokenKind.COMMA, null, null);
                    }
                    i = index - 1;
                }
                path.push(part);
                need_colon = true;
            } else if (path.length > 0) {
                throw "expected identifier after ':'";
            } else {
                return [path, i];
            }
        }
        ++i;
    }
}

function parseType() {
    if (tokens[index].kind === tokenKind.IDENT) {
        const [path, i] = parsePath(index);
        index = i;
        return {
            kind: exprKind.TYPE,
            path: path,
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

function parseLocal(endTokenKind, start = index, end) {
    let opLevel = 0;
    let opKind;
    let opIndex;
    let scopes = [0, 0, 0];
    let skipNextBlock = 0;

    let i = start - 1;
    w: while (1) {
        if (++i === end) {
            break;
        }

        const token = tokens[i];

        if (skipNextBlock > 0) {
            if (token.kind === tokenKind.LEFT_BRACE) {
                ++scopes[1];
            } else if (token.kind === tokenKind.RIGHT_BRACE) {
                --scopes[1];
                if (scopes[1] === 0) {
                    --skipNextBlock;
                }
            }
            continue;
        }

        if (token.kind === tokenKind.LEFT_PAREN) {
            ++scopes[0]; continue;
        } else if (token.kind === tokenKind.RIGHT_PAREN) {
            if (scopes[0]-- === 0) {
                throw "unexpected ')'";
            } continue;
        }

        if (endTokenKind !== tokenKind.LEFT_BRACE) {
            if (token.kind === tokenKind.LEFT_BRACE) {
                ++scopes[1]; continue;
            } else if (token.kind === tokenKind.RIGHT_BRACE) {
                if (scopes[1]-- === 0) {
                    throw "unexpected '}'";
                } continue;
            }
        }

        if (token.kind === tokenKind.LEFT_BRACKET) {
            ++scopes[2]; continue;
        } else if (endTokenKind !== tokenKind.RIGHT_BRACKET && token.kind === tokenKind.RIGHT_BRACKET) {
            if (scopes[2]-- === 0) {
                throw "unexpected ']'";
            } continue;
        }

        if (scopes[0] > 0 || scopes[1] > 0 || scopes[2] > 0) {
            continue;
        }

        if (token.kind === tokenKind.IF ||
            token.kind === tokenKind.ELIF ||
            token.kind === tokenKind.ELSE ||
            token.kind === tokenKind.LOOP ||
            token.kind === tokenKind.WHILE ||
            token.kind === tokenKind.FOR ||
            token.kind === tokenKind.EACH ||
            token.kind === tokenKind.MATCH) {
            ++skipNextBlock; continue;
        }

        if (token.kind === endTokenKind) {
            end = i;
            break w;
        }

        switch (token.kind) {
            case tokenKind.SEMICOLON:
                if (!end) {
                    end = i;
                    break w;
                }
                break;
            case tokenKind.ASSIGN:
            case tokenKind.DEREF_ASSIGN:
            case tokenKind.ADD_ASSIGN:
            case tokenKind.SUBTRACT_ASSIGN:
            case tokenKind.MULTIPLY_ASSIGN:
            case tokenKind.DIVIDE_ASSIGN:
            case tokenKind.MODULO_ASSIGN:
                if (opLevel < 5) {
                    opLevel = 5;
                    opIndex = i;
                    opKind = token.kind;
                }
                break;
            case tokenKind.LESS:
                if (opLevel != 4) {
                    let genericCount = 1;
                    let tryEnd = end ? end : tokens.length + 1;
                    f: for (let ii = i + 1; ii < tryEnd; ++ii) {
                        if (!end && tokens[ii].kind === tokenKind.SEMICOLON || tokens[ii].kind === endTokenKind) {
                            break;
                        }
                        switch (tokens[ii].kind) {
                            case tokenKind.LESS:
                                ++genericCount;
                                break;
                            case tokenKind.IDENT:
                            case tokenKind.COLON:
                            case tokenKind.COMMA:
                                break;
                            case tokenKind.GREATER:
                                if (--genericCount == 0) {
                                    i = ii;
                                    continue w;
                                }
                                break;
                            default:
                                break f;
                        }
                    }
                }
            case tokenKind.AND:
            case tokenKind.OR:
            case tokenKind.LESS_EQUAL:
            case tokenKind.GREATER:
            case tokenKind.GREATER_EQUAL:
            case tokenKind.EQUAL:
            case tokenKind.NOT_EQUAL:
                if (opLevel == 4) {
                    throw 'dont mix these (you mixed ' + token.kind + ' with ' + opKind + ')';
                }
                if (opLevel < 4) {
                    opLevel = 4;
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
        }
    }

    if (opLevel !== 0) {
        let expr = { type: opKind };
        if (opKind === tokenKind.NOT || opKind === tokenKind.DEREF || opKind === tokenKind.ADDRESS) {
            expr.kind = exprKind.UNARY_OP;
            expr.value = parseLocal(null, start, opIndex);
        } else {
            expr.kind = exprKind.BINARY_OP;
            expr.lhs = parseLocal(null, start, opIndex);
            if (expr.lhs.kind === exprKind.LOCAL_VAR_DEF) {
                index = end + 1;
                return expr.lhs;
            }
            // switch (opKind) {
            //     case tokenKind.ASSIGN:
            //     case tokenKind.ADD_ASSIGN:
            //     case tokenKind.SUBTRACT_ASSIGN:
            //     case tokenKind.MULTIPLY_ASSIGN:
            //     case tokenKind.DIVIDE_ASSIGN:
            //     case tokenKind.MODULO_ASSIGN:
            //         if (expr.lhs.kind !== exprKind.LOCAL_VAR_DEF && expr.lhs.kind !== exprKind.VAR_USE) {
            //             throw "expected a mutable variable";
            //         }
            //         break;
            // }
            expr.rhs = parseLocal(null, opIndex + 1, end);
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
            expr = parseLocal(null, start + 1, end - 1);
            index = end + 1;
            return expr;
        }

        switch (token.kind) {
            case tokenKind.IF:
                expr.kind = exprKind.IF;
                token = nextToken();
                expr.if = {
                    cond: parseLocal(tokenKind.LEFT_BRACE)
                };
                expr.if.body = parseBlock(true);
                expr.elif = [];
                while (1) {
                    token = tokens[index];
                    let should_break;
                    token = optionalToken(tokenKind.ELIF, () => {
                        let elif = {
                            cond: parseLocal(tokenKind.LEFT_BRACE)
                        };
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
                expr.cond = parseLocal(tokenKind.LEFT_BRACE);
                expr.body = parseBlock(true);
                break;
            case tokenKind.FOR:
                expr.kind = exprKind.FOR;
                token = nextToken();
                token = expectToken(tokenKind.IDENT, "expected 'for' iteration variable name", () => {
                    expr.loop_var = { name: tokens[index].value };
                });
                token = optionalToken(tokenKind.ASSIGN, () => {
                    expr.loop_var.value = parseLocal(tokenKind.COMMA);
                }, () => {
                    token = expectToken(tokenKind.COMMA, "expected ',' after iteration variable");
                });
                expr.cond = parseLocal(tokenKind.COMMA);
                expr.next = parseLocal(tokenKind.LEFT_BRACE);
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
                expr.iter = parseLocal(tokenKind.LEFT_BRACE);
                expr.body = parseBlock(true);
                token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'each' body");
                break;
            case tokenKind.MATCH:
                expr.kind = exprKind.MATCH;
                token = nextToken();
                expr.value = parseLocal(tokenKind.LEFT_BRACE);
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
                    _case.value = parseLocal(tokenKind.LEFT_BRACE);
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
                token = optionalToken(tokenKind.SEMICOLON, null, () => {
                    expr.value = parseLocal();
                });
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
                expr.value = parseLocal();
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
            case tokenKind.THIS:
                expr.kind = exprKind.THIS_LIT;
                break;
            case tokenKind.VAR:
                expr.kind = exprKind.LOCAL_VAR_DEF;
                token = nextToken();
                token = expectToken(tokenKind.IDENT, "expected identifier after 'var' keyword", () => {
                    expr.name = token.value;
                });
                token = optionalToken(tokenKind.SEMICOLON, null, () => {
                    token = expectToken(tokenKind.ASSIGN, "expected '=' or ';' after 'var' name");
                    expr.value = parseLocal(null);
                });
                break;
            case tokenKind.IDENT:
                let i = start + 1;
                [expr.path, i] = parsePath(start);
                if (i >= end) {
                    expr.kind = exprKind.VAR_USE;
                } else if (tokens[i].kind === tokenKind.LEFT_PAREN) {
                    expr.kind = exprKind.SUB_CALL;
                    expr.args = [];
                    const paren = nextOfKind(tokenKind.RIGHT_PAREN, ++i);
                    if (paren === undefined) {
                        throw "expected ')' after '('";
                    }
                    if (paren !== i) {
                        let comma = nextOfKind(tokenKind.COMMA, i + 1, paren);
                        while (comma !== undefined && comma + 1 < paren) {
                            expr.args.push(parseLocal(null, i, comma));
                            i = comma + 1;
                            comma = nextOfKind(tokenKind.COMMA, i, paren);
                        }
                        expr.args.push(parseLocal(null, i, paren));
                    }
                    i = paren + 1;
                } else if (tokens[i].kind === tokenKind.LEFT_BRACKET) {
                    index = i + 1;
                    let inside = parseLocal(tokenKind.RIGHT_BRACKET);
                    i = index;
                } else if (tokens[i].kind === tokenKind.LEFT_BRACE) {
                    expr.kind = exprKind.COMP_INST;
                    expr.args = [];
                    const brace = nextOfKind(tokenKind.RIGHT_BRACE, ++i);
                    if (brace === undefined) {
                        throw "expected '}' after '{'";
                    }
                    if (brace !== i) {
                        let comma = nextOfKind(tokenKind.COMMA, i + 1, brace);
                        index = i;
                        while (1) {
                            let arg = { kind: exprKind.FIELD_VAL };
                            token = expectToken(tokenKind.IDENT, "expected field name", () => {
                                arg.name = tokens[index].value;
                            });
                            token = expectToken(tokenKind.ASSIGN, "expected '=' after field name");
                            i = index;
                            arg.value = parseLocal(null, i, comma);
                            expr.args.push(arg);
                            if (comma == brace || comma + 1 == brace) {
                                break;
                            }
                            i = comma + 1;
                            comma = nextOfKind(tokenKind.COMMA, i, brace);
                            comma = comma ? comma : brace;
                        }
                    }
                    i = brace + 1;
                } else if (tokens[i].kind === tokenKind.IDENT) {
                    expr.kind = exprKind.LOCAL_VAR_DEF;
                    expr.name = tokens[i].value;
                    expr.type = {
                        kind: exprKind.TYPE,
                        path: expr.path,
                    };
                    delete expr.path;
                    index = i + 1;
                    token = optionalToken(tokenKind.SEMICOLON, null, () => {
                        token = expectToken(tokenKind.ASSIGN, "expected '=' or ';' after 'var' name");
                        expr.value = parseLocal(null);
                    });
                    i = index;
                } else {
                    expr.kind = exprKind.VAR_USE;
                }

                if (i < end) {
                    throw "expected operator or ';'";
                }
                break;
            default:
                console.log(tokens[start]);
                throw "expected a value";
        }
        index = end + 1;
        return expr;
    }
}

function parseGlobal() {
    let expr = {};
    expr.tags = parseTags();

    let token = tokens[index];

    end: switch (token.kind) {
        case tokenKind.USE:
            expr.kind = exprKind.USE;
            token = nextToken();
            expr.path = [];
            while (1) {
                token = expectToken(tokenKind.IDENT, "expected identifier after 'use' keyword", () => {
                    expr.path.push(token.value);
                });
                let should_break;
                token = optionalToken(tokenKind.SEMICOLON, () => {
                    should_break = true;
                });
                if (should_break) { break; }
                token = expectToken(tokenKind.COLON, "expected ':' or ';' after 'use' path");
            }
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
                expr.name = tokens[index].value;
            });
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'comp' name");
            expr.body = [];
            while (1) {
                let should_end;
                token = optionalToken(tokenKind.RIGHT_BRACE, () => {
                    token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'comp' body");
                    should_end = true;
                });
                if (should_end) { break end; }
                let field = {
                    kind: exprKind.FIELD,
                    tags: parseTags()
                };
                field.type = parseType();
                token = expectToken(tokenKind.IDENT, "expected field name in 'comp' body", () => {
                    field.name = tokens[index].value;
                });
                expr.body.push(field);
                let should_break;
                token = optionalToken(tokenKind.COMMA, null, () => {
                    should_break = true;
                });
                if (should_break) { break; }
            }
            token = expectToken(tokenKind.RIGHT_BRACE, "expected '}' after last field");
            token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'comp' body");
            break;
        case tokenKind.ENUM:
            expr.kind = exprKind.ENUM;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'enum' keyword", () => {
                expr.name = tokens[index].value;
            });
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'enum' name");
            expr.body = [];
            while (1) {
                let should_end;
                token = optionalToken(tokenKind.RIGHT_BRACE, () => {
                    token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'enum' body");
                    should_end = true;
                });
                if (should_end) { break end; }
                let opt = {
                    kind: exprKind.OPTION,
                    tags: parseTags(),
                    body: []
                };
                token = expectToken(tokenKind.IDENT, "expected option name in 'enum' body", () => {
                    opt.name = tokens[index].value;
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
                        field.type = parseType();
                        token = expectToken(tokenKind.IDENT, "expected field name in 'option' body", () => {
                            field.name = tokens[index].value;
                        });
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
            token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'enum' body");
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
                arg.type = parseType();
                token = expectToken(tokenKind.IDENT, "expected arg name in 'tag' args", () => {
                    arg.name = tokens[index].value;
                });
                expr.args.push(arg);
                let should_break;
                token = optionalToken(tokenKind.COMMA, null, () => {
                    should_break = true;
                });
                if (should_break) { break; }
            }
            token = expectToken(tokenKind.RIGHT_PAREN, "expected ')' after last arg");
            token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'tag' args");
            break;
        case tokenKind.PROP:
            expr.kind = exprKind.PROP;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'prop' keyword", () => {
                expr.name = tokens[index].value;
            });
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'prop' name");
            expr.body = parseBlock();
            token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'prop' body");
            break;
        case tokenKind.DEF:
            expr.kind = exprKind.DEF;
            token = nextToken();
            expr.prop = parseType();
            // token = expectToken(tokenKind.FOR, "expected 'for' after 'def' property");
            // expr.target = parseType();
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'def' target");
            expr.body = parseBlock();
            token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'def' body");
            break;
        case tokenKind.SUB:
            expr.kind = exprKind.SUB_DEF;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'sub' keyword", () => {
                expr.name = tokens[index].value;
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
                    arg.type = parseType();
                    token = expectToken(tokenKind.IDENT, "expected arg name in 'sub' args", () => {
                        arg.name = tokens[index].value;
                    });
                    expr.args.push(arg);
                    let should_break;
                    token = optionalToken(tokenKind.COMMA, null, () => {
                        should_break = true;
                    });
                    if (should_break) { break; }
                }
                token = expectToken(tokenKind.RIGHT_PAREN, "expected ')' after last arg");
            } while (0);
            token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'sub' args");
            expr.body = parseBlock(true);
            token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'sub' body");
            break;
        case tokenKind.VAR:
            expr.kind = exprKind.VAR_DEF;
            token = nextToken();
            token = expectToken(tokenKind.IDENT, "expected identifier after 'var' keyword", () => {
                expr.name = token.value;
            });
            token = optionalToken(tokenKind.SEMICOLON, null, () => {
                token = expectToken(tokenKind.ASSIGN, "expected '=' or ';' after 'var' name");
                expr.value = parseLocal(null);
            });
            break;
        case tokenKind.IDENT:
            let type = parseType();
            if (token = tokenKind.IDENT) {
                expr.name = tokens[index].value;
                token = nextToken();
                if (token.kind === tokenKind.ASSIGN || tokenKind.kind === tokenKind.SEMICOLON) {
                    expr.kind = exprKind.VAR_DEF;
                    expr.type = type;
                    if (token.kind === tokenKind.ASSIGN) {
                        token = nextToken();
                        expr.value = parseLocal(null);
                    }
                } else {
                    expr.kind = exprKind.SUB_DEF;
                    expr.retType = type;
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
                            arg.type = parseType();
                            token = expectToken(tokenKind.IDENT, "expected arg name in 'sub' args", () => {
                                arg.name = tokens[index].value;
                            });
                            expr.args.push(arg);
                            let should_break;
                            token = optionalToken(tokenKind.COMMA, null, () => {
                                should_break = true;
                            });
                            if (should_break) { break; }
                        }
                        token = expectToken(tokenKind.RIGHT_PAREN, "expected ')' after last arg");
                    } while (0);
                    token = expectToken(tokenKind.LEFT_BRACE, "expected '{' after 'sub' args");
                    expr.body = parseBlock(true);
                    token = expectToken(tokenKind.SEMICOLON, "expected ';' after 'sub' body");
                }
            } else {
                throw "expected an identficator after 'var'/'sub' type";
            }
            break;
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

        let expr;
        if (local) {
            let label = parseLabel(true);
            let tags = parseTags();
            expr = parseLocal(null);
            expr.tags = tags;
            if (label) {
                expr.label = label;
            }
        } else {
            expr = parseGlobal();
        }
        exprs.push(expr);
    }

    return exprs;
}

module.exports.parse = function (_tokens) {
    tokens = _tokens;
    index = 0;
    return parseBlock();
};