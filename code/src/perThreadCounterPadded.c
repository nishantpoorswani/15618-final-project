#include <iostream>
#include <cstdlib>
#include <pthread.h>

using namespace std;

#define NUM_THREADS 4

// allocate per-thread variable for local per-thread accumulation
struct PerThreadState {
   int myPerThreadCounter;
   char padding[64 - sizeof(int)];
};
PerThreadState myPerThreadCounter[NUM_THREADS];

void *perThreadCounter(void *threadid) {
      for(int i=0; i<100; i++)
      {
         myPerThreadCounter[*(int *)threadid].myPerThreadCounter++;
      }
      free(threadid);
}

int main () {
   pthread_t threads[NUM_THREADS];
   int rc;
   int i;
   
   for( i = 1; i < NUM_THREADS; i++ ) {
      int *p = (int *) malloc(sizeof(int));
      *p = i;
      rc = pthread_create(&threads[i], NULL, perThreadCounter, (void *)p);
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }
   for(int i=0; i<100; i++)
   {
      myPerThreadCounter[0].myPerThreadCounter++;
   }
   pthread_exit(NULL);
}