#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "mab.h"

int main() {
 mab m(60, .4, .98);
 int list[1];
 int i;

 srand(42);

 for (i = 0; i < 60; i++) {
  m.setReward(i, 1);
 }

 for (i = 0; i < 2000; i++) {
  m.select(list, 1);
  printf("%d %d\n", i, list[0]);

  if (list[0] == 14) {
   m.update(list, 1, 0);
  }
  else {
   m.update(list, 1);
  }
 }

 printf("final: %d %d\n", i, list[0]);

 return 0;
}
