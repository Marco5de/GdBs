#include "workers.h"
#include "semaphores.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

// AUFGABENSTELLUNG:
// Bringen Sie diese Programm mittels Semaphoren in Ordnung.
// Wichtig: Aenderungen an einem Datensatz sollen nur diesen
// einen Datensatz sperren.

#define INCREMENTS_PRO_WORKER 10000

//-----------------------------------------------------------------------------
// alle globalen variablen fuer die beiden worker hier definieren,
// alle unbedingt mit "volatile" !!!
//-----------------------------------------------------------------------------

// implementiert wird eine datenbank in einer temporaeren Datei mit
// fuenf datensaetzen (je ein int)

char db_filename[20]={"temp_db_XXXXXX"};

//-----------------------------------------------------------------------------
// bevor der test beginnt wird test_setup() einmal aufgerufen
//-----------------------------------------------------------------------------

void test_setup(void) {
  printf("Test Setup\n");
  readers=0;
  writers=3;

  // datenbank anlegen
  int db=mkstemp(db_filename);
  int nulldata[5]={0,0,0,0,0};
  write(db, nulldata, sizeof(nulldata));
  close(db);

  srandom(time(NULL));
}

//-----------------------------------------------------------------------------
// wenn beider worker fertig sind wird test_end() noch aufgerufen
//-----------------------------------------------------------------------------

void test_end(void) {
  int result[5];
  int db=open(db_filename, O_RDONLY);
  read(db, &result, sizeof(result));
  close(db);
  unlink(db_filename);

  printf("Einzelergebnisse: %i %i %i %i %i\n", result[0], result[1], result[2], result[3], result[4]);
  int sum = result[0] + result[1] + result[2] + result[3] + result[4];
  int expected=writers*INCREMENTS_PRO_WORKER;
  printf("Test End: %i, Erwartet: %i\n", sum, expected);
  if (sum==expected) {
    printf("Test ok\n");
  } else {
    printf("Test failed\n");
  }
}

//-----------------------------------------------------------------------------
// die beiden worker laufen parallel:
//-----------------------------------------------------------------------------

void reader(long my_id) {
  printf("Wer hat mich da aufgerufen?\n");
  exit(1);
}

void writer(long my_id) {
  int i;
  int db=open(db_filename, O_RDWR);
  for (i=0; i<INCREMENTS_PRO_WORKER; i++) {
    int idx=random()%5; // 0..4 == diesen zufaelligen Datensatz hochzaehlen
    int val;
    lseek(db, idx*sizeof(int), SEEK_SET);
    read(db, &val, sizeof(int));
    val++;
    lseek(db, idx*sizeof(int), SEEK_SET);
    write(db, &val, sizeof(int));
  }
  close(db);
}
