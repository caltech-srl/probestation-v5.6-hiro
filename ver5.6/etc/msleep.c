/*
 * milisecond sleep
 *
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>


int main(int argc, char **argv)
{
    int ist1=1000;
    char st1[BUFSIZ];

    if (argc==2) {
       strcpy(st1,argv[1]);
       ist1=atoi(st1);

       //printf("%d \n",ist1);
    }
 
    usleep(ist1*1000);
    return 0;
}

