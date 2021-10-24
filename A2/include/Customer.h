#ifndef CUSTOMER_H
#define CUSTOMER_H

struct Customer{ /// use this struct to record the customer information read from customers.txt
    int user_id;
	int class_type;
	int service_time;
	int arrival_time;
};


struct Customer * CustomerFactory(char *str);

#endif