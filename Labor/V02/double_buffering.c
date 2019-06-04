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

#define BUFFERSIZE 4096

//-----------------------------------------------------------------------------
// alle globalen variablen fuer die beiden worker hier definieren,
// alle unbedingt mit "volatile" !!!
//-----------------------------------------------------------------------------

char tempfilename[]="temporary_XXXXXX";
int testfile;

volatile char buffers[2][BUFFERSIZE];

//-----------------------------------------------------------------------------
// bevor der test beginnt wird test_setup() einmal aufgerufen
//-----------------------------------------------------------------------------

void test_setup(void) {
  printf("Test Setup\n");
  readers=1;
  writers=1;

  // testdatei erzeugen und befuellen
  int i;
  unsigned char buff[BUFFERSIZE];
  for (i=0; i<BUFFERSIZE; i++) buff[i]=0;

  testfile=mkstemp(tempfilename);
  for (i=0; i<256; i++) {
    buff[i]=17;
    write(testfile, buff, BUFFERSIZE);
    buff[i]=0;
  }
  lseek(testfile, 0, SEEK_SET); // zurueck zum dateianfang zum lesen

  //srandom(time(NULL));
}

//-----------------------------------------------------------------------------
// wenn beider worker fertig sind wird test_end() noch aufgerufen
//-----------------------------------------------------------------------------

void test_end(void) {
  close(testfile);
  unlink(tempfilename);

  printf("Test End\n");
}

//-----------------------------------------------------------------------------
// die beiden worker laufen parallel:
//-----------------------------------------------------------------------------

void reader(long my_id) {
  int x=0;
  while (BUFFERSIZE==read(testfile, &(buffers[x]), BUFFERSIZE)) {
    x=1-x; // toggles x between 0 and 1
  }
}

void writer(long my_id) {
  int x=0;
  int block_num;
  for (block_num=0; block_num<256; block_num++) {
    int i;
    for (i=0; i<BUFFERSIZE; i++) {
      char b=buffers[x][i];
      if ((i==block_num && buffers[x][i]!=17)
      ||  (i!=block_num && buffers[x][i]!= 0)) {
        printf("Failed!\n");
        return;
      }
    }
    x=1-x;
  }
}
