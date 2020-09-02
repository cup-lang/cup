gcc src\cup.c src\cup.res -O3 -s -o bin\cup.exe
REM tcc src\cup.c -o bin\cup.exe
REM cl src\cup.c /O2 /Fe:bin\cup.exe
REM clang src\cup.c -Ofast -o bin\cup.exe
bin\cup.exe test\test0\main.cup -o test\test0\main.c