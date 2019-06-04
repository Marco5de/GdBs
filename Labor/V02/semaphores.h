//=== Zaehler Semaphore

// sempahore sind nur ein Zeiger auf irgendwas
// (der Inhalt geht niemanden ausser semaphore.c was an)
typedef struct SEMAPHORE * semaphore;


//=== neues semaphor erzeugen mit startwert
// zu benutzen etwa wie folgt:
//   semaphore mutex=sem_init(1);
semaphore sem_init(int startwert);


//=== sem_p und _v
void sem_p(semaphore s);

void sem_v(semaphore s);



#ifdef NOCH_NICHT_IMPLEMENTIERT

// sem_p, aber nur wenn man nicht warten muss
// gibt true zurueck, wenn p geklappt hat
int sem_t(semaphore s);

#endif


// den zaehlerstand des semaphors abfragen ohne seiteneffekte
int sem_count(semaphore s);
