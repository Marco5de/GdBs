// see workers.c for description
extern int readers;
extern int writers;

void test_setup(void);
void test_end(void);

void reader(long my_id);
void writer(long my_id);
