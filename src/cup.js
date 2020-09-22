const fs = require("fs");

const commands = {
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

const commandAliases = [
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
        return commands.HELP;
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

    command = commandAliases.indexOf(command);

    return command === -1 ? commands.NONE : command + 2;
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

if (argc === 2 || command === commands.HELP) {
    console.log("Cup Toolkit v0.0.1\n");
    console.log("USAGE:\n    cup [COMMAND] [OPTIONS]\n");
    console.log("COMMANDS:");
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
} else if (command === commands.NONE) {
    console.error(`error: no such command: '${process.argv.slice(2).join(" ")}'`);
    console.error("\nSee 'cup help' for the list of available commands.");
    process.exit(1);
}

switch (command) {
    case commands.RUN:
    case commands.BUILD:
        break;
    case commands.HELP_RUN:
        commandHelp("run", "Compile and run the current package", COMPILE_OPTIONS);
    case commands.HELP_BUILD:
        commandHelp("build", "Compile the current package", COMPILE_OPTIONS);
    case commands.HELP_CHECK:
        commandHelp("check", "Analyze the current package", NO_OPTIONS);
    case commands.HELP_NEW_PACKAGE:
        commandHelp("new", "Create a new package", NO_OPTIONS);
    case commands.HELP_UPDATE_PACKAGE:
        commandHelp("update", "Update given dependency", NO_OPTIONS);
    case commands.HELP_ADD_PACKAGE:
        commandHelp("add", "Adds given dependency", NO_OPTIONS);
    case commands.HELP_REMOVE_PACKAGE:
        commandHelp("remove", "Removes given dependency", NO_OPTIONS);
    case commands.HELP_GEN_DOCS:
        commandHelp("gen docs", "Generate documentation for the current package", NO_OPTIONS);
    case commands.HELP_GEN_BINDS:
        commandHelp("gen binds", "Generate bindings for a given C header file", NO_OPTIONS);
    case commands.HELP_SELF_UPDATE:
        commandHelp("self update", "Update the Cup Toolkit", NO_OPTIONS);
    case commands.HELP_SELF_INSTALL:
        commandHelp("self install", "Install the Cup Toolkit", NO_OPTIONS);
    case commands.HELP_SELF_UNINSTALL:
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
    console.error(`\nSee 'cup help ${commandAliases[command - 2]}' for the list of available options.`);
    process.exit(1);
}

var file;

try {
    file = fs.readFileSync(input).toString();
} catch (err) {
    console.log("error: no such file or directory: '%s'", file_name);
    process.exit(1);
}

const tokens = {
    // General
    IDENTIFIER: 0,
    PUBLIC: 1,
    MODULE: 2,
    FUNCTION: 3, // REMOVE
    VARIABLE: 4, // REMOVE
    STRUCT: 5,
    ENUM: 6,
    IN: 7,
    SEMICOLON: 8,
    COLON: 9,
    COMMA: 10,
    DOT: 11,
    // Flow control
    IF: 12,
    ELIF: 13,
    ELSE: 14,
    DO: 15,
    WHILE: 16,
    FOR: 17,
    RETURN: 18,
    BREAK: 19,
    NEXT: 20,
    // Brackets
    LEFT_PAREN: 21,
    RIGHT_PAREN: 22,
    LEFT_BRACE: 23,
    RIGHT_BRACE: 24,
    LEFT_SQUARE: 25,
    RIGHT_SQUARE: 26,
    // Operators
    RANGE: 27,
    ASSIGN: 28,
    EQUAL: 29,
    NOT_EQUAL: 30,
    NOT: 31,
    LESS: 32,
    LESS_EQUAL: 33,
    GREATER: 34,
    GREATER_EQUAL: 35,
    ADD: 36,
    ADD_ASSIGN: 37,
    SUBSTRACT: 38,
    SUBSTRACT_ASSIGN: 39,
    MULTIPLY: 40,
    MULTIPLY_ASSIGN: 41,
    DIVIDE: 42,
    DIVIDE_ASSIGN: 43,
    MODULO: 44,
    MODULO_ASSIGN: 45
}

let tokens = [];
let value = "";
let comment = false;
for (let i = 0; i < file.length; i++) {
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

    let type = tokens.IDENTIFIER;

    if (/\s/.test(char)) {
        type = -1;
    }
}