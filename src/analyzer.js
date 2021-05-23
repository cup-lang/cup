const exprKind = require('./parser.js').exprKind;

let genExprs = {};
let enumExprs = {};
let mods;
let gens;

function analyzeExpr(expr) {
    switch (expr.kind) {
        case exprKind.MOD:
        case exprKind.COMP:
        case exprKind.ENUM:
        case exprKind.PROP:
        case exprKind.DEF:
        case exprKind.SUB_DEF:
            const oldGens = gens.length;
            expr.gen = gens.concat(expr.gen || []);
            if (expr.tags) {
                for (let i = 0; i < expr.tags.length; ++i) {
                    const tag = expr.tags[i];
                    if (tag.name === 'gen') {
                        expr.gen.push(tag.args[0].value);
                        gens.push(tag.args[0].value);
                    }
                }
            }
            const name = mods.concat(expr.name);
            if (expr.gen.length > 0 &&
                (expr.kind === exprKind.COMP ||
                    expr.kind === exprKind.ENUM ||
                    expr.kind === exprKind.SUB_DEF)) {
                genExprs[name.join('_')] = expr;
                expr.path = name;
            } else if (expr.kind === exprKind.ENUM) {
                enumExprs[name.join('_')] = expr;
            }
            const oldMods = mods.length;
            if (expr.kind === exprKind.DEF) {
                for (let i = 0; i < expr.prop.path.length; ++i) {
                    mods.push(expr.prop.path[i].name);
                }
            } else {
                mods.push(expr.name);
            }
            analyzeBlock(expr.body);
            mods = mods.slice(0, oldMods);
            gens = gens.slice(0, oldGens);
            break;
    }
}

function analyzeBlock(exprs) {
    for (let i = 0; i < exprs.length; ++i) {
        analyzeExpr(exprs[i]);
    }
}

module.exports.analyze = function (ast) {
    mods = [];
    gens = [];
    analyzeBlock(ast);
}

module.exports.genExprs = genExprs;
module.exports.enumExprs = enumExprs;