g++ -o linker ./src/linker.cpp 

./linker -hex -place=iv_table@0x0000 -place=text@0x4000 -o izlaz.hex ./tests/izlaz.o ./tests/izlaz1.o