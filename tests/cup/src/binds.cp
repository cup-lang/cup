#os("win") mod _ {
    #req("windows.h") mod _ {};
};

#req("stdint.h") mod _ {};

#req("time.h")
mod time {
    #bind("clock") sub now() {};
};

#req("stdlib.h")
mod mem {
    #bind("malloc") sub alloc() {};
    #bind("realloc") sub realloc() {};
    #bind("sizeof") sub size() {}; 

    #req("string.h")
    #bind("memcpy")
    sub copy() {};
};

#req("string.h")
mod str {
    #bind("strcmp") sub cmp() {};
    #bind("strlen") sub len() {};
};

#req("stdio.h")
mod file {
    #bind("fopen") sub open() {};
    #bind("fclose") sub close() {};
    #bind("fread") sub read() {};
    #bind("fseek") sub seek() {};
    #bind("ftell") sub size() {};
    #bind("rewind") sub rewind() {};
};

#req("ctype.h")
mod num {
    #bind("isspace") sub is_space() {};
    #bind("isdigit") sub is_num() {};
    #bind("isalpha") sub is_alpha() {};
    #bind("isalnum") sub is_alpha_num() {};
};