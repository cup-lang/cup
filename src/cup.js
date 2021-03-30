const fs = require('fs');
const { stdout } = require('process');
const lexer = require('./lexer.js');
const parser = require('./parser.js');
const cgen = require('./cgen.js');

function compileRecursive(mods, path) {
    const files = fs.readdirSync(path, { withFileTypes: true });
    let allAst = [];
    for (let i = 0; i < files.length; ++i) {
        let file = files[i];
        if (file.isDirectory()) {
            const ast = compileRecursive(mods.concat(file.name), path + '/' + file.name);
            allAst = allAst.concat(ast);
        } else {
            file = path + '/' + file.name;
            const contents = fs.readFileSync(file).toString();
            const tokens = lexer.lex(contents);
            let ast = parser.parse(tokens);
            if (mods.length) {
                ast = {
                    kind: parser.exprKind.MOD,
                    name: mods[mods.length - 1],
                    body: ast
                };
            }
            allAst = allAst.concat(ast);

            // console.log('````````````````````````````````````````');
            // console.log('File:', file);
            // console.log('Tokens:', tokens);
            // stdout.write('AST: ');
            // console.dir(ast, { depth: null });
        }
    }
    return allAst;
}

let ast = compileRecursive([], 'tests/main/src');
console.dir(ast, { depth: null });
const c = cgen.generate(ast);
fs.writeFileSync('out.c', c);