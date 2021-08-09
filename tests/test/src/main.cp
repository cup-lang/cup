` vec -> darr
` analyzer
    ` global
        ` register types (#pub)
        ` check for collisions
    ` local
        ` apply mods
        ` type inference for var_defs
        ` apply custom ops
        ` check type
            ` when assigning check if type matches
            ` when giving to sub_call check
            ` when giving to comp_inst
            ` when giving to enum_inst
            ` when ops
        ` binds
        ` check path existance
        ` check if label defined
        ` check number of args for
            ` tag
            ` sub_call
            ` enum_inst
            ` comp_inst
        ` check if comp_inst fields exist
        ` apply transformations
            ` enum_inst (==, !=)
            ` #self
                ` this
                ` sub_calls (.)
` playground
    ` multiple files
    ` line numbers
` language server
    ` parser error tolerance
` parser
    ` combo ops
    ` combo var_defs

int main() {
    ret 0;
};