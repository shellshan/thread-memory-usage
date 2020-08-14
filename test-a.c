#include <pthread.h>
#include <stdio.h>
#include "thread-mem-cal.h"
#define NUM_THREADS     5

void *one( )
{
    printf("I am One\n");
    void *a = thread_malloc(2);
    void *b = thread_malloc(8);
    void *c = thread_malloc(5);
    thread_free(a);
    thread_free(c);
    thread_free(b);
    a = thread_malloc(55);
    //sleep(100);
    pthread_exit(NULL);
}
void *two( )
{
    printf("I am Two\n");
    void *a = thread_malloc(20);
    a = thread_malloc(10);
    a = thread_malloc(70);
    thread_free(a);
    //        sleep(100);
    pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
    int rc;
    pthread_t threads[NUM_THREADS];
    rc = pthread_create(&threads[0], NULL, one , NULL );
    if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
    rc = pthread_create(&threads[1], NULL, two , NULL );
    if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    void *a = thread_malloc(300);
    /* Last thing that main() should do */
    //      sleep(100);
    sleep(1);
    thread_memory_usage();
    pthread_exit(NULL);
}
