const fs = require('fs');
const { stdout } = require('process');
const { exec } = require("child_process");
const lexer = require('./lexer.js');
const parser = require('./parser.js');
const cgen = require('./cgen.js');

function lex_parse_recursive(path) {
    const files = fs.readdirSync(path, { withFileTypes: true });
    for (let i = 0; i < files.length; ++i) {
        const file = files[i];
        if (file.isDirectory()) {
            lex_parse_recursive(path + '/' + file.name);
        } else {
            const contents = fs.readFileSync(path + '/' + file.name).toString();
            console.log('````````````````````````````````````````');
            console.log('File:', file.name);
            const tokens = lexer.lex(contents);
            console.log('Tokens:', tokens);
            const ast = parser.parse(tokens);
            stdout.write('AST: ');
            console.dir(ast, { depth: null });
            const c = cgen.generate(ast);
            console.log('C:', c);
            fs.writeFileSync('out.c', c);
            exec('rm out && cc out.c -o out');
        }
    }
}

lex_parse_recursive('tests/main/src');