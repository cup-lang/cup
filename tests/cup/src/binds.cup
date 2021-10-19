#os("win") mod _ {
    #req("windows.h") mod _ {};
};

#req("stdint.h") mod _ {};

#gen("T")
mod math {
    T min(T a, T b) {
        if (a) < b {
            ret a;
        };
        ret b;
    };

    T max(T a, T b) {
        if a > b {
            ret a;
        };
        ret b;
    };

    #bind("pow") sub pow() {};
    #bind("log10") sub log10() {};
};

#req("time.h")
mod time {
    #bind("clock") sub now() {};
};

#req("stdlib.h")
mod mem {
    #bind("malloc") sub alloc() {};
    #bind("realloc") sub realloc() {};
    #bind("sizeof") sub size() {}; 
    #bind("free") sub free() {}; 

    #req("string.h")
    #bind("memcpy")
    sub copy() {};
};

#req("stdio.h")
mod fmt {
    #bind("printf") sub print() {};
    #bind("vprintf") sub vprint() {};
};

#req("string.h")
mod str {
    #bind("strcmp") sub cmp() {};
    #bind("strlen") sub len() {};
    #bind("strcpy") sub copy() {};
};

#raw("typedef struct dirent dirent;")
#req("dirent.h")
mod dir {
    #bind("opendir") sub open() {};
    #bind("readdir") sub read() {};
    #bind("rewinddir") sub rewind() {};
    #bind("closedir") sub close() {};    
};

#req("stdio.h")
mod file {
    #bind("fopen") sub open() {};
    #bind("fclose") sub close() {};
    #bind("fread") sub read() {};
    #bind("fseek") sub seek() {};
    #bind("ftell") sub size() {};
    #bind("rewind") sub rewind() {};
    #bind("fprintf") sub print() {};
};

#req("ctype.h")
mod ch {
    #bind("isspace") sub is_space() {};
    #bind("isdigit") sub is_num() {};
    #bind("isalpha") sub is_alpha() {};
    #bind("isalnum") sub is_alpha_num() {};
};