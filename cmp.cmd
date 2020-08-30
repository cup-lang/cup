cd C:\MinGW\bin
gcc C:\Projects\cup\src\cup.c C:\Projects\cup\src\cup.res -o C:\Projects\cup\bin\cup.exe -O3 -s
cd C:\Projects\cup
bin\cup.exe test\test0\main.cup -o test\test0\main.c

REM cl src\cup.c /O2 /Fe:bin\cup.exe
REM bin\cup.exe test\test0\main.cup -o test\test0\main.c