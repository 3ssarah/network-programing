#include <stdio.h>
#include <stdlib.h>

void DieWithError(char *msg){

    perror(msg);
    exit(1);
}
