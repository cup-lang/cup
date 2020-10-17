const fs = require("fs");

const command_type = {
    NONE: 0,
    HELP: 1,
    HELP_RUN: 2,
    HELP_BUILD: 3,
    HELP_CHECK: 4,
    HELP_NEW_PACKAGE: 5,
    HELP_UPDATE_PACKAGE: 6,
    HELP_ADD_PACKAGE: 7,
    HELP_REMOVE_PACKAGE: 8,
    HELP_GEN_DOCS: 9,
    HELP_GEN_BINDS: 10,
    HELP_SELF_UPDATE: 11,
    HELP_SELF_INSTALL: 12,
    HELP_SELF_UNINSTALL: 13,
    RUN: 14,
    BUILD: 15,
    CHECK: 16,
    NEW_PACKAGE: 17,
    UPDATE_PACKAGE: 18,
    ADD_PACKAGE: 19,
    REMOVE_PACKAGE: 20,
    GEN_DOCS: 21,
    GEN_BINDS: 22,
    SELF_UPDATE: 23,
    SELF_INSTALL: 24,
    SELF_UNINSTALL: 25
}

const command_aliases = [
    "help",
    "help run",
    "help build",
    "help check",
    "help update",
    "help add",
    "help remove",
    "help gen docs",
    "help gen binds",
    "help self update",
    "help self install",
    "help self uninstall",
    "run",
    "build",
    "check",
    "update",
    "add",
    "remove",
    "gen docs",
    "gen binds",
    "self update",
    "self install",
    "self uninstall"
];

const COMPILE_OPTIONS = "\n    -i, --input     Specify the input file name\n    -o, --output    Specify the output file name\n    -cg, --gcc     Use GCC as a compiler\n    -cm, --msvc    Use MSVC as a compiler\n    -cc, --clang   use Clang as a compiler\n    -ct, --tcc     Use TinyCC as a compiler";
const NO_OPTIONS = "\n    No options are available for this command";

function get_command() {
    if (argc === 2) {
        return command_type.HELP;
    }

    let command = "";
    for (let i = 2; i < argc; ++i) {
        if (i === 5) {
            break;
        }

        const arg = process.argv[i];
        if (arg[0] === "-") {
            break;
        }

        if (command.length) {
            command += " ";
        }
        command += arg;
    }

    command = command_aliases.indexOf(command);

    return command === -1 ? command_type.NONE : command + 2;
}

function command_help(name, desc, options) {
    console.log(desc);
    console.log("\nUSAGE:");
    console.log("cup " + name + " [OPTIONS]");
    console.log("\nOPTIONS:" + options);
    process.exit(1);
}

function duplicate_option(index) {
    console.error(`error: duplicate option: '${process.argv[index]}'`);
    process.exit(1);
}

const argc = process.argv.length;
const command = get_command();

if (argc === 2 || command === command_type.HELP) {
    console.log("Cup Toolkit v0.0.1\n");
    console.log("USAGE:\n    cup [COMMAND] [OPTIONS]\n");
    console.log("command_type:");
    console.log("    run                  Compile and run the current package");
    console.log("    build                Compile the current package");
    console.log("    check                Analyze the current package");
    console.log("    new [PACKAGE]        Create a new package");
    console.log("    update [PACKAGE]     Update given dependency");
    console.log("    add [PACKAGE]        Adds given dependency");
    console.log("    remove [PACKAGE]     Removes given dependency");
    console.log("    gen docs             Generate documentation for the current package");
    console.log("    gen binds [HEADER]   Generate bindings for a given C header file");
    console.log("    self update          Update the Cup Toolkit");
    console.log("    self install         Install the Cup Toolkit");
    console.log("    self uninstall       Uninstall the Cup Toolkit");
    console.log("\nSee 'cup help [COMMAND]' for more info about a specific command and it's available options.");
} else if (command === command_type.NONE) {
    console.error(`error: no such command: '${process.argv.slice(2).join(" ")}'`);
    console.error("\nSee 'cup help' for the list of available command_type.");
    process.exit(1);
}

switch (command) {
    case command_type.RUN:
    case command_type.BUILD:
        break;
    case command_type.HELP_RUN:
        command_help("run", "Compile and run the current package", COMPILE_OPTIONS);
    case command_type.HELP_BUILD:
        command_help("build", "Compile the current package", COMPILE_OPTIONS);
    case command_type.HELP_CHECK:
        command_help("check", "Analyze the current package", NO_OPTIONS);
    case command_type.HELP_NEW_PACKAGE:
        command_help("new", "Create a new package", NO_OPTIONS);
    case command_type.HELP_UPDATE_PACKAGE:
        command_help("update", "Update given dependency", NO_OPTIONS);
    case command_type.HELP_ADD_PACKAGE:
        command_help("add", "Adds given dependency", NO_OPTIONS);
    case command_type.HELP_REMOVE_PACKAGE:
        command_help("remove", "Removes given dependency", NO_OPTIONS);
    case command_type.HELP_GEN_DOCS:
        command_help("gen docs", "Generate documentation for the current package", NO_OPTIONS);
    case command_type.HELP_GEN_BINDS:
        command_help("gen binds", "Generate bindings for a given C header file", NO_OPTIONS);
    case command_type.HELP_SELF_UPDATE:
        command_help("self update", "Update the Cup Toolkit", NO_OPTIONS);
    case command_type.HELP_SELF_INSTALL:
        command_help("self install", "Install the Cup Toolkit", NO_OPTIONS);
    case command_type.HELP_SELF_UNINSTALL:
        command_help("self uninstall", "Uninstall the Cup Toolkit", NO_OPTIONS);
}

let input = "src\\main.cp";
let output = "bin\\main.c";

for (let i = 3; i < argc; ++i) {
    const arg = process.argv[i];

    switch (arg) {
        case "-i":
        case "--input":
            if (input.length === 0) {
                if (i + 1 < argc) {
                    input = process.argv[++i];
                }
                continue;
            }
            duplicate_option(i);
        case "-o":
        case "--output":
            if (output.length === 0) {
                if (i + 1 < argc) {
                    output = process.argv[++i];
                }
                continue;
            }
            duplicate_option(i);
    }

    console.error(`error: invalid option: '${arg}'`);
    console.error(`\nSee 'cup help ${command_aliases[command - 2]}' for the list of available options.`);
    process.exit(1);
}

let code = "#include <stdint.h>\n";
let mod = [];

compile_deep(process.cwd() + "\\src");

function compile_deep(path) {
    const dir = fs.readdirSync(path);
    for (let i = 0; i < dir.length; ++i) {
        const file = dir[i];
        if (file.endsWith(".cp")) {
            compile(fs.readFileSync(path + "\\" + file).toString());
        } else if (fs.lstatSync(path + "\\" + file).isDirectory()) {
            mod.push(file);
            compile_deep(path + "\\" + file);
            mod.pop();
        }
    }
}

function compile(file) {

    // #region Lexer

    const token_type = {
        // General
        IDENTIFIER: 0,
        AS: 1,
        DO: 2,
        IN: 3,
        IF: 4,
        PUB: 5,
        USE: 6,
        MOD: 7,
        FOR: 8,
        IMP: 9,
        INL: 10,
        FALL: 11,
        NULL: 12,
        TRUE: 13,
        REST: 14,
        ENUM: 15,
        ELIF: 16,
        ELSE: 17,
        NEXT: 18,
        GOTO: 19,
        SELF: 20,
        FALSE: 21,
        TRAIT: 22,
        CONST: 23,
        DEFER: 24,
        WHILE: 25,
        UNION: 26,
        BREAK: 27,
        MATCH: 28,
        MACRO: 29,
        STRUCT: 30,
        RETURN: 31,
        // Symbols
        LEFT_PAREN: 32,
        RIGHT_PAREN: 33,
        LEFT_BRACE: 34,
        RIGHT_BRACE: 35,
        LEFT_SQUARE: 36,
        RIGHT_SQUARE: 37,
        ARROW: 38,
        EQUAL_ARROW: 39,
        SEMICOLON: 40,
        COLON: 41,
        COMMA: 42,
        DOT: 43,
        // Operators
        RANGE: 44,
        ASSIGN: 45,
        EQUAL: 46,
        NOT_EQUAL: 47,
        LESS: 48,
        LESS_EQUAL: 49,
        GREATER: 50,
        GREATER_EQUAL: 51,
        NOT: 52,
        AND: 53,
        OR: 54,
        ADD: 55,
        ADD_ASSIGN: 56,
        SUBSTRACT: 57,
        SUBSTRACT_ASSIGN: 58,
        MULTIPLY: 59,
        MULTIPLY_ASSIGN: 60,
        DIVIDE: 61,
        DIVIDE_ASSIGN: 62,
        MODULO: 63,
        MODULO_ASSIGN: 64,
        BITWISE_NOT: 65,
        BITWISE_AND: 66,
        BITWISE_AND_ASSIGN: 67,
        BITWISE_OR: 68,
        BITWISE_OR_ASSIGN: 69,
        BITWISE_XOR: 70,
        BITWISE_XOR_ASSIGN: 71,
        LEFT_SHIFT: 72,
        LEFT_SHIFT_ASSIGN: 73,
        RIGHT_SHIFT: 74,
        RIGHT_SHIFT_ASSIGN: 75
    }

    let tokens = [];
    let value = "";
    let comment = false;
    for (let i = 0; i < file.length; ++i) {
        const char = file[i];

        if (comment) {
            if (char === "\n") {
                comment = false;
            }
            continue;
        }

        let type = token_type.IDENTIFIER;

        if (value[0] !== "\"" && /\s/.test(char)) {
            type = -1;
        }

        switch (char) {
            case "#":
                comment = true;
                type = -1;
                break;
            case "(":
                type = token_type.LEFT_PAREN;
                break;
            case ")":
                type = token_type.RIGHT_PAREN;
                break;
            case "{":
                type = token_type.LEFT_BRACE;
                break;
            case "}":
                type = token_type.RIGHT_BRACE;
                break;
            case "[":
                type = token_type.LEFT_SQUARE;
                break;
            case "]":
                type = token_type.RIGHT_SQUARE;
                break;
            case ";":
                type = token_type.SEMICOLON;
                break;
            case ":":
                if (file[i + 1] === ":" && value.length) {
                    value += ":";
                    ++i;
                } else {
                    type = token_type.COLON;
                }
                break;
            case ",":
                type = token_type.COMMA;
                break;
            case ".":
                if (file[i + 1] === ".") {
                    type = token_type.RANGE;
                    ++i;
                } else {
                    type = token_type.DOT;
                }
                break;
            case "=":
                if (file[i + 1] === ">") {
                    type = token_type.EQUAL_ARROW;
                    ++i;
                } else if (file[i + 1] === "=") {
                    type = token_type.EQUAL;
                    ++i;
                } else {
                    type = token_type.ASSIGN;
                }
                break;
            case "!":
                if (file[i + 1] === "=") {
                    type = token_type.NOT_EQUAL;
                    ++i;
                } else {
                    type = token_type.NOT;
                }
                break;
            case "<":
                if (file[i + 1] === "=") {
                    type = token_type.LESS_EQUAL;
                    ++i;
                } else if (file[i + 1] === "<") {
                    if (file[i + 2] === "=") {
                        type = token_type.LEFT_SHIFT_ASSIGN;
                        ++i;
                    } else {
                        type = token_type.LEFT_SHIFT;
                    }
                    ++i;
                } else {
                    type = token_type.LESS;
                }
                break;
            case ">":
                if (file[i + 1] === "=") {
                    type = token_type.GREATER;
                    ++i;
                } else if (file[i + 1] === ">") {
                    if (file[i + 2] === "=") {
                        type = token_type.RIGHT_SHIFT_ASSIGN;
                        ++i;
                    } else {
                        type = token_type.RIGHT_SHIFT;
                    }
                    ++i;
                } else {
                    type = token_type.GREATER_EQUAL;
                }
                break;
            case "+":
                if (file[i + 1] === "=") {
                    type = token_type.ADD_ASSIGN;
                    ++i;
                } else {
                    type = token_type.ADD;
                }
                break;
            case "-":
                if (file[i + 1] === ">") {
                    type = token_type.ARROW;
                    ++i;
                } else if (file[i + 1] === "=") {
                    type = token_type.SUBSTRACT_ASSIGN;
                    ++i;
                } else {
                    type = token_type.SUBSTRACT;
                }
                break;
            case "*":
                if (file[i + 1] === "=") {
                    type = token_type.MULTIPLY_ASSIGN;
                    ++i;
                } else {
                    type = token_type.MULTIPLY;
                }
                break;
            case "/":
                if (file[i + 1] === "=") {
                    type = token_type.DIVIDE_ASSIGN;
                    ++i;
                } else {
                    type = token_type.DIVIDE;
                }
                break;
            case "%":
                if (file[i + 1] === "=") {
                    type = token_type.MODULO_ASSIGN;
                    ++i;
                } else {
                    type = token_type.MODULO;
                }
                break;
            case "~":
                type = token_type.BITWISE_NOT;
                break;
            case "&":
                if (file[i + 1] === "=") {
                    type = token_type.BITWISE_AND_ASSIGN;
                    ++i;
                } else {
                    type = token_type.BITWISE_AND;
                }
                break;
            case "|":
                if (file[i + 1] === "=") {
                    type = token_type.BITWISE_OR_ASSIGN;
                    ++i;
                } else {
                    type = token_type.BITWISE_OR;
                }
                break;
            case "^":
                if (file[i + 1] === "=") {
                    type = token_type.BITWISE_XOR_ASSIGN;
                    ++i;
                } else {
                    type = token_type.BITWISE_XOR;
                }
                break;
        }

        if (type) {
            let value_type = token_type.IDENTIFIER;

            if (value.length) {
                value_type = -1;
            }

            switch (value) {
                case "as": value_type = token_type.AS;
                    break;
                case "do": value_type = token_type.DO;
                    break;
                case "in": value_type = token_type.IN;
                    break;
                case "if": value_type = token_type.IF;
                    break;
                case "pub": value_type = token_type.PUB;
                    break;
                case "use": value_type = token_type.USE;
                    break;
                case "mod": value_type = token_type.MOD;
                    break;
                case "for": value_type = token_type.FOR;
                    break;
                case "imp": value_type = token_type.IMP;
                    break;
                case "inl": value_type = token_type.INL;
                    break;
                case "fall": value_type = token_type.FALL;
                    break;
                case "null": value_type = token_type.NULL;
                    break;
                case "true": value_type = token_type.TRUE;
                    break;
                case "rest": value_type = token_type.REST;
                    break;
                case "enum": value_type = token_type.ENUM;
                    break;
                case "elif": value_type = token_type.ELIF;
                    break;
                case "else": value_type = token_type.ELSE;
                    break;
                case "next": value_type = token_type.NEXT;
                    break;
                case "goto": value_type = token_type.GOTO;
                    break;
                case "self": value_type = token_type.SELF;
                    break;
                case "false": value_type = token_type.FALSE;
                    break;
                case "trait": value_type = token_type.TRAIT;
                    break;
                case "const": value_type = token_type.CONST;
                    break;
                case "defer": value_type = token_type.DEFER;
                    break;
                case "while": value_type = token_type.WHILE;
                    break;
                case "union": value_type = token_type.UNION;
                    break;
                case "break": value_type = token_type.BREAK;
                    break;
                case "match": value_type = token_type.MATCH;
                    break;
                case "macro": value_type = token_type.MACRO;
                    break;
                case "struct": value_type = token_type.STRUCT;
                    break;
                case "return": value_type = token_type.RETURN;
                    break;
            }

            if (value_type) {
                if (value_type === -1) {
                    tokens.push({ type: token_type.IDENTIFIER, value: value, index: i });
                } else {
                    tokens.push({ type: value_type, index: i - value.length });
                }
                value = "";
            }

            if (type !== -1) {
                tokens.push({ type: type, index: i });
            }
        } else {
            value += char;
        }
    }

    // #endregion

    // #region Parser

    const expr_type = {
        MOD: 0,
        FN_DEF: 1,
        ARG: 2,
        FN_CALL: 3,
        VAR_DEF: 4,
        STRUCT: 5,
        ENUM: 6,
        TRAIT: 7,
        UNION: 8,
        VAR_USE: 9,
        OP: 10,
        VALUE: 11,
        ARRAY: 12,
        BLOCK: 13,
        IF: 14,
        ELIF: 15,
        ELSE: 16,
        DO: 17,
        WHILE: 18,
        MATCH: 19,
        FOR: 20,
        RETURN: 21,
        BREAK: 22,
        NEXT: 23
    };

    let var_stack = null;
    let index = 0;

    function next_token_of_type(type, start, end) {
        let paren_count = 0;

        for (let i = start; i < end; ++i) {
            const t = tokens[i].type;

            switch (t) {
                case token_type.LEFT_PAREN:
                    ++paren_count;
                    break;
                case token_type.RIGHT_PAREN:
                    --paren_count;
                    break;
            }

            switch (type) {
                case token_type.RIGHT_PAREN:
                case token_type.COMMA:
                    if (paren_count > 0) {
                        break;
                    }
                default:
                    if (t === type) {
                        return i;
                    }
                    break;
            }
        }

        return -1;
    }

    function is_number(s) {
        const split = s.split("");
        for (let i = 0; i < split.length; ++i) {
            const c = split[i];
            if (parseInt(c)) {
                continue;
            }
            return 0;
        }
        return 1;
    }

    function parse(start, end) {
        let op_level = 0;
        let op_count = 0;
        let op_index;
        let paren_count = 0;
        let op_type;

        for (let i = start; i < end; ++i) {
            const type = tokens[i].type;

            switch (type) {
                case token_type.LEFT_PAREN:
                    ++paren_count;
                    continue;
                case token_type.RIGHT_PAREN:
                    if (paren_count-- === 0) {
                        throw "unexpected ')'";
                    }
                    continue;
            }

            if (paren_count) {
                continue;
            }

            ++op_count;

            switch (type) {
                case token_type.ASSIGN:
                case token_type.ADD_ASSIGN:
                case token_type.SUBSTRACT_ASSIGN:
                case token_type.MULTIPLY_ASSIGN:
                case token_type.DIVIDE_ASSIGN:
                case token_type.MODULO_ASSIGN:
                    if (op_level < 6) {
                        op_level = 6;
                    }
                    if (op_level === 6) {
                        op_type = type;
                        op_index = i;
                    }
                    break;
                case token_type.EQUAL:
                case token_type.NOT_EQUAL:
                    if (op_level < 5) {
                        op_level = 5;
                    }
                    if (op_level === 5) {
                        op_type = type;
                        op_index = i;
                    }
                    break;
                case token_type.LESS:
                case token_type.LESS_EQUAL:
                case token_type.GREATER:
                case token_type.GREATER_EQUAL:
                    if (op_level < 4) {
                        op_level = 4;
                    }
                    if (op_level === 4) {
                        op_type = type;
                        op_index = i;
                    }
                    break;
                case token_type.ADD:
                case token_type.SUBSTRACT:
                    if (op_level < 3) {
                        op_level = 3;
                    }
                    if (op_level === 3) {
                        op_type = type;
                        op_index = i;
                    }
                    break;
                case token_type.MULTIPLY:
                case token_type.DIVIDE:
                case token_type.MODULO:
                    if (op_level < 2) {
                        op_level = 2;
                    }
                    if (op_level === 2) {
                        op_type = type;
                        op_index = i;
                    }
                    break;
                case token_type.NOT:
                    if (op_level == 0) {
                        op_level = 1;
                    }
                    if (op_level === 1) {
                        op_type = type;
                        op_index = i;
                    }
                    break;
                default:
                    --op_count;
                    break;
            }
        }

        if (op_count === 0) {
            let expr = {};
            const token = tokens[start];

            if (token.type === token_type.LEFT_PAREN) {
                if (tokens[end - 1].type !== token_type.RIGHT_PAREN) {
                    throw "expected ')' after '('";
                }
                return parse(start + 1, end - 1);
            }

            if (token.type !== token_type.IDENTIFIER) {
                throw "expected identifier";
            }

            switch (tokens[start + 1].type) {
                case token_type.LEFT_PAREN:
                    expr = { kind: expr_type.FN_CALL, name: token.value, args: [] };
                    const paren = next_token_of_type(token_type.RIGHT_PAREN, start + 2, tokens.length);
                    if (paren === -1) {
                        throw "expected ')' after '('";
                    }
                    if (start + 2 !== paren) {
                        let comma = next_token_of_type(token_type.COMMA, start + 2, paren);
                        let i = start + 2;
                        while (comma !== -1 && comma + 1 < paren) {
                            expr.args.push(parse(i, comma));
                            i = comma + 1;
                            comma = next_token_of_type(token_type.COMMA, i, paren);
                        }
                        expr.args.push(parse(i, paren));
                    }
                    return expr;
                default:
                    if (token.value[0] === "\"") {
                        expr.kind = expr_type.VALUE;
                        expr.value = token.value;
                        expr.value_type = "string";
                    } else if (is_number(token.value)) {
                        expr.kind = expr_type.VALUE;
                        expr.value = token.value;
                        expr.value_type = "i32";
                    } else {
                        expr.kind = expr_type.VAR_USE;
                        expr.name = token.value;
                    }
                    return expr;
            }
        } else {
            let expr = {};
            expr.kind = expr_type.OP;
            expr.op_type = op_type;
            expr.lhs = parse(start, op_index);
            switch (op_type) {
                case token_type.ASSIGN:
                case token_type.ADD_ASSIGN:
                case token_type.SUBSTRACT_ASSIGN:
                case token_type.MULTIPLY_ASSIGN:
                case token_type.DIVIDE_ASSIGN:
                case token_type.MODULO_ASSIGN:
                    if (expr.lhs.type !== expr_type.VAR_USE) {
                        throw "expected a mutable variable";
                    }
                    break;
            }
            expr.rhs = parse(op_index + 1, end);
            return expr;
        }
    }

    function parse_block() {
        let old = var_stack;
        var_stack = { vars: [], before: old };

        const global_scope = index == 0;

        let vector = [];
        let expr_index = 0;
        let expr_state = 0;

        while (index < tokens.length) {
            const token = tokens[index];

            if (expr_index == vector.length) {
                let expr = { kind: -1 };

                main: switch (token.type) {
                    case token_type.RIGHT_PAREN:
                        throw "unexpected ')'";
                    case token_type.RIGHT_BRACE:
                        var_stack = var_stack.before;
                        return vector;
                    case token_type.MOD:
                        expr.kind = expr_type.MOD;
                        break;
                    case token_type.IDENTIFIER:
                        switch (tokens[index + 1].type) {
                            case token_type.COLON:
                                expr = { kind: expr_type.VAR_DEF, name: token.value };
                                ++index;
                                break main;
                            case token_type.LEFT_PAREN:
                                let i = index + 2;
                                while (i < tokens.length) {
                                    if (tokens[i].type === token_type.RIGHT_PAREN) {
                                        while (i < tokens.length) {
                                            if (tokens[i].type === token_type.LEFT_BRACE) {
                                                expr = { kind: expr_type.FN_DEF, name: token.value, args: [] };
                                                ++index;
                                                break main;
                                            }
                                            ++i;
                                        }
                                    }
                                    ++i;
                                }
                                break;
                        }
                    case token_type.IDENTIFIER:
                    case token_type.LEFT_PAREN:
                        {
                            const semicolon = next_token_of_type(token_type.SEMICOLON, index, tokens.length);
                            expr = parse(index, semicolon);
                            index = semicolon;
                            ++expr_index;
                        }
                        break;
                    case token_type.STRUCT:
                        expr.kind = expr_type.STRUCT;
                        break;
                    case token_type.ENUM:
                        expr.kind = expr_type.ENUM;
                        break;
                    case token_type.LEFT_BRACE:
                        expr.kind = expr_type.BLOCK;
                        ++index;
                        expr.body = parse_block();
                        ++expr_index;
                        break;
                    case token_type.IF:
                        expr.kind = expr_type.IF;
                        break;
                    case token_type.ELIF:
                        expr.kind = expr_type.ELIF;
                        break;
                    case token_type.ELSE:
                        expr.kind = expr_type.ELSE;
                        break;
                    case token_type.DO:
                        expr.kind = expr_type.DO;
                        break;
                    case token_type.WHILE:
                        expr.kind = expr_type.WHILE;
                        break;
                    case token_type.FOR:
                        expr.kind = expr_type.FOR;
                        break;
                    case token_type.RETURN:
                        expr.kind = expr_type.RETURN;
                        const semicolon = next_token_of_type(token_type.SEMICOLON, index, tokens.length);
                        if (semicolon === index + 1) {
                            expr.expr = null;
                        } else {
                            expr.expr = parse(index + 1, semicolon);
                        }
                        index = semicolon;
                        ++expr_index;
                        break;
                    case token_type.BREAK:
                        expr.kind = expr_type.BREAK;
                        break;
                    case token_type.NEXT:
                        expr.kind = expr_type.NEXT;
                        break;
                }

                if (expr.kind !== -1) {
                    vector.push(expr);
                    expr_state = 0;
                }
            } else {
                expr = vector[expr_index];

                switch (expr.kind) {
                    case expr_type.MOD:
                        switch (expr_state) {
                            case 0:
                                if (token.type === token_type.IDENTIFIER) {
                                    expr.name = token.value;
                                    ++expr_state;
                                } else {
                                    throw "expected module name after 'mod'";
                                }
                                break;
                            case 1:
                                if (token.type === token_type.LEFT_BRACE) {
                                    ++expr_state;
                                } else {
                                    throw "expected '{' after 'mod' name";
                                }
                                break;
                            case 2:
                                expr.body = parse_block();
                                ++expr_index;
                                break;
                        }
                        break;
                    case expr_type.FN_DEF:
                        switch (expr_state) {
                            case 0:
                                if (token.type === token_type.RIGHT_PAREN) {
                                    expr_state = 4;
                                } else if (token.type === token_type.IDENTIFIER) {
                                    const arg = { kind: expr_type.ARG, name: token.value };
                                    expr.args.push(arg);
                                    ++expr_state;
                                } else {
                                    throw "expected argument name or ')' after '('";
                                }
                                break;
                            case 1:
                                if (token.type === token_type.COLON) {
                                    ++expr_state;
                                } else {
                                    throw "expected ':' after argument name";
                                }
                                break;
                            case 2:
                                if (token.type === token_type.IDENTIFIER) {
                                    expr.args[expr.args.length - 1].type = token.value;
                                    ++expr_state;
                                } else {
                                    throw "expected argument type after ':'";
                                }
                                break;
                            case 3:
                                if (token.type === token_type.RIGHT_PAREN) {
                                    ++expr_state;
                                } else if (token.type === token_type.COMMA) {
                                    expr_state = 0;
                                } else {
                                    throw "expected ',' or ')' after argument type";
                                }
                                break;
                            case 4:
                                if (token.type === token_type.LEFT_BRACE) {
                                    expr_state = 6;
                                } else if (!expr.type && token.type === token_type.ARROW) {
                                    ++expr_state;
                                } else {
                                    throw "expected '->' or '{' after ')'";
                                }
                                break;
                            case 5:
                                if (token.type === token_type.IDENTIFIER) {
                                    expr.type = token.value;
                                    expr_state = 4;
                                } else {
                                    throw "expected funtion return type after ':'";
                                }
                                break;
                            case 6:
                                expr.body = parse_block();
                                ++expr_index;
                                break;
                        }
                        break;
                    case expr_type.VAR_DEF:
                        switch (expr_state) {
                            case 0:
                                if (token.type === token_type.IDENTIFIER) {
                                    expr.type = token.value;
                                    ++expr_state;
                                } else if (token.type === token_type.ASSIGN) {
                                    expr_state = 2;
                                } else {
                                    throw "expected ':' after variable name";
                                }
                                break;
                            case 1:
                                if (token.type === token_type.ASSIGN) {
                                    ++expr_state;
                                } else if (token.type === token_type.SEMICOLON) {
                                    ++expr_index;
                                } else {
                                    throw "expected '='or ';' after variable type";
                                }
                                break;
                            case 2:
                                const semicolon = next_token_of_type(token_type.SEMICOLON, index, tokens.length);
                                expr.value = parse(index, semicolon);
                                index = semicolon;
                                ++expr_index;
                                break;
                        }
                        break;
                    case expr_type.STRUCT:
                        switch (expr_state) {
                            case 0:
                                if (token.type === token_type.IDENTIFIER) {
                                    expr.name = token.value;
                                    ++expr_state;
                                } else {
                                    throw "expected struct name after 'struct'";
                                }
                                break;
                            case 1:
                                if (token.type === token_type.LEFT_BRACE) {
                                    expr.body = [];
                                    expr_state = 3;
                                } else {
                                    throw "expected '{' after struct name";
                                }
                                break;
                            case 2:
                                if (token.type === token_type.COMMA) {
                                    ++expr_state;
                                } else if (token.type === token_type.RIGHT_BRACE) {
                                    ++expr_index;
                                } else {
                                    throw "',' or '}' after variable type";
                                }
                                break;
                            case 3:
                                if (token.type === token_type.RIGHT_BRACE) {
                                    ++expr_index;
                                } else if (token.type === token_type.IDENTIFIER) {
                                    expr.body.push({ kind: expr_type.ARG, name: token.value });
                                    ++expr_state;
                                } else {
                                    throw "field name or '}' after '{'";
                                }
                                break;
                            case 4:
                                if (token.type === token_type.COLON) {
                                    ++expr_state;
                                } else {
                                    throw "':' after variable name";
                                }
                                break;
                            case 5:
                                if (token.type === token_type.IDENTIFIER) {
                                    expr.body[expr.body.length - 1].type = token.value;
                                    expr_state = 2;
                                } else {
                                    throw "field type after ':'";
                                }
                                break;
                        }
                        break;
                    case expr_type.ENUM:
                        switch (expr_state) {
                            case 0:
                                if (token.type === token_type.IDENTIFIER) {
                                    expr.name = token.value;
                                    ++expr_state;
                                } else {
                                    throw "enum name after 'enum'";
                                }
                                break;
                            case 1:
                                if (token.type === token_type.LEFT_BRACE) {
                                    expr.body = [];
                                    expr_state = 3;
                                } else {
                                    throw "'{' after enum name";
                                }
                                break;
                            case 2:
                                if (token.type === token_type.COMMA) {
                                    ++expr_state;
                                } else if (token.type === token_type.RIGHT_BRACE) {
                                    ++expr_index;
                                } else {
                                    throw "'}' or ',' after '{'";
                                }
                                break;
                            case 3:
                                if (token.type === token_type.RIGHT_BRACE) {
                                    ++expr_index;
                                } else if (token.type === token_type.IDENTIFIER) {
                                    expr.body.push({ kind: expr_type.STRUCT, name: token.value, body: [] });
                                    ++expr_state;
                                } else {
                                    throw "option name or '}' after '{'";
                                }
                                break;
                            case 4:
                                if (token.type === token_type.LEFT_PAREN) {
                                    expr_state = 6;
                                } else if (token.type === token_type.COMMA) {
                                    expr_state = 3;
                                } else if (token.type === token_type.RIGHT_BRACE) {
                                    ++expr_index;
                                } else {
                                    throw "'(' or ',' after option name";
                                }
                                break;
                            case 5:
                                if (token.type === token_type.COMMA) {
                                    ++expr_state;
                                } else if (token.type === token_type.RIGHT_PAREN) {
                                    expr_state = 2;
                                } else {
                                    throw "',' or ')' after field type";
                                }
                                break;
                            case 6:
                                if (token.type === token_type.RIGHT_PAREN) {
                                    expr_state = 2;
                                } else if (token.type === token_type.IDENTIFIER) {
                                    expr.body[expr.body.length - 1].body.push({ kind: expr_type.ARG, name: token.value });
                                    ++expr_state;
                                } else {
                                    throw "field name or ')' after '('";
                                }
                                break;
                            case 7:
                                if (token.type === token_type.COLON) {
                                    ++expr_state;
                                } else {
                                    throw "':' after field name";
                                }
                                break;
                            case 8:
                                if (token.type === token_type.IDENTIFIER) {
                                    const v = expr.body[expr.body.length - 1].body;
                                    v[v.length - 1].type = token.value;
                                    expr_state = 5;
                                } else {
                                    throw "field type after ':'";
                                }
                                break;
                        }
                        break;
                    case expr_type.IF:
                    case expr_type.ELIF:
                    case expr_type.WHILE:
                        const brace = next_token_of_type(token_type.LEFT_BRACE, index, tokens.length);
                        console.log(brace);
                        if (brace !== -1) {
                            expr.con = parse(index, brace);
                            index = brace + 1;
                            expr.body = parse_block();
                            ++expr_index;
                        }
                        else {
                            switch (expr.kind) {
                                case expr_type.IF:
                                    throw "expected '{' after 'if'";
                                case expr_type.ELIF:
                                    throw "expected '{' after 'elif'";
                                case expr_type.WHILE:
                                    throw "expected '{' after 'while'";
                            }
                        }
                        break;
                    case expr_type.ELSE:
                        switch (expr_state) {
                            case 0:
                                if (token.type == token_type.LEFT_BRACE) {
                                    ++expr_state;
                                } else {
                                    throw "'{' after 'else'";
                                }
                                break;
                            case 1:
                                expr.body = parse_block();
                                ++expr_index;
                                break;
                        }
                        break;
                    case expr_type.DO:
                        break;
                }
            }

            ++index;
        }

        if (global_scope) {
            return vector;
        }

        throw "expected '}' token";
    }

    console.log("");
    console.log("Tokens:");
    console.log(tokens);
    console.log("");

    const ast = parse_block();

    console.log("AST:");
    console.log(JSON.stringify(ast));

    // #endregion

    // #region Generator

    function generate_type(type) {
        switch (type) {
            case null:
                code += "void";
                break;
            case "uint":
                code += "unsigned int";
                break;
            case "i32":
                code += "int32_t";
                break;
            case "f32":
                code += "float";
                break;
            case "i8":
                code += "int8_t";
                break;
            case "f64":
                code += "double";
                break;
            case "u32":
                code += "uint32_t";
                break;
            case "i64":
                code += "int64_t";
                break;
            case "i16":
                code += "int16_t";
                break;
            case "u8":
                code += "uint8_t";
                break;
            case "u64":
                code += "uint64_t";
                break;
            case "u16":
                code += "uint16_t";
                break;
            default:
                code += type;
                break;
        }
    }

    function prefix_name(name) {
        return mod.concat(name).join("_");
    }

    function generate_expr(expr, last, semicolon, parens) {
        switch (expr.kind) {
            case expr_type.MOD:
                mod.push(expr.name);
                generate_block(expr.body, false, false);
                mod.pop();
                break;
            case expr_type.FN_DEF:
                generate_type(expr.type);
                code += " " + prefix_name(expr.name) + "(";
                generate_block(expr.args, false, false);
                code += "){";
                const temp = [...mod];
                mod = [];
                generate_block(expr.body, true, false);
                mod = temp;
                code += "}";
                break;
            case expr_type.ARG:
                generate_type(expr.type);
                code += " " + expr.name;
                if (semicolon) {
                    code += ";";
                } else if (!last) {
                    code += ",";
                }
                break;
            case expr_type.FN_CALL:
                code += expr.name.replace("::", "_") + "(";
                generate_block(expr.args, false, true);
                code += ")";
                if (semicolon) {
                    code += ";";
                }
                break;
            case expr_type.VAR_DEF:
                generate_type(expr.type);
                code += " " + prefix_name(expr.name);
                if (expr.value) {
                    code += "=";
                    generate_expr(expr.value, false, true, false);
                } else {
                    code += ";";
                }
                break;
            case expr_type.STRUCT:
                code += "typedef struct{";
                generate_block(expr.body, true, false);
                code += "}" + prefix_name(expr.name) + ";";
                break;
            case expr_type.ENUM:
                let simple = true;
                for (let i = 0; i < expr.body.length; ++i) {
                    if (expr.body[i].body.length > 0) {
                        simple = false;
                    }
                }
                const name = prefix_name(expr.name);
                mod.push(expr.name);
                if (simple) {
                    code += "\n";
                    for (let i = 0; i < expr.body.length; ++i) {
                        code += "#define " + prefix_name(expr.body[i].name) + " " + i + "\n";
                    }
                } else {
                    generate_block(expr.body, true, false);
                    code += "typedef union{";
                    for (let i = 0; i < expr.body.length; i++) {
                        code += prefix_name(expr.body[i].name) + " _" + i + ";";
                    }
                    code += "}" + name + "Union;";
                    code += "typedef struct{" + name + "Union u;";
                    generate_type("uint");
                    code += " t;}" + name + ";";
                }
                mod.pop();
                break;
            case expr_type.VAR_USE:
                code += expr.name.replace("::", "_");
                if (semicolon) {
                    code += ";";
                }
                break;
            case expr_type.OP:
                if (parens) {
                    code += "(";
                }
                generate_expr(expr.lhs, false, false, parens + 1);
                switch (expr.op_type) {
                    case token_type.ASSIGN:
                        code += "=";
                        break;
                    case token_type.EQUAL:
                        code += "==";
                        break;
                    case token_type.LESS:
                        code += "<";
                        break;
                    case token_type.LESS_EQUAL:
                        code += "<=";
                        break;
                    case token_type.GREATER:
                        code += ">";
                        break;
                    case token_type.GREATER_EQUAL:
                        code += ">=";
                        break;
                    case token_type.ADD:
                        code += "+";
                        break;
                    case token_type.ADD_ASSIGN:
                        code += "+=";
                        break;
                    case token_type.SUBSTRACT:
                        code += "-";
                        break;
                    case token_type.SUBSTRACT_ASSIGN:
                        code += "-=";
                        break;
                    case token_type.MULTIPLY:
                        code += "*";
                        break;
                    case token_type.MULTIPLY_ASSIGN:
                        code += "*=";
                        break;
                    case token_type.DIVIDE:
                        code += "/";
                        break;
                    case token_type.DIVIDE_ASSIGN:
                        code += "/=";
                        break;
                    case token_type.MODULO:
                        code += "%";
                        break;
                    case token_type.MODULO_ASSIGN:
                        code += "%=";
                        break;
                    case token_type.NOT:
                        code += "!";
                        break;
                    case token_type.NOT_EQUAL:
                        code += "!=";
                        break;
                }
                generate_expr(expr.rhs, false, false, parens + 1);
                if (parens) {
                    code += ")";
                }
                if (semicolon) {
                    code += ";";
                }
                break;
            case expr_type.VALUE:
                code += expr.value;
                if (semicolon) {
                    code += ";";
                }
                break;
            case expr_type.ARRAY:
                break;
            case expr_type.BLOCK:
                code += "{";
                generate_block(expr.body, true, false);
                code += "}";
                break;
            case expr_type.ELIF:
                code += "else ";
            case expr_type.IF:
                code += "if(";
                generate_expr(expr.con, false, false, false);
                code += "){";
                generate_block(expr.body, true, false);
                code += "}";
                break;
            case expr_type.ELSE:
                code += "else{";
                generate_block(expr.body, true, false);
                code += "}";
                break;
            case expr_type.DO:
                break;
            case expr_type.WHILE:
                code += "while(";
                generate_expr(expr.con, false, false, false);
                code += "){";
                generate_block(expr.body, true, false);
                code += "}";
                break;
            case expr_type.FOR:
                break;
            case expr_type.RETURN:
                code += "return";
                if (expr.expr !== null) {
                    code += " ";
                    generate_expr(expr.expr, false, true, false);
                }
                break;
            case expr_type.BREAK:
                code += "break;";
                break;
            case expr_type.NEXT:
                code += "continue;";
                break;
        }
    }

    function generate_block(block, semicolon, comma) {
        for (let i = 0; i < block.length; ++i) {
            const last = i + 1 === block.length;
            generate_expr(block[i], last, semicolon, false);
            if (comma && !last) {
                code += ",";
            }
        }
    }

    generate_block(ast);

    // #endregion

}

fs.writeFileSync(output, code);