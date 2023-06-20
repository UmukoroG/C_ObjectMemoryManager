#include "ObjectManager.h"
#include <stdio.h>

#define NUM_OBJECTS 31

int main(int argc, char *argv[])
{
  char *ptr;
  int i;
  int ids[NUM_OBJECTS];
  initPool();

  int objectSizes[NUM_OBJECTS] = {100, 4000, 20200, 10, 500, 10000, 200, 100, 8000, 50, 300, 1500, 30, 700,
                                  5000, 15000, 50, 6000, 8000, 20, 7000, 12000, 30, 9000, 15000, 25, 6000,
                                  8000, 40, 7000, 12000, 15, 9000, 5500};

  for (i = 0; i < NUM_OBJECTS; i++) {
    ids[i] = insertObject(objectSizes[i]);
    if (i == 0) {
      ptr = (char*)retrieveObject(ids[i]);
      for (int j = 0; j < objectSizes[i]; j++)
        ptr[j] = (char)(j % 26 + 'A');
    }
    if (i == 5 || i == 9 || i == 13 || i == 17 || i == 21 || i == 25 || i == 29) {
      dropReference(ids[i]);
    }
  }

  ptr = (char*)retrieveObject(ids[0]);
  for (i = 0; i < objectSizes[0]; i++)
    fprintf(stderr, "%c", ptr[i]);
  fprintf(stderr, "\n");

  dumpPool();
  destroyPool();
  fprintf(stderr, "---\n");
  return 0;
}