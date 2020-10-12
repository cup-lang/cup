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

function getCommand() {
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

function commandHelp(name, desc, options) {
    console.log(desc);
    console.log("\nUSAGE:");
    console.log("cup " + name + " [OPTIONS]");
    console.log("\nOPTIONS:" + options);
    process.exit(1);
}

function duplicateOption(index) {
    console.error(`error: duplicate option: '${process.argv[index]}'`);
    process.exit(1);
}

const argc = process.argv.length;
const command = getCommand();

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
        commandHelp("run", "Compile and run the current package", COMPILE_OPTIONS);
    case command_type.HELP_BUILD:
        commandHelp("build", "Compile the current package", COMPILE_OPTIONS);
    case command_type.HELP_CHECK:
        commandHelp("check", "Analyze the current package", NO_OPTIONS);
    case command_type.HELP_NEW_PACKAGE:
        commandHelp("new", "Create a new package", NO_OPTIONS);
    case command_type.HELP_UPDATE_PACKAGE:
        commandHelp("update", "Update given dependency", NO_OPTIONS);
    case command_type.HELP_ADD_PACKAGE:
        commandHelp("add", "Adds given dependency", NO_OPTIONS);
    case command_type.HELP_REMOVE_PACKAGE:
        commandHelp("remove", "Removes given dependency", NO_OPTIONS);
    case command_type.HELP_GEN_DOCS:
        commandHelp("gen docs", "Generate documentation for the current package", NO_OPTIONS);
    case command_type.HELP_GEN_BINDS:
        commandHelp("gen binds", "Generate bindings for a given C header file", NO_OPTIONS);
    case command_type.HELP_SELF_UPDATE:
        commandHelp("self update", "Update the Cup Toolkit", NO_OPTIONS);
    case command_type.HELP_SELF_INSTALL:
        commandHelp("self install", "Install the Cup Toolkit", NO_OPTIONS);
    case command_type.HELP_SELF_UNINSTALL:
        commandHelp("self uninstall", "Uninstall the Cup Toolkit", NO_OPTIONS);
}

let input = "";
let output = "";

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
            duplicateOption(i);
        case "-o":
        case "--output":
            if (output.length === 0) {
                if (i + 1 < argc) {
                    output = process.argv[++i];
                }
                continue;
            }
            duplicateOption(i);
    }

    console.error(`error: invalid option: '${arg}'`);
    console.error(`\nSee 'cup help ${command_aliases[command - 2]}' for the list of available options.`);
    process.exit(1);
}

var file;

try {
    file = fs.readFileSync(input).toString();
} catch (err) {
    console.error("error: no such file or directory: '%s'", file_name);
    process.exit(1);
}

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

    if (char === "#") {
        comment = true;
        continue;
    }

    let type = token_type.IDENTIFIER;

    if (/\s/.test(char)) {
        type = -1;
    }

    switch (char) {
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
            type = token_type.COLON;
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
                type = token_type.DIVIDE;
                ++i;
            } else {
                type = token_type.DIVIDE_ASSIGN;
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
                tokens.push({ type: value_type, value: value, index: i - value.length });
            }
            value = "";
        }

        if (type !== -1) {
            tokens.push({ type: type, value: "", index: i });
        }
    } else {
        value += char;
    }
}

// #endregion

// #region Parser

const expr_type = {
    MOD: 0,
    FN_DEF: 0,
    ARG: 0,
    FN_CALL: 0,
    VAR_DEF: 0,
    STRUCT: 0,
    ENUM: 0,
    VAR_USE: 0,
    OP: 0,
    VALUE: 0,
    ARRAY: 0,
    BLOCK: 0,
    IF: 0,
    ELIF: 0,
    ELSE: 0,
    DO: 0,
    WHILE: 0,
    FOR: 0,
    RETURN: 0
};

// #endregion