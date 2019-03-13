#include <pthread.h>
#include <stdio.h>
#include "code.c" 
#define NUM_THREADS     5

void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;
   int r = rand();
//   printf("Hello World! It's me, thread #%ld!\n", tid);
   printf("%ld - %d\n",pthread_self(),r); 
   void *a = (char *) thread_malloc ( r ) ;
   sleep(100);
   pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;
   for(t=1; t<=NUM_THREADS; t++){
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }

   /* Last thing that main() should do */
	  sleep(1);
   thread_memory_usage();
   sleep(100);
   pthread_exit(NULL);
}
