@echo off
gcc src\cup.c -Isrc -O3 -s -o bin\cup.exe
cd tests\main
..\..\bin\cup
cd ..\..