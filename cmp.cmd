@echo off
REM tcc src\cup.c -o bin\cup.exe
REM cl src\cup.c /O2 /Fe:bin\cup.exe
REM clang src\cup.c -Ofast -o bin\cup.exe
gcc src\cup.c -O3 -s -o bin\cup.exe
cd ..\cup-test
..\cup\bin\cup
cd ..\cup