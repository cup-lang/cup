@echo off
REM gcc src\cup.c src\cup.res -O3 -s -o bin\cup.exe
REM tcc src\cup.c -o bin\cup.exe
REM cl src\cup.c /O2 /Fe:bin\cup.exe
REM clang src\cup.c -Ofast -o bin\cup.exe
REM bin\cup build -i test\test0\main.cup -o test\test0\main.c
node src\cup.js build -i test\test0\main.cup -o test\test0\main.c