./cup build -i ../src -o out.c &&
gcc -g -Ofast out.c -o cup2 &&
./cup2 build -i ../../test/src -o ../../test/bin/new.c