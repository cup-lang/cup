const exprKind = require('./parser.js').exprKind;

module.exports.gens = {};

function deepEqual(a, b) {
    if (typeof a == 'object' && a != null && typeof b == 'object' && b != null) {
        var count = [0, 0];
        for (var key in a) count[0]++;
        for (var key in b) count[1]++;
        if (count[0] - count[1] != 0) {
            return false;
        }
        for (var key in a) {
            if (!(key in b) || !deepEqual(a[key], b[key])) {
                return false;
            }
        }
        for (var key in b) {
            if (!(key in a) || !deepEqual(b[key], a[key])) {
                return false;
            }
        }
        return true;
    }
    return a === b;
}

function registerGenericUse(type) {
    if (!type) return;

    let hasGenerics = false;
    for (let i = 0; i < type.path.length; ++i) {
        if (type.path[i].gens.length > 0) {
            hasGenerics = true;
            break;
        }
    }
    if (hasGenerics) {
        let name = type.path.map(p => p.name).join('_');
        if (!module.exports.gens[name]) {
            module.exports.gens[name] = [type.path.map(p => p.gens)];
        }
        else {
            let gen = module.exports.gens[name];
            let exists;
            let typeGens = type.path.map(p => p.gens);
            for (let i = 0; i < gen.length; ++i) {
                if (deepEqual(gen[i], typeGens)) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                module.exports.gens[name].push(typeGens);
            }
        }
    }
}

function analyzeExpr(expr) {
    switch (expr.kind) {
        case exprKind.USE:
            break;
        case exprKind.MOD:
            analyzeBlock(expr.body);
            break;
        case exprKind.COMP:
            analyzeBlock(expr.body);
            break;
        case exprKind.ENUM:
            analyzeBlock(expr.body);
            break;
        case exprKind.OPTION:
            analyzeBlock(expr.body);
            break;
        case exprKind.OPTION_FIELD:
            break;
        case exprKind.PROP:
            break;
        case exprKind.DEF:
            break;
        case exprKind.SUB_DEF:
            registerGenericUse(expr.retType);
            analyzeBlock(expr.args);
            analyzeBlock(expr.body);
            break;
        case exprKind.FIELD:
        case exprKind.ARG:
            registerGenericUse(expr.type);
            break;
        case exprKind.VAR_DEF:
        case exprKind.LOCAL_VAR_DEF:
            registerGenericUse(expr.type);
            if (expr.value) {
                analyzeExpr(expr.value);
            }
            break;
        case exprKind.SUB_CALL:
            registerGenericUse(expr);
            analyzeBlock(expr.args);
            break;
        case exprKind.VAR_USE:
            break;
        case exprKind.STRING_LIT:
            break;
        case exprKind.ARRAY_LIT:
            break;
        case exprKind.INT_LIT:
        case exprKind.FLOAT_LIT:
            break;
        case exprKind.BOOL_LIT:
            break;
        case exprKind.CHAR_LIT:
            break;
        case exprKind.NULL_LIT:
            break;
        case exprKind.SELF_LIT:
            break;
        case exprKind.BLOCK:
            analyzeBlock(expr.body);
            break;
        case exprKind.LOOP:
            analyzeBlock(expr.body);
            break;
        case exprKind.WHILE:
            analyzeExpr(expr.cond);
            analyzeBlock(expr.body);
            break;
        case exprKind.IF:
            analyzeExpr(expr.if.cond);
            analyzeBlock(expr.if.body);
            for (let i = 0; i < expr.elif.length; ++i) {
                analyzeExpr(expr.elif[i].cond);
                analyzeBlock(expr.elif[i].body);
            }
            if (expr.else) {
                analyzeBlock(expr.else);
            }
            break;
        case exprKind.FOR:
            analyzeExpr(expr.cond);
            analyzeExpr(expr.next);
            analyzeBlock(expr.body);
            break;
        case exprKind.EACH:
            break;
        case exprKind.MATCH:
            break;
        case exprKind.NEXT:
            break;
        case exprKind.JUMP:
            break;
        case exprKind.RET:
            if (expr.value) {
                analyzeExpr(expr.value);
            }
            break;
        case exprKind.TRY:
            break;
        case exprKind.UNARY_OP:
            analyzeExpr(expr.value);
            break;
        case exprKind.BINARY_OP:
            break;
    }
}

function analyzeBlock(exprs) {
    for (let i = 0; i < exprs.length; ++i) {
        analyzeExpr(exprs[i]);
    }
}

module.exports.analyze = function (ast) {
    analyzeBlock(ast);
    return ast;
}