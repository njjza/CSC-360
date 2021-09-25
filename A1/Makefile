.phony all:
all: pman

pman: PMan.c
	gcc -Wall PMan.c -lreadline -D DEBUG -o PMan -g

.PHONY clean:
clean:
	-rm -rf *.o *.exe
