@echo off
gcc src\cup.c -Isrc -O3 -s -o bin\cup.exe
cd ..\cup-test
..\cup\bin\cup
cd ..\cup