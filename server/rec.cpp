// Copyright Andrew Weiss 2015,2016
// All rights reserved
// iteration: 99 RMSE: 0.183857

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <map>

#define Positive .55
#define Negative .45

#define NumItems1 30000000
#define NumItems2 2000
#define NumMatches 650000000
#define NumLatentFactors 20

#define NumIterations 50
#define LearningRate .2
#define Lambda .005
#define Lambda2 .01

using namespace std;

map<int, int> gItemMap1;
map<int, int> gRevItemMap1;
int gItemId1 = 0;

map<int, int> gItemMap2;
map<int, int> gRevItemMap2;
int gItemId2 = 0;

map<int, int> gBaseMap;
map<int, int> gConservativeMap;

class def_match {
 public:

 int itemId1;
 int itemId2;
 float match;
};

def_match matches[NumMatches];

float *itemBias1 = new float [NumItems1];
float *itemBias2 = new float [NumItems2];

float *itemMean1 = new float [NumItems1];
int *itemCount1 = new int [NumItems1];

float *itemFactors1 = new float [NumItems1 * NumLatentFactors];
float *itemFactors2 = new float [NumItems2 * NumLatentFactors];

int hist[2];
float freq[2];

int matchCount = 0;
int testCount = 0;

void readDataFile() {
 FILE *fd;
 int itemId1, rawItemId1, itemId2, rawItemId2, i;
 int maxItemId1, maxItemId2;
 int match;
 double gMatch = 0;

 printf("readDataFile: start\n");

 for (i = 0; i < 2; i++) {
  hist[i] = 0;
 }

 for (itemId1 = 0; itemId1 < NumItems1; itemId1++) {
  itemMean1[itemId1] = 0.;
  itemCount1[itemId1] = 0;
 }

 matchCount = 0;
 fd = fopen("extract.dat", "r");

 while (fscanf(fd, "%d %d %d", &rawItemId1, &rawItemId2, &match) != EOF) {
  // Dense map itemIds
  if (gItemMap1.count(rawItemId1) ==  0) {
   maxItemId1 = itemId1 = gItemId1;
   gItemId1++;
   gItemMap1[rawItemId1] = itemId1;
   gRevItemMap1[itemId1] = rawItemId1;
  }
  else {
   itemId1 = gItemMap1[rawItemId1];
  }

  if (gItemMap2.count(rawItemId2) ==  0) {
   maxItemId2 = itemId2 = gItemId2;
   gItemId2++;
   gItemMap2[rawItemId2] = itemId2;
   gRevItemMap2[itemId2] = rawItemId2;
  }
  else {
   itemId2 = gItemMap2[rawItemId2];
  }

  matches[matchCount].itemId1 = itemId1;
  matches[matchCount].itemId2 = itemId2;
  matches[matchCount].match = float(match);

  if (match != 1 && match != 0) {
   printf("error: %d\n", match);
  }

  hist[match] += 1;
  gMatch += float(match);

  itemMean1[itemId1] = itemMean1[itemId1] + match;
  itemCount1[itemId1]++;

  matchCount++;
  if (int(matchCount / 10000000) * 10000000 == matchCount) {
   printf("hist: %d %d %d %d\n", matchCount, hist[0], hist[1], gItemId1);
  }
 }

 fclose(fd);

 for (itemId1 = 0; itemId1 < gItemId1; itemId1++) {
  itemMean1[itemId1] = itemMean1[itemId1] / itemCount1[itemId1];
 }

 printf("frequency distribution\n");
 for (i = 0; i < 2; i++) {
  freq[i] = float(hist[i]) / float(matchCount);
  printf("%d: %f\n", i, freq[i]);
 }

 printf("\n");

 printf("line count: %d\n", matchCount);
 printf("global match average: %f\n", float(gMatch / matchCount));
 printf("item1 count: %d item2 count: %d\n", maxItemId1 + 1, maxItemId2 + 1);
 printf("readDataFile: end\n");
}

float predict(int itemId1, int itemId2) {
 float pred, normPred;
 int j;

 pred = itemMean1[itemId1] + itemBias1[itemId1] + itemBias2[itemId2];

 for (j = 0; j < NumLatentFactors; j++) {
  pred += itemFactors1[(itemId1 * NumLatentFactors) + j] * itemFactors2[(itemId2 * NumLatentFactors) + j];
 }

 normPred = pred;
 if (normPred < 0.) {
  normPred = 0.;
 }
 else if (normPred > 1.) {
  normPred = 1.;
 }

 return normPred;
}

class SortElement {
 public:

 int rawItem;
 float score;
};

int cmpfunc (const void *a, const void *b)
{
 if (((SortElement *) a)->score < ((SortElement *) b)->score) {
  return 1;
 }
 else if (((SortElement *) a)->score > ((SortElement *) b)->score) {
  return -1;
 }
 else {
  return 0;
 }
}

class SortElement2 {
 public:

 int count;
 int itemId;
};

int cmpfunc2 (const void *a, const void *b)
{
 if (((SortElement2 *) a)->count < ((SortElement2 *) b)->count) {
  return 1;
 }
 else if (((SortElement2 *) a)->count > ((SortElement2 *) b)->count) {
  return -1;
 }
 else {
  return 0;
 }
}

int predict(int rawItemId1, SortElement *se) {
 int itemId1, itemId2, count = 0;
 float score;

 if (gItemMap1.count(rawItemId1) > 0) {
  itemId1 = gItemMap1[rawItemId1];
  count = 0;
  for (itemId2 = 0; itemId2 < NumItems2; itemId2++) {
   score = predict(itemId1, itemId2);
   if (score > 0.) {
    se[count].rawItem = gRevItemMap2[itemId2];
    se[count].score = score;
    count++;
   }
  }
 }

 qsort(se, count, sizeof(SortElement), cmpfunc);

 return count;
}

// Learn is stochastic gradient descent on matrix factorization, with average item, and user and item bias
void learn() {
 unsigned long seed = 457345325;
 float match, pred, normPred, err, normErr, RMSE, prvRMSE;
 double sumNormErr;
 float itemFactor1, itemFactor2;
 int itemId1, itemId2, weightCount, i, j, k, n;

 printf("learn: start\n");

 printf("learn: initialize data structures\n");

 // initialize the biases
 for (i = 0; i < NumItems1; i++) {
  seed = ((seed * 1103515245) + 12345) % 982451653;
  itemBias1[i] = 0.1f*float(seed)/float(982451653.f)-0.05f;
 }

 for (i = 0; i < NumItems2; i++) {
  seed = ((seed * 1103515245) + 12345) % 982451653;
  itemBias2[i] = 0.1f*float(seed)/float(982451653.f)-0.05f;
 }

 // initialize the latent factors
 for (i = 0; i < NumItems1; i++) {
  for (j = 0; j < NumLatentFactors; j++) {
   seed = ((seed * 1103515245) + 12345) % 982451653;
   itemFactors1[(i * NumLatentFactors) + j] = 0.1f*float(seed)/float(982451653.f)-0.05f;
  }
 }

 for (i = 0; i < NumItems2; i++) {
  for (j = 0; j < NumLatentFactors; j++) {
   seed = ((seed * 1103515245) + 12345) % 982451653;
   itemFactors2[(i * NumLatentFactors) + j] = 0.1f*float(seed)/float(982451653.f)-0.05f;
  }
 }

 printf("learn: start iteration\n");

 for (n = 0; n < NumIterations; n++) {
  sumNormErr = 0.;

  for (i = 0; i < matchCount; i++) {
   itemId1 = matches[i].itemId1;
   itemId2 = matches[i].itemId2;
   match = matches[i].match;

   weightCount = int(1. / freq[int(match)]);

   for (k = 0; k < weightCount; k++) {
    pred = itemMean1[itemId1] + itemBias1[itemId1] + itemBias2[itemId2];

    for (j = 0; j < NumLatentFactors; j++) {
     pred += itemFactors1[(itemId1 * NumLatentFactors) + j] * itemFactors2[(itemId2 * NumLatentFactors) + j];
    }

    normPred = pred;
    if (normPred < 0.) {
     normPred = 0.;
    }
    else if (normPred > 1.) {
     normPred = 1.;
    }

    normErr = match - normPred;
    sumNormErr += normErr * normErr;

    err = match - pred;

    itemBias1[itemId1] += LearningRate * (err - (Lambda2 * itemBias1[itemId1]));
    itemBias2[itemId2] += LearningRate * (err - (Lambda2 * itemBias2[itemId2]));

    // derive the gradients
    for (j = 0; j < NumLatentFactors; j++) {
     itemFactor1 = LearningRate * ((err * itemFactors2[(itemId2 * NumLatentFactors) + j]) - (Lambda * itemFactors1[(itemId1 * NumLatentFactors) + j]));
     itemFactor2 = LearningRate * ((err * itemFactors1[(itemId1 * NumLatentFactors) + j]) - (Lambda * itemFactors2[(itemId2 * NumLatentFactors) + j]));

     itemFactors1[(itemId1 * NumLatentFactors) + j] += itemFactor1;
     itemFactors2[(itemId2 * NumLatentFactors) + j] += itemFactor2;
    }
   }
  }

  if (n == 0) {
   RMSE = sqrt(sumNormErr / matchCount);
   printf("iteration: 0 RMSE: %f\n", RMSE);
   fflush(stdout);
  }
  else {
   prvRMSE = RMSE;
   RMSE = sqrt(sumNormErr / matchCount);
   printf("iteration: %d RMSE: %f\n", n, RMSE);
   fflush(stdout);

   if (prvRMSE < RMSE) {
    break;
   }
  }
 }

 printf("learn: end\n");
}

int main() {
 SortElement se[NumItems2];
 int itemId1, rawItemId1, itemId2, rawItemId2, match;
 int accuracyCount = 0, errCount = 0, i, j;
 int x, y, z, a, grayCount;
 float normPred, normErr, sumNormErr = 0, p, r;
 FILE *fd;

 readDataFile();
 learn();

 fd = fopen("ItemBias1.dat", "w");
 for (i = 0; i < NumItems1; i++) {
  fprintf(fd, "%f\n", itemBias1[i]);
 }

 fclose(fd);

 fd = fopen("ItemBias2.dat", "w");
 for (i = 0; i < NumItems2; i++) {
  fprintf(fd, "%f\n", itemBias2[i]);
 }

 fclose(fd);

 fd = fopen("LatentFactors1.dat", "w");
 for (i = 0; i < NumItems1; i++) {
  for (j = 0; j < NumLatentFactors; j++) {
   fprintf(fd, "%f\n", itemFactors1[(i * NumLatentFactors) + j]);
  }
 }

 fclose(fd);

 fd = fopen("LatentFactors2.dat", "w");
 for (i = 0; i < NumItems2; i++) {
  for (j = 0; j < NumLatentFactors; j++) {
   fprintf(fd, "%f\n", itemFactors2[(i * NumLatentFactors) + j]);
  }
 }

 fclose(fd);

 fd = fopen("ItemMap1.dat", "w");
 for (std::map<int, int>::iterator it = gItemMap1.begin(); it != gItemMap1.end(); it++) {
   fprintf(fd, "%d %d\n", it->first, it->second);
 }

 fclose(fd);

 fd = fopen("ItemMap2.dat", "w");
 for (std::map<int, int>::iterator it = gItemMap2.begin(); it != gItemMap2.end(); it++) {
   fprintf(fd, "%d %d\n", it->first, it->second);
 }

 fclose(fd);

 fd = fopen("extract_test.dat", "r");
 errCount = 0;

 x = 0;
 y = 0;
 z = 0;
 a = 0;
 grayCount = 0;

 while (fscanf(fd, "%d %d %d", &rawItemId1, &rawItemId2, &match) != EOF) {
  // Dense map itemIds
  if (gItemMap1.count(rawItemId1) ==  0) {
   itemId1 = -1;
  }
  else {
   itemId1 = gItemMap1[rawItemId1];
  }

  if (gItemMap2.count(rawItemId2) ==  0) {
   itemId2 = -1;
  }
  else {
   itemId2 = gItemMap2[rawItemId2];
  }

  if (match != 1 && match != 0) {
   printf("error: %d\n", match);
  }

  if (itemId1 != -1 && itemId2 != -1) {
   normPred = predict(itemId1, itemId2);

   normErr = match - normPred;
   sumNormErr += normErr * normErr;

   if (match == 1) {
    x++;
   }

   if (normPred >= Positive) {
    y++;
   }

   if (match == 1 and normPred >= Positive) {
    z++;
   }

   if ((normPred >= Positive && match == 1) || (normPred <= Negative && match == 0)) {
    a++;
   }

   if (normPred < Positive && normPred > Negative) {
    grayCount++;
   }

   if (y > 0) {
    p = float(z) / float(y);
   }
   else {
    p = 0;
   }

   if (x > 0) {
    r = float(z) / float(x);
   }
   else {
    r = 0;
   }

   testCount++;
  }
  else {
   errCount++;
  }
 }

 fclose(fd);

 printf("test count: %d gray count: %d RMSE: %f\n", testCount, grayCount, sqrt(sumNormErr / testCount));
 printf("accuracy: %f precision: %f recall: %f\n", float(a) / float(testCount), p, r);

 return 0;
}
