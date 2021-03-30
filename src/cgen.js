const tokenKind = require('./lexer.js').tokenKind;
const exprKind = require('./parser.js').exprKind;

let output;
let mods;
let gens;
let reqs;
let binds;

function generateType(type) {
    if (type === undefined) {
        output += 'void';
        return;
    }

    type = type.path[0];
    if (type === 'i32') {
        output += 'int32_t';
    }
    else if (type === 'f32') {
        output += 'float';
    }
    else if (type === 'i8') {
        output += 'int8_t';
    }
    else if (type === 'f64') {
        output += 'double';
    }
    else if (type === 'u32') {
        output += 'uint32_t';
    }
    else if (type === 'i64') {
        output += 'int64_t';
    }
    else if (type === 'i16') {
        output += 'int16_t';
    }
    else if (type === 'u8') {
        output += 'uint8_t';
    }
    else if (type === 'u64') {
        output += 'uint64_t';
    }
    else if (type === 'u16') {
        output += 'uint16_t';
    }
    else {
        output += type;
    }
}

function registerGenericUse(type) {
    if (type.gens.length) {
        gens[type.path.join('_')] = type.gens;
    }
}

function generateExpr(expr, last, semicolon, parenths) {
    if (expr.tags) {
        for (let i = 0; i < expr.tags.length; ++i) {
            const tag = expr.tags[i];
            switch (tag.name) {
                case 'req':
                    if (!reqs.includes(tag.args[0].value)) {
                        reqs.push(tag.args[0].value);
                    }
                    break;
                case 'bind':
                    binds[mods.concat(expr.name).join('_')] = tag.args[0].value;
                    break;
            }
        }
    }

    switch (expr.kind) {
        case exprKind.USE:
            break;
        case exprKind.MOD:
            mods.push(expr.name);
            generateVector(expr.body, 0, 0);
            mods.pop();
            break;
        case exprKind.COMP: {
            let name = mods.concat(expr.name).join('_');
            let gen = gens[name];
            console.log(gen);
            output += 'typedef struct {';
            generateVector(expr.body, 1, 0);
            output += `} ${name};`;
            break;
        }
        case exprKind.ENUM: {
            generateVector(expr.body, 1, 0);
            let name = mods.concat(expr.name).join('_');
            output += `union ${name} {`;
            for (let i = 0; i < expr.body.length; ++i) {
                output += `struct ${expr.body[i].name} u${i};`;
            }
            output += `};\n`;
            output += 'typedef struct {\n';
            output += '    int type;\n';
            output += `    union ${name} u;\n`;
            output += `} ${name};`;
            break;
        }
        case exprKind.OPTION:
            output += `struct ${expr.name} {`;
            generateVector(expr.body, 1, 0);
            output += '}';
            break;
        case exprKind.OPTION_FIELD:
            generateType(expr.type);
            output += ` ${expr.name};`;
            break;
        case exprKind.PROP:
            break;
        case exprKind.DEF:
            break;
        case exprKind.SUB_DEF: {
            let name = mods.concat(expr.name).join('_');
            if (binds[name]) { break; }
            generateType(expr.retTypes[0]);
            output += ` ${name}(`;
            generateVector(expr.args, 0, 0);
            output += ') {';
            generateVector(expr.body, 1, 0);
            output += '};';
            break;
        }
        case exprKind.FIELD:
        case exprKind.ARG:
            registerGenericUse(expr.type);
            generateType(expr.type);
            output += ` ${expr.name}`;
            if (semicolon) {
                output += ';';
            }
            else if (!last) {
                output += ',';
            }
            break;
        case exprKind.VAR_DEF:
        case exprKind.LOCAL_VAR_DEF:
            registerGenericUse(expr.type);
            generateType(expr.type);
            output += ` ${mods.concat(expr.name).join('_')}`;
            if (expr.value) {
                output += '=';
                generateExpr(expr.value, 0, 0, 0);
            }
            output += ';';
            break;
        case exprKind.SUB_CALL:
            let path = expr.path.join('_');
            let bind = binds[path];
            output += `${bind ? bind : path}(`;
            generateVector(expr.args, 0, 1);
            output += ')';
            if (semicolon) {
                output += ';';
            }
            break;
        case exprKind.VAR_USE:
            output += expr.path.join('_');
            if (semicolon) {
                output += ';';
            }
            break;
        case exprKind.STRING_LIT:
            output += `"${expr.value}"`;
            break;
        case exprKind.ARRAY_LIT:
            break;
        case exprKind.INT_LIT:
        case exprKind.FLOAT_LIT:
            output += expr.value;
            break;
        case exprKind.BOOL_LIT:
            output += expr.value ? '1' : '0';
            break;
        case exprKind.CHAR_LIT:
            output += `'${expr.value}'`;
            break;
        case exprKind.NULL_LIT:
            output += 'NULL';
            break;
        case exprKind.SELF_LIT:
            break;
        case exprKind.BLOCK:
            output += '{';
            generateVector(expr.body, 1, 0);
            output += '}';
            break;
        case exprKind.LOOP:
            output += 'for(;;) {';
            generateVector(expr.body, 1, 0);
            output += '}';
            break;
        case exprKind.WHILE:
            output += 'while(';
            generateExpr(expr.cond, 0, 0, 0);
            output += ') {';
            generateVector(expr.body, 1, 0);
            output += '}';
            break;
        case exprKind.IF:
            output += 'if (';
            generateExpr(expr.if.cond, 0, 0, 0);
            output += ') {';
            generateVector(expr.if.body, 1, 0);
            output += '}';
            for (let i = 0; i < expr.elif.length; ++i) {
                output += ' else if (';
                generateExpr(expr.elif[i].cond, 0, 0, 0);
                output += ') {';
                generateVector(expr.elif[i].body, 1, 0);
                output += '}';
            }
            if (expr.else) {
                output += ' else {';
                generateVector(expr.else, 1, 0);
                output += '}';
            }
            break;
        case exprKind.FOR:
            output += `for(int ${expr.loop_var}=0;`;
            generateExpr(expr.cond, 0, 0, 0);
            output += ';';
            generateExpr(expr.next, 0, 0, 0);
            output += ') {';
            generateVector(expr.body, 1, 0);
            output += '}';
            break;
        case exprKind.EACH:
            break;
        case exprKind.MATCH:
            break;
        case exprKind.NEXT:
            output += `continue ${expr.target};`;
            break;
        case exprKind.JUMP:
            output += `goto ${expr.target};`;
            break;
        case exprKind.RET:
            if (expr.label) {
                output += `break ${expr.label}`;
            } else {
                output += 'return';
            }
            if (expr.value) {
                output += ' ';
                generateExpr(expr.value, 0, 0, 0);
            }
            output += ';';
            break;
        case exprKind.TRY:
            break;
        case exprKind.UNARY_OP:
            switch (expr.type) {
                case tokenKind.NOT:
                    output += '!';
                    break;
                case tokenKind.DEREF:
                    output += '*';
                    break;
                case tokenKind.ADDRESS:
                    output += '&';
                    break;
            }
            generateExpr(expr.value, 0, 0, parenths + 1);
            break;
        case exprKind.BINARY_OP:
            if (parenths) {
                output += '(';
            }
            generateExpr(expr.lhs, 0, 0, parenths + 1);
            switch (expr.type) {
                case tokenKind.ASSIGN:
                    output += '=';
                    break;
                case tokenKind.EQUAL:
                    output += '==';
                    break;
                case tokenKind.AND:
                    output += '&&';
                    break;
                case tokenKind.OR:
                    output += '||';
                    break;
                case tokenKind.LESS:
                    output += '<';
                    break;
                case tokenKind.LESS_EQUAL:
                    output += '<=';
                    break;
                case tokenKind.GREATER:
                    output += '>';
                    break;
                case tokenKind.GREATER_EQUAL:
                    output += '>=';
                    break;
                case tokenKind.ADD:
                    output += '+';
                    break;
                case tokenKind.ADD_ASSIGN:
                    output += '+=';
                    break;
                case tokenKind.SUBTRACT:
                    output += '-';
                    break;
                case tokenKind.SUBTRACT_ASSIGN:
                    output += '-=';
                    break;
                case tokenKind.MULTIPLY:
                    output += '*';
                    break;
                case tokenKind.MULTIPLY_ASSIGN:
                    output += '*=';
                    break;
                case tokenKind.DIVIDE:
                    output += '/';
                    break;
                case tokenKind.DIVIDE_ASSIGN:
                    output += '/=';
                    break;
                case tokenKind.MODULO:
                    output += '%';
                    break;
                case tokenKind.MODULO_ASSIGN:
                    output += '%=';
                    break;
                case tokenKind.NOT_EQUAL:
                    output += '!=';
                    break;
            }
            generateExpr(expr.rhs, 0, 0, parenths + 1);
            if (parenths) {
                output += ')';
            }
            if (semicolon) {
                output += ';';
            }
            break;
    }
}

function generateVector(exprs, semicolon, comma) {
    for (let i = 0; i < exprs.length; ++i) {
        let is_last = i + 1 === exprs.length;
        generateExpr(exprs[i], is_last, semicolon, 0);
        if (comma && !is_last) {
            output += ',';
        }
    }
}

module.exports.generate = function (ast) {
    output = '';
    mods = [];
    gens = {};
    reqs = [];
    binds = {};
    generateVector(ast, 0, 0);
    for (let i = 0; i < reqs.length; ++i) {
        reqs[i] = `#include <${reqs[i]}>\n`;
    }
    output = reqs.join('') + output;
    return output;
};