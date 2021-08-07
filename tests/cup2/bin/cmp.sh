./cup build -i ../src -o out.c && 
gcc -Ofast out.c -o cup2 && 
./cup2 build -i ../../test/src