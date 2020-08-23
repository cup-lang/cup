cd C:\MinGW\bin
gcc C:\Projects\Cache\src\cache.c -o C:\Projects\Cache\bin\cache.exe -O3 -s
cd C:\Projects\cache
bin\cache.exe test\test0\main.ch -o test\test0\main.c

REM cl src\cache.c /O2 /Fe:bin\cache.exe
REM bin\cache.exe test\test0\main.ch -o test\test0\main.c