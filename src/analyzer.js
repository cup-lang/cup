const exprKind = require('./parser.js').exprKind;

let genExprs = {};
let mods;

function analyzeExpr(expr) {
    switch (expr.kind) {
        case exprKind.MOD:
        case exprKind.COMP:
        case exprKind.ENUM:
        case exprKind.PROP:
        case exprKind.DEF:
        case exprKind.SUB_DEF:
            if (expr.tags) {
                for (let i = 0; i < expr.tags.length; ++i) {
                    const tag = expr.tags[i];
                    if (tag.name === 'gen') {
                        const name = mods.concat(expr.name);
                        genExprs[name.join('_')] = expr;
                    }
                }
            }
            mods.push(expr.name);
            analyzeBlock(expr.body);
            mods.pop();
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
    analyzeBlock(ast);
}

module.exports.genExprs = genExprs;

// function deepEqual(a, b) {
//     if (typeof a == 'object' && a != null && typeof b == 'object' && b != null) {
//         var count = [0, 0];
//         for (var key in a) count[0]++;
//         for (var key in b) count[1]++;
//         if (count[0] - count[1] != 0) {
//             return false;
//         }
//         for (var key in a) {
//             if (!(key in b) || !deepEqual(a[key], b[key])) {
//                 return false;
//             }
//         }
//         for (var key in b) {
//             if (!(key in a) || !deepEqual(b[key], a[key])) {
//                 return false;
//             }
//         }
//         return true;
//     }
//     return a === b;
// }

// function registerGenericUse(type) {
//     if (!type) return;

//     let hasGenerics = false;
//     for (let i = 0; i < type.path.length; ++i) {
//         if (type.path[i].gens.length > 0) {
//             hasGenerics = true;
//             break;
//         }
//     }
//     if (hasGenerics) {
//         let name = type.path.map(p => p.name).join('_');
//         if (!module.exports.gens[name]) {
//             module.exports.gens[name] = [type.path.map(p => p.gens)];
//         }
//         else {
//             let gen = module.exports.gens[name];
//             let exists;
//             let typeGens = type.path.map(p => p.gens);
//             for (let i = 0; i < gen.length; ++i) {
//                 if (deepEqual(gen[i], typeGens)) {
//                     exists = true;
//                     break;
//                 }
//             }
//             if (!exists) {
//                 module.exports.gens[name].push(typeGens);
//             }
//         }
//     }
// }