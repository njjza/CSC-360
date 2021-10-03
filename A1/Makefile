Include = ./include
PMan_src = $(Include)/PMan/PMan.c 
PMan_h =  $(Include)/PMan/PMan.h
LL_src = $(Include)/LinkedList/LinkedList.c
LL_h = $(Include)/LinkedList/LinkedList.h
default: main_debug

LinkedList:
	gcc -Wall $(LL_src) -c -o ./bin/LinkedList.o -g

PMan: LinkedList $(LL_h)
	gcc -Wall $(PMan_src) -std=gnu90 -c -DDEBUG -o ./bin/PMan.o -g

Main: $(PMan_h)
	gcc -c ./main.c -DDEBUG -o ./bin/main.o -g

main_debug: PMan Main LinkedList
	gcc ./bin/main.o ./bin/PMan.o ./bin/LinkedList.o -lreadline -o ./build/main.exe -g
	./build/main.exe