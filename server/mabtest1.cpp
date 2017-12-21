#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "mab.h"

int main() {
 mab m(10, .4, .1);
 int list[4];
 int sel1, sel2, selx, i, j;

 srand(42);

 for (i = 0; i < 10; i++) {
  m.setReward(i, 10 * i);
 }

 sel1 = 3;
 sel2 = 5;

 for (i = 0; i < 100; i++) {
  m.select(list, 4);
  printf("%d %d %d %d\n", list[0], list[1], list[2], list[3]);

  for (j = 0; j < 4; j++) {
   if (list[j] == sel1) {
    printf("update1: %d\n", sel1);
    m.update(list, 4, j);

    selx = sel1;
    sel1 = sel2;
    sel2 = selx;

    break;
   }
  }

  if (j == 4) {
   for (j = 0; j < 4; j++) {
    if (list[j] == sel2) {
     printf("update2: %d\n", sel2);
     m.update(list, 4, j);

     selx = sel1;
     sel1 = sel2;
     sel2 = selx;

     break;
    }
   }
  }

  if (j == 4) {
   m.update(list, 4);
  }
 }

 printf("final %d %d %d %d\n", list[0], list[1], list[2], list[3]);

 return 0;
}
