#include "workers.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef void *faketype(void *);


int readers = -1;
int writers = -1;

#define MAX_THREADS 20
int main(void) {
  pthread_t th_read[MAX_THREADS];
  int rc;

  test_setup();
  if ( readers<0
  ||   writers<0
  ||  (readers==0 && writers==0)
  ||   readers+writers>MAX_THREADS ) {
    printf("=============================================================\n");
    printf("readers und/oder writers wurden in test_setup() nicht sinnvoll gesetzt\n");
    printf("Bitte 'readers=7;' und 'writers=3;' oder so was aehnliches einfuegen!\n");
    printf("=============================================================\n");
    exit(1);
  }

  long i;
  for (i=0; i<readers+writers; i++) {
    if (i<readers) {
      rc = pthread_create(&th_read[i], NULL, (faketype *)reader, (void *) i);
    } else {
      rc = pthread_create(&th_read[i], NULL, (faketype *)writer, (void *) i-readers);
    }
    if (rc != 0 ) {
      printf("pthread_create error, rc= %d\n", rc);
      exit(1);
    }  
  }

  for (i=0; i<readers+writers; i++) {
    if (0!=(rc=pthread_join(th_read[i], NULL))) {
      perror("pthread_join fails");
      exit(1);
    }
  }

  test_end();

  exit(0);
}
