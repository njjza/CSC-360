default: main_debug

Queue:
	gcc -Wall ./lib/Queue/Queue.c -I./include -c -o ./bin/Queue.o -g 

Customer:
	gcc -Wall ./lib/Customer/Customer.c -I./include -c -o ./bin/Customer.o -g

Clerk:
	gcc -Wall ./lib/Clerk/Clerk.c -I./include -c -o ./bin/Clerk.o -g

Main:
	gcc -c -I./include ./main.c -DDEBUG -o ./bin/main.o -lm -g

main_debug: Queue Customer Clerk Main
	gcc ./bin/main.o ./bin/Customer.o ./bin/Clerk.o ./bin/Queue.o -pthread -o ./build/main.exe -g
	@read -p "Input file path is: " FILEPATH;\
	echo "./build/main.exe ./input/$$FILEPATH.txt";\
	./build/main.exe ./input/$$FILEPATH.txt