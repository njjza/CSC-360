#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if(argc < 3) {
        return 0;
    }

    printf("process started\n");

    for(int i = 0; i < 10; i++) {
        printf("midd process check, pid = %s\n", argv[1]);
        sleep(atoi(argv[2]));
    }
    
    printf("process end\n");

    return 0;
}