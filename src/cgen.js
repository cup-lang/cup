const tokenKind = require('./lexer.js').tokenKind;
const exprKind = require('./parser.js').exprKind;
const genExprs = require('./analyzer.js').genExprs;
const enumExprs = require('./analyzer.js').enumExprs;
const binds = require('./analyzer.js').binds;

let types;
let output;
let mods;
let genNames;
let reqs;
let scopeGens;
let vars;
let typeHeaders;
let headers;

function applyGeneric(type) {
    type = JSON.parse(JSON.stringify(type));
    for (let i = 0; i < type.path.length; ++i) {
        const p = type.path[i];
        if (p.gens.length > 0) {
            for (let ii = 0; ii < p.gens.length; ++ii) {
                p.gens[ii] = applyGeneric(p.gens[ii]);
            }
        } else {
            const gen = genNames[type.path.map(p => p.name).join('_')];
            if (gen) {
                return gen;
            }
        }
    }
    return type;
}

function generateType(type, raw) {
    if (type === undefined) {
        return 'void';
    }

    let path = type.path[0].name;
    let pointerCount = 0;
    while (type.path.length === 1 && path === 'ptr') {
        type = type.path[0].gens[0];
        path = type.path[0].name;
        ++pointerCount;
    }

    type = applyGeneric(type);
    path = type.path[0].name;

    let out = '';

    generateGeneric(type.path.map(p => p.name).join('_'), type.path.map(p => p.gens));

    nothing = true;
    if (!raw) {
        nothing = false;
        if (path === 'int' || path === 'i32') {
            out += 'int32_t';
        }
        else if (path === 'f32') {
            out += 'float';
        }
        else if (path === 'u8' || path === 'bool' || path === 'b8') {
            out += 'uint8_t';
        }
        else if (path === 'i8') {
            out += 'int8_t';
        }
        else if (path === 'float' || path === 'f64') {
            out += 'double';
        }
        else if (path === 'int' || path === 'u32') {
            out += 'uint32_t';
        }
        else if (path === 'i64') {
            out += 'int64_t';
        }
        else if (path === 'i16') {
            out += 'int16_t';
        }
        else if (path === 'u64') {
            out += 'uint64_t';
        }
        else if (path === 'u16') {
            out += 'uint16_t';
        }
        else {
            nothing = true;
        }
    }

    if (nothing) {
        for (let i = 0; i < type.path.length; ++i) {
            if (i != 0) {
                out += '_';
            }
            out += type.path[i].name;
            for (let ii = 0; ii < type.path[i].gens.length; ++ii) {
                out += '_';
                out += generateType(type.path[i].gens[ii], true);
            }
        }
    }

    for (let i = 0; i < pointerCount; ++i) {
        out += '*';
    }

    return out;
}

function generateGeneric(path, gens) {
    let expr = genExprs[path];
    if (!expr) {
        return;
    }
    const oldGenNames = Object.assign({}, genNames);
    let gensList = [];
    for (let i = 0; i < gens.length; ++i) {
        gensList = gensList.concat(gens[i]);
    }
    for (let i = 0; i < expr.gen.length; ++i) {
        genNames[expr.gen[i]] = gensList[i];
    }
    let out;
    switch (expr.kind) {
        case exprKind.COMP:
            out = generateComp(expr, gens);
            break;
        case exprKind.ENUM:
            out = generateEnum(expr, gens);
            break;
        case exprKind.SUB_DEF:
            out = generateSub(expr, gens);
            break;
        case exprKind.COMP_INST:
            out = generateCompInst(expr, gens);
            break;
    }
    if (expr.kind === exprKind.COMP || expr.kind === exprKind.ENUM) {
        types += out;
    } else {
        output += out;
    }
    genNames = oldGenNames;
}

function generateGenericNamePart(type) {
    let name = '';
    for (let i = 0; i < type.path.length; ++i) {
        if (i != 0) {
            name += '_';
        }
        name += type.path[i].name;
        for (let ii = 0; ii < type.path[i].gens.length; ++ii) {
            name += '_';
            name += generateGenericNamePart(type.path[i].gens[ii]);
        }
    }
    return name;
}

function generateGenericName(name, gens) {
    let out = '';
    for (let i = 0; i < name.length; ++i) {
        if (i != 0) { out += '_'; }
        out += name[i];
        if (gens && gens[i]) {
            for (let ii = 0; ii < gens[i].length; ++ii) {
                out += '_';
                let g = genNames[gens[i][ii].path.map(p => p.name).join('_')];
                out += generateGenericNamePart(g ? g : gens[i][ii]);
            }
        }
    }
    return out;
}

function generateComp(expr, gen) {
    let out = '';
    let name = mods.concat(expr.name);
    if (gen) {
        name = expr.path;
    }
    let gname = generateGenericName(name, gen);
    let header = `typedef struct ${gname} ${gname};`;
    if (typeHeaders.indexOf(header) === -1) {
        typeHeaders.push(header);
        out += `struct ${gname}{`;
        out += generateBlock(expr.body, 1, 0) + '};';
    }
    return out;
}

function generateEnum(expr, gen) {
    let out = '';
    let name = mods.concat(expr.name);
    if (gen) {
        name = expr.path;
    }
    let gname = generateGenericName(name, gen);
    typeHeaders.push(`typedef struct ${gname} ${gname};`);
    mods.push(expr.name);
    out += generateBlock(expr.body, 1, 0);
    mods.pop();
    out += `union ${gname}Union{`;
    for (let i = 0; i < expr.body.length; ++i) {
        if (expr.body[i].body.length > 0) {
            out += `${gname + '_' + expr.body[i].name} u${i};`;
        }
    }
    out += `char _;};struct ${gname}{int type;union ${gname}Union u;};`;
    return out;
}

function generateSub(expr, gen) {
    let out = '';
    let name = mods.concat(expr.name);
    if (gen) {
        name = expr.path;
    }
    if (binds[name.join('_')]) { return ''; }
    let isSelf = expr.tags.map(t => t.name).indexOf('self') !== -1;
    if (isSelf && (expr.args.length === 0 || expr.args[0].name !== 'this')) {
        expr.args = [{
            kind: exprKind.ARG,
            tags: [],
            type: {
                kind: exprKind.TYPE,
                path: [{
                    kind: exprKind.PATH_PART,
                    name: 'ptr',
                    gens: [{
                        kind: exprKind.TYPE,
                        path: expr.path.slice(0, -1).map((p, i) => {
                            return {
                                kind: exprKind.PATH_PART,
                                name: p,
                                gens: [{
                                    kind: exprKind.TYPE,
                                    path: [{ kind: exprKind.PATH_PART, name: expr.gen[i], gens: [] }],
                                }],
                            };
                        }),
                    }],
                }],
            },
            name: 'this',
        }].concat(expr.args);
    }
    for (let i = 0; i < expr.args.length; ++i) {
        const arg = expr.args[i];
        vars[vars.length - 1][arg.name] = arg.type;
    }
    let header = generateType(expr.retType);
    header += ` ${generateGenericName(name, gen)}(`;
    header += generateBlock(expr.args, 0, 0);
    if (expr.tags.map(t => t.name).indexOf('rest') !== -1) {
        if (!reqs.includes('stdarg.h')) {
            reqs.push('stdarg.h');
        }
        header += ',...';
    }
    header += ')';
    if (headers.indexOf(header + ';') === -1) {
        headers.push(header + ';');
        out += header + '{' + generateBlock(expr.body, 1, 0) + '};';
    }
    return out;
}

function generateCompInst(expr, gen) {
    const name = expr.path.map(p => p.name);
    let out = `(${generateGenericName(name, gen)}){`;
    for (let i = 0; i < expr.args.length; ++i) {
        out += '.';
        out += expr.args[i].name;
        out += '=';
        out += generateExpr(expr.args[i].value);
        out += ',';
    }
    out += '}';
    return out;
}

function generateExpr(expr, last, semicolon, parenths) {
    if (expr.gen && expr.gen.length > 0) {
        return '';
    }

    let out = '';

    if (expr.tags) {
        for (let i = 0; i < expr.tags.length; ++i) {
            const tag = expr.tags[i];
            switch (tag.name) {
                case 'os':
                    switch (tag.args[0].value) {
                        case 'win':
                            if (process.platform !== 'win32') {
                                return '';
                            }
                            break;
                        case 'linux':
                            if (process.platform !== 'linux') {
                                return '';
                            }
                            break;
                    }
                    break;
                case 'req':
                    if (!reqs.includes(tag.args[0].value)) {
                        reqs.push(tag.args[0].value);
                    }
                    break;
                case 'bind':
                    break;
                case 'raw':
                    out += '\n' + tag.args[0].value + '\n';
                    break;
            }
        }
    }

    let oldScopeGens = [].concat(scopeGens);
    if (expr.gen) {
        for (let i = 0; i < expr.gen.length; ++i) {
            scopeGens.push(expr.gen[i]);
        }
        expr.gen = oldScopeGens.concat(expr.gen);
    }

    s: switch (expr.kind) {
        case exprKind.USE:
            break;
        case exprKind.MOD:
            mods.push(expr.name);
            out += generateBlock(expr.body, 0, 0);
            mods.pop();
            break;
        case exprKind.COMP: {
            types += generateComp(expr);
            break;
        }
        case exprKind.ENUM: {
            types += generateEnum(expr);
            break;
        }
        case exprKind.OPTION:
            if (expr.body.length > 0) {
                const name = mods.concat(expr.name).join('_');
                out += 'typedef struct{';
                out += generateBlock(expr.body, 1, 0);
                out += `}${name};`;
            }
            break;
        case exprKind.OPTION_FIELD:
            out += generateType(expr.type);
            out += ` ${expr.name};`;
            break;
        case exprKind.PROP:
            break;
        case exprKind.DEF:
            const oldMods = mods.length;
            for (let i = 0; i < expr.prop.path.length; ++i) {
                mods.push(expr.prop.path[i].name);
            }
            out += generateBlock(expr.body, 0, 0);
            mods = mods.slice(0, oldMods);
            break;
        case exprKind.SUB_DEF: {
            out += generateSub(expr);
            break;
        }
        case exprKind.FIELD:
        case exprKind.ARG:
            out += generateType(expr.type);
            out += ` ${expr.name}`;
            if (semicolon) {
                out += ';';
            }
            else if (!last) {
                out += ',';
            }
            break;
        case exprKind.VAR_DEF:
        case exprKind.LOCAL_VAR_DEF:
            out += generateType(expr.type);
            const vname = expr.kind === exprKind.VAR_DEF ? mods.concat(expr.name).join('_') : expr.name;
            vars[vars.length - 1][vname] = expr.type;
            out += ` ${vname}`;
            if (expr.value) {
                out += '=';
                out += generateExpr(expr.value, 0, 0, 0);
            }
            out += ';';
            break;
        case exprKind.SUB_CALL: {
            const name = expr.path.map(p => p.name);
            if (name.length > 1) {
                const enumName = name.slice(0, -1).join('_');
                let _enum = enumExprs[enumName];
                if (_enum) {
                    const index = _enum.body.map(e => e.name).indexOf(expr.path[expr.path.length - 1].name);
                    out += `(${enumName}){.type=${index},.u={.u${index}={`;
                    if (expr.args.length > 0) {
                        for (let i = 0; i < expr.args.length; ++i) {
                            out += '.';
                            out += _enum.body[index].body[i].name;
                            out += '=';
                            out += generateExpr(expr.args[i]);
                            out += ',';
                        }
                    }
                    out += '}}}';
                    break s;
                }
            }
            let bind = binds[name.join('_')];
            if (bind === 'sizeof') {
                out += 'sizeof(';
                out += generateType(expr.path[1].gens[0]);
                out += ')';
                break s;
            }
            generateGeneric(name.join('_'), expr.path.map(p => p.gens));
            out += `${bind ? bind : generateGenericName(expr.path.map(p => p.name), expr.path.map(p => p.gens))}(`;
            out += generateBlock(expr.args, 0, 1);
            out += ')';
            if (semicolon) {
                out += ';';
            }
            break;
        }
        case exprKind.VAR_USE:
            const name = expr.path.map(p => p.name);
            if (name.length > 1) {
                const enumName = name.slice(0, -1).join('_');
                let _enum = enumExprs[enumName];
                if (_enum) {
                    const index = _enum.body.map(e => e.name).indexOf(expr.path[expr.path.length - 1].name);
                    out += `(${enumName}){${index}}`;
                    break s;
                }
            }
            out += name.join('_');
            if (semicolon) {
                out += ';';
            }
            break;
        case exprKind.COMP_INST:
            out += generateCompInst(expr, expr.path.map(p => p.gens));
            break;
        case exprKind.STRING_LIT:
            out += `"${expr.value}"`;
            break;
        case exprKind.ARRAY_LIT:
            break;
        case exprKind.INT_LIT:
        case exprKind.FLOAT_LIT:
            out += expr.value;
            break;
        case exprKind.BOOL_LIT:
            out += expr.value ? '1' : '0';
            break;
        case exprKind.CHAR_LIT:
            out += `'${expr.value}'`;
            break;
        case exprKind.NONE_LIT:
            out += '(void*)0';
            break;
        case exprKind.THIS_LIT:
            out += '(*this)';
            break;
        case exprKind.SELF_LIT:
            break;
        case exprKind.BLOCK:
            out += '{';
            out += generateBlock(expr.body, 1, 0);
            if (expr.label) {
                out += `${expr.label}:`;
            }
            out += '}';
            break;
        case exprKind.LOOP:
            out += 'for(;;) {';
            out += generateBlock(expr.body, 1, 0);
            if (expr.label) {
                out += `${expr.label}:`;
            }
            out += '}';
            break;
        case exprKind.WHILE:
            out += 'while(';
            out += generateExpr(expr.cond, 0, 0, 0);
            out += ') {';
            out += generateBlock(expr.body, 1, 0);
            if (expr.label) {
                out += `${expr.label}:`;
            }
            out += '}';
            break;
        case exprKind.IF:
            out += 'if (';
            out += generateExpr(expr.if.cond, 0, 0, 0);
            out += ') {';
            out += generateBlock(expr.if.body, 1, 0);
            out += '}';
            for (let i = 0; i < expr.elif.length; ++i) {
                out += ' else if (';
                out += generateExpr(expr.elif[i].cond, 0, 0, 0);
                out += ') {';
                out += generateBlock(expr.elif[i].body, 1, 0);
                out += '}';
            }
            if (expr.else) {
                out += ' else {';
                out += generateBlock(expr.else, 1, 0);
                out += '}';
            }
            break;
        case exprKind.FOR:
            out += `for(int ${expr.loop_var.name}=${generateExpr(expr.loop_var.value)};`;
            out += generateExpr(expr.cond, 0, 0, 0);
            out += ';';
            out += generateExpr(expr.next, 0, 0, 0);
            out += ') {';
            out += generateBlock(expr.body, 1, 0);
            if (expr.label) {
                out += `${expr.label}:`;
            }
            out += '}';
            break;
        case exprKind.EACH:
            break;
        case exprKind.MATCH:
            const value = generateExpr(expr.value, 0, 0, 0);
            const _enum = enumExprs[expr.body[0].cases[0].path.map(p => p.name).slice(0, -1).join('_')];
            for (let i = 0; i < expr.body.length; ++i) {
                const _case = expr.body[i].cases[0];
                const name = _case.path[_case.path.length - 1].name;
                let index = _enum.body.map(e => e.name).indexOf(name);
                if (index === -1) {
                    index = `${generateExpr(_case, 0, 0, 0)}.type`;
                }
                if (name === '_') {
                    out += `}else{`;
                } else {
                    if (i === 0) {
                        out += `if(${value}.type==${index}){`;
                    } else {
                        out += `}else if(${value}.type==${index}){`;
                    }
                }
                if (_case.kind === exprKind.SUB_CALL) {
                    for (let ii = 0; ii < _case.args.length; ++ii) {
                        out += generateType(_enum.body[index].body[ii].type) + ' ';
                        out += _case.args[ii].path[0].name;
                        out += `=${value}.u.u${index}.${_enum.body[index].body[ii].name};`;
                    }
                }
                out += generateBlock(expr.body[i].body, 1, 0);
            }
            out += '}';
            break;
        case exprKind.NEXT:
            if (expr.target) {
                out += `goto ${expr.target};`;
            } else {
                out += `continue;`;
            }
            break;
        case exprKind.JUMP:
            out += `goto ${expr.target};`;
            break;
        case exprKind.RET:
            if (expr.target) {
                out += `goto brk_${expr.target}`;
            } else {
                out += 'return';
            }
            if (expr.value) {
                out += ' ';
                out += generateExpr(expr.value, 0, 0, 0);
            }
            out += ';';
            break;
        case exprKind.TRY:
            break;
        case exprKind.UNARY_OP:
            out += '(';
            switch (expr.type) {
                case tokenKind.NOT:
                    out += '!';
                    break;
                case tokenKind.DEREF:
                    out += '*';
                    break;
                case tokenKind.ADDRESS:
                    out += '&';
                    break;
            }
            out += generateExpr(expr.value, 0, 0, parenths + 1);
            out += ')';
            break;
        case exprKind.BINARY_OP:
            if (parenths) {
                out += '(';
            }

            if (expr.type === tokenKind.DOT && expr.rhs.kind === exprKind.SUB_CALL) {
                const name = expr.lhs.path.map(p => p.name).join('_');
                for (let i = vars.length - 1; i >= 0; --i) {
                    const v = vars[i];
                    if (v[name]) {
                        expr = {
                            kind: exprKind.SUB_CALL,
                            label: expr.label,
                            path: v[name].path.concat(expr.rhs.path),
                            args: [{
                                kind: exprKind.UNARY_OP,
                                type: tokenKind.ADDRESS,
                                value: expr.lhs,
                            }].concat(expr.rhs.args),
                            tags: expr.tags,
                        };
                        out += generateExpr(expr, last, semicolon, parenths);
                        break s;
                    }
                }

            }

            if (expr.type === tokenKind.AS) {
                out += '(';
                out += generateType(expr.rhs);
                out += ')';
                out += generateExpr(expr.lhs, 0, 0, parenths + 1);
            } else {
                out += generateExpr(expr.lhs, 0, 0, parenths + 1);
                switch (expr.type) {
                    case tokenKind.ASSIGN:
                        out += '=';
                        break;
                    case tokenKind.EQUAL:
                        out += '==';
                        break;
                    case tokenKind.AND:
                        out += '&&';
                        break;
                    case tokenKind.OR:
                        out += '||';
                        break;
                    case tokenKind.LESS:
                        out += '<';
                        break;
                    case tokenKind.LESS_EQUAL:
                        out += '<=';
                        break;
                    case tokenKind.GREATER:
                        out += '>';
                        break;
                    case tokenKind.GREATER_EQUAL:
                        out += '>=';
                        break;
                    case tokenKind.ADD:
                        out += '+';
                        break;
                    case tokenKind.ADD_ASSIGN:
                        out += '+=';
                        break;
                    case tokenKind.SUBTRACT:
                        out += '-';
                        break;
                    case tokenKind.SUBTRACT_ASSIGN:
                        out += '-=';
                        break;
                    case tokenKind.MULTIPLY:
                        out += '*';
                        break;
                    case tokenKind.MULTIPLY_ASSIGN:
                        out += '*=';
                        break;
                    case tokenKind.DIVIDE:
                        out += '/';
                        break;
                    case tokenKind.DIVIDE_ASSIGN:
                        out += '/=';
                        break;
                    case tokenKind.MODULO:
                        out += '%';
                        break;
                    case tokenKind.MODULO_ASSIGN:
                        out += '%=';
                        break;
                    case tokenKind.NOT_EQUAL:
                        out += '!=';
                        break;
                    case tokenKind.DOT:
                        out += '.';
                        break;
                    case tokenKind.LEFT_BRACKET:
                        out += '[';
                        break;
                }
                out += generateExpr(expr.rhs, 0, 0, parenths + 1);
                if (expr.type === tokenKind.LEFT_BRACKET) {
                    out += ']';
                }
            }
            if (parenths) {
                out += ')';
            }
            if (semicolon) {
                out += ';';
            }
            break;
    }

    if (expr.label) {
        out += `brk_${expr.label}:`;
    }

    scopeGens = oldScopeGens;

    return out;
}

function generateBlock(exprs, semicolon, comma) {
    let out = '';
    vars.push({});
    for (let i = 0; i < exprs.length; ++i) {
        let is_last = i + 1 === exprs.length;
        out += generateExpr(exprs[i], is_last, semicolon, 0);
        if (comma && !is_last) {
            out += ',';
        }
    }
    vars.pop();
    return out;
}

module.exports.generate = function (ast) {
    types = '';
    output = '';
    mods = [];
    genNames = {};
    reqs = [];
    scopeGens = [];
    vars = [];
    typeHeaders = [];
    headers = [];
    let out = generateBlock(ast, 0, 0);
    output = typeHeaders.join('') + headers.join('') + types + output;
    output += out;
    for (let i = 0; i < reqs.length; ++i) {
        reqs[i] = `#include <${reqs[i]}>\n`;
    }
    output = reqs.join('') + output;
    return output;
};