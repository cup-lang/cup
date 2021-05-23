const fs = require('fs');
const { stdout } = require('process');
const lexer = require('./lexer.js');
const parser = require('./parser.js');
const analyzer = require('./analyzer.js');
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
            console.log('Tokens:', tokens);
            console.log(file);
            let ast = parser.parse(tokens);
            stdout.write('AST: ');
            console.dir(ast, { depth: null });
            if (mods.length) {
                ast = {
                    kind: parser.exprKind.MOD,
                    name: mods[mods.length - 1],
                    body: ast
                };
            }
            allAst = allAst.concat(ast);
        }
    }
    return allAst;
}

let ast = compileRecursive([], 'tests/cup/src');
analyzer.analyze(ast);
// console.dir(ast, { depth: null });
const c = cgen.generate(ast);
fs.writeFileSync('tests/cup/bin/out.c', c);