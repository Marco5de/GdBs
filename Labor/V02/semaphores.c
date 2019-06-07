#include "semaphores.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void perror(const char *s);
#include <errno.h>


// drei mal fuer semget()
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>


struct SEMAPHORE {
  int count;
  key_t key;
};

//--- garbage collection: destroy all semaphores atexit()
// essential, since semaphores are persistent -and- linux allows for
// only a few per computer

static void remove_all_semaphores_of_my_uid(void) {
  char tempfilename[9]="YYXXXXXX";
  close(mkstemp(tempfilename));

  char cmd[256];
  snprintf(cmd, 250, "ipcs -s > %s", tempfilename);
  system(cmd);
  FILE *f=fopen(tempfilename, "r");
  char line[256];
  while (fgets(line, 200, f)) {
    //printf("%s\n", line);
    if (strlen(line)>15 && '0'<=line[11] && line[11]<='9') {
      char *helferlein;
      int semid = strtoul(&(line[11]), &helferlein, 10);
      if (helferlein==&(line[11])) {
	fprintf(stderr, "ipcs spinnt\n");
	_exit(1);
      }
      //printf("Extracted %lu\n", semid);
      if (semctl(semid, 0, IPC_RMID, NULL)) {
	fprintf(stderr, "IPC_RMID faild on %i\n", semid);
      }
    }
  }
  fclose(f);
  unlink(tempfilename);
}


static int atexit_installed=0;
static void install_garbage_collector(void) {
  if (!atexit_installed) {
    atexit_installed=1;
    if (atexit(remove_all_semaphores_of_my_uid)) {
      fprintf(stderr, "Failed to install semaphore garbage collection\n");
      exit(1);
    }
  }
}

//--- now for the real semaphore operations

semaphore sem_init(int startwert) {
  install_garbage_collector();
  semaphore s;
  if (NULL==(s=(semaphore)malloc(sizeof(struct SEMAPHORE)))) {
    printf("malloc() failed in sem_init\n");
    exit(1);
  }
  if (-1==(s->key=semget(IPC_PRIVATE, 1, IPC_CREAT|0600))) {
    printf("Problems in sem_init: semget() returned errno=%i\n", errno);
    perror("Blubb");
    exit(1);
  } /* if */
  if (-1==semctl(s->key, 0, SETVAL, startwert)) {
    printf("Problems in sem_init: semctl() returned errno=%i\n", errno);
    perror("Blubb");
    exit(1);
  } /* if */
  s->count=startwert;
  return s;
}

void sem_p(semaphore s) {
  s->count -= 1;
  struct sembuf do_p;
  do_p.sem_num=0; do_p.sem_op=-1; do_p.sem_flg=0;
  if (-1==semop(s->key, &do_p, 1)) {
    printf("Aerger mit sem_p(empty)\n");
    exit(1);
  } /* if */
}

void sem_v(semaphore s) {
  s->count += 1;
  struct sembuf do_v;
  do_v.sem_num=0; do_v.sem_op=1; do_v.sem_flg=0;
  if (-1==semop(s->key, &do_v, 1)) {
    printf("Problems in sem_v: semop() returned errno=%i\n", errno);
    perror("Blubb");
    exit(1);
  } /* if */
}

#ifdef SPAETER_MAL
int sem_t(semaphore s) {
  printf("sem_t noch nicht implementiert\n");
  exit(1);
}
#endif

int sem_count(semaphore s) {
  return s->count;
}
