cd D:\MinGW\bin
gcc D:\Projects\Cache\src\cachec.c -o D:\Projects\Cache\bin\cachec.exe -O3
cd D:\Projects\Cache
bin\cachec.exe test\test0\main.ch -o test\test0\main.c

REM cl src\cachec.c /O2 /Fe:bin\cachec.exe
REM bin\cachec.exe test\test0\main.ch -o test\test0\main.c