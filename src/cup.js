const fs = require('fs');
const lexer = require('./lexer.js');
const parser = require('./parser.js');

function lex_parse_recursive(path) {
    const files = fs.readdirSync(path, { withFileTypes: true });
    for (let i = 0; i < files.length; ++i) {
        const file = files[i];
        if (file.isDirectory()) {
            lex_parse_recursive(path + '/' + file.name);
        } else {
            const contents = fs.readFileSync(path + '/' + file.name).toString();
            console.log("File:", file.name);
            const tokens = lexer.lex(contents);
            console.log("Tokens:", tokens);
            const ast = parser.parse(tokens);
            console.log("AST:", ast);
        }
    }
}

lex_parse_recursive('tests/main/src');