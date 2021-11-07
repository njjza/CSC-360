all: check_directory main Queue Customer Clerk TIME Main clean
.PHONY: all 

main: Queue Customer Clerk TIME Main
	gcc ./bin/main.o ./bin/Customer.o ./bin/Clerk.o ./bin/Queue.o ./bin/GetCurrentTime.o -pthread -o ./build/main.exe -g
	@read -p "Input file path is: " FILEPATH;\
	echo "./build/main.exe $$FILEPATH";\

Queue:
	gcc -Wall -I./include ./lib/Queue/Queue.c -c -o ./bin/Queue.o -g 

Customer:
	gcc -Wall -I./include ./lib/Customer/Customer.c -c -o ./bin/Customer.o -g

Clerk:
	gcc -Wall -I./include ./lib/Clerk/Clerk.c -c -o ./bin/Clerk.o -g

TIME:
	gcc -Wall -I./include ./lib/CurrentTime/GetCurrentTime.c -c -o ./bin/GetCurrentTime.o -g

Main:
	gcc -c -I./include ./main.c -DDEBUG -o ./bin/main.o -lm -g

clean:
	rm ./bin/*.o
	rm ./build/*.exe
