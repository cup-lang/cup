const tokenKind = require('./lexer.js').tokenKind;
const exprKind = require('./parser.js').exprKind;

let output;
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

function generateExpr(expr, last, semicolon, parenths) {
    switch (expr.kind) {
        case exprKind.USE:
            break;
        case exprKind.MOD:
            generateVector(expr.body, 0, 0);
            break;
        case exprKind.COMP:
            output += 'typedef struct {';
            generateVector(expr.body, 1, 0);
            output += `} ${expr.name}`;
            break;
        case exprKind.ENUM:
            generateVector(expr.body, 1, 0);
            output += 'typedef union {';
            for (let i = 0; i < expr.body.length; ++i) {
                output += `struct ${expr.body[i].name} u${i};`;
            }
            output += `} ${expr.name}Union;\n`;
            output += 'typedef struct {\n';
            output += '    int type;\n';
            output += `    ${expr.name}Union u;\n`;
            output += `} ${expr.name};`;
            break;
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
        case exprKind.SUB_DEF:
            generateType(expr.retTypes[0]);
            output += ` ${expr.name}(`;
            generateVector(expr.args, 0, 0);
            output += ') {';
            generateVector(expr.body, 1, 0);
            output += '}';
            break;
        case exprKind.FIELD:
        case exprKind.ARG:
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
            generateType(expr.type);
            output += ` ${expr.name}`;
            if (expr.value) {
                output += '=';
                generateExpr(expr.value, 0, 0, 0);
            }
            output += ';';
            break;
        case exprKind.SUB_CALL:
            output += `${expr.path[0]}(`;
            generateVector(expr.args, 0, 1);
            output += ')';
            if (semicolon) {
                output += ';';
            }
            break;
        case exprKind.VAR_USE:
            output += expr.path[0];
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
    output = '#include <stdint.h>\n';
    binds = {};
    generateVector(ast, 0, 0);
    return output;
};