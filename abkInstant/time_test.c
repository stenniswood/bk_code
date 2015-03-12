#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

int main()
{
    struct timeval start, end;

    long mtime, seconds, useconds;    

    gettimeofday(&start, NULL);
    sleep(2.0);
    usleep(600);
    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    long ms = round(useconds / 1000.0);
    long us = useconds - ms;
    
	printf("Elapsed time: %ld sec \n", seconds );
    printf("            : %ld ms  \n", ms      );
    printf("            : %ld us  \n", us      );    

    return 0;
}

