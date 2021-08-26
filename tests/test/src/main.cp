` lexer
    ` raw strings
` parser
    ` add enum_dest
` analyzer
    ` global
        ` ✓ register types ✓
        ` ✓ check for collisions ✓
    ` local
        ` binds (#single, #allow)
        ` ✓ apply mods ✓
        ` check path existance (foo:bar abc:dfg = xyz:ooo)
            ` is_type
            ` is_sub
            ` is_var
        ` check type
            ` when assigning check if type matches
            ` when giving to sub_call check
            ` when giving to comp_inst
            ` when giving to enum_inst
            ` when ops
        ` check number of args for
            ` tag
            ` sub_call
            ` enum_inst
            ` comp_inst
        ` check if label defined
        ` check if comp_inst fields exist
        ` check if enum_dest fields exist
        ` apply transformations
            ` custom ops
            ` enum_inst (==, !=)
            ` #self
                ` this
                ` sub_calls (.)
        ` type inference for var_defs
` playground
    ` multiple files
    ` line numbers
` language server
    ` parser error tolerance
` parser
    ` combo ops
    ` combo var_defs

#pub sub foo() {
    echo 1 + 1;    
};

echo 2 + 2;    