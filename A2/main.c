#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "./include/Queue.h"
#include "./include/Customer.h"

int FileParser(char* filepath, struct Customer** result);
int CompFunc(struct Customer* a, struct Customer* b) {
	return (a->arrival_time - b->arrival_time);
}

struct Queue *customer_list_in_time_order;

int main(int argc, char *argv[]) {
	if(argc < 2) {
		printf("Incorrect input format, please attach input file link\n");
		exit(EXIT_FAILURE);
	}

	char *file_path = argv[1];
	struct Customer **customer_list;
	int size = FileParser(file_path, customer_list);

	return 0;
}

int FileParser(char* filepath) {
	char buffer[256];
	struct Customer **li, *customer;
	int i = 0, len = 10;

	FILE *f = fopen(filepath, "r");
	if (!f) {
		printf("incorrect file name\n");
		exit(EXIT_FAILURE);
	}

	while(fgets(buffer, 256, f)) {
		customer = CustomerFactory(buffer);
		CUSTOMERLISTADD(li, customer, i, len);
		i++;
	}

	#undef CUSTOMERLISTADD
	
	result = li;
	fclose(f);
	return i;
}

