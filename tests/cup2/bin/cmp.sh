cd ../../.. &&
node src/cup.js &&
cd tests/cup/bin &&
gcc -g out.c -o ../../cup2/bin/cup &&
cd ../../cup2/bin &&
./cup build -i ../src -o out.c