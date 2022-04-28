#include <iostream>
#include <cstdlib>
#include <pthread.h>
void PrintHello2(int threadid);

using namespace std;

#define NUM_THREADS 2

void *PrintHello(void *threadid) {
   long tid;
   tid = (long)threadid;
   cout << "Hello World! Thread ID, " << tid << endl;
   PrintHello2(tid);
   pthread_exit(NULL);
}

int main () {
   pthread_t threads[NUM_THREADS];
   int rc;
   int i;
   
   for( i = 0; i < NUM_THREADS; i++ ) {
      cout << "main() : creating thread, " << i << endl;
      rc = pthread_create(&threads[i], NULL, PrintHello, (void *)i);
      
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }
   //PrintHello2(1);
   //PrintHello2(5);
   //PrintHello2(15);
   pthread_exit(NULL);
}

void PrintHello2(int threadid) {
   cout << "Hello World! Thread ID, " << threadid << endl;
}