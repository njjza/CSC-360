PMan_src = ./include/PMan/PMan.c 
PMan_h = ./include/PMan/PMan.h

default: main_debug

PMan:
	gcc -Wall $(PMan_src) -c -DDEBUG -o ./bin/PMan.o

Main: $(PMan_h)
	gcc -c ./main.c -DDEBUG -o ./bin/main.o

main_debug: PMan Main
	gcc ./bin/main.o ./bin/PMan.o -lreadline -o ./build/main.exe -g
	./build/main.exe
