#include "workers.h"
#include "semaphores.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define NUMBERS_CREATED_PER_WRITER 5000

//-----------------------------------------------------------------------------
// alle globalen variablen fuer die beiden worker hier definieren,
// alle unbedingt mit "volatile" !!!
//-----------------------------------------------------------------------------
volatile semaphore procSem[4];
// der ringpuffer:

#define SIZE 4

volatile struct RINGPUFFER {
  int feld[SIZE];
} ringpuffer;

//-----------------------------------------------------------------------------
// bevor der test beginnt wird test_setup() einmal aufgerufen
//-----------------------------------------------------------------------------

void test_setup(void) {
  printf("Test Setup\n");

  int pos_startone = 0;
  ringpuffer.feld[pos_startone] = 1;

  for(int i=0; i<4; i++) {
	if(i == pos_startone) procSem[i] = sem_init(1);
	else procSem[i] = sem_init(0);
  }

  readers=0;
  writers=4;


}

//-----------------------------------------------------------------------------
// wenn beider worker fertig sind wird test_end() noch aufgerufen
//-----------------------------------------------------------------------------

void test_end(void) {
  printf("--End--\n");
}

//-----------------------------------------------------------------------------
// die beiden worker laufen parallel:
//-----------------------------------------------------------------------------

void writer(long my_id) {
  int i;
  int me = (int) my_id;
  int nxt = (me+1)%4;
  for (i=1; i<=NUMBERS_CREATED_PER_WRITER; i++) {
	sem_p(procSem[me]);
	if(ringpuffer.feld[me] != 1) {
		printf("Fehler: Keine 1 zum Weitergeben!");
		exit(1);
	}
    ringpuffer.feld[nxt] = 1;
	ringpuffer.feld[me] = 0;
	printf("%i>%i", me, nxt);
	sem_v(procSem[nxt]);
  }
}

void reader(long my_id) {
  // es darf nur einen leser geben (SS14: die Anleitung ist falsch! Nur writers erhoehen!)
  printf("Wer hat mich aufgerufen?!");
  exit(0);
}
