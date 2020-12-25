gcc ./src/cup.c -O3 -ggdb3 -o ./bin/cup
cd ./tests/main
valgrind --leak-check=full ../../bin/cup
cd ../..