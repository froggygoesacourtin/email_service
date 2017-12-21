#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "mab.h"

mab::mab(int itemCount, float alpha, float beta) {
 int i;

 _minPi = 0.0001;
 _maxPi = 1.;
 _alpha = alpha;
 _beta = beta;

 _itemCount = itemCount;
 _curReward = 0.;

 _pi = new float [itemCount];
 _reward = new float [itemCount];

 for (i = 0; i < _itemCount; i++) {
  _pi[i] = 1. / float(itemCount);
  _reward[i] = 0.;
 }
}

void mab::setReward(int item, float reward) {
 _reward[item] = reward;

 if (_maxPi < _itemCount * reward) {
  _maxPi = _itemCount * reward;
 }
}

void mab::select(int *list, int listCount) {
 float sum = 0.;
 float p, r;
 int count, i, j;

 for (i = 0; i < _itemCount; i++) {
  sum += _pi[i];
 }

 count = 0;
 while (count < listCount) {
  r = float(rand()) / float(RAND_MAX);

  j =  listCount - 1;
  p = (_pi[0] / sum);

  for (i = 0; i < _itemCount - 1; i++) {
   if (r < p) {
    j = i;
    break;
   }

   p += (_pi[i + 1] / sum);
  }

  // Check that it has not been used
  for (i = 0; i < count; i++) {
   if (list[i] == j) {
    break;
   }
  }

  if (i == count) {
   list[count] = j;
   count++;
  }
 }
}

void mab::update(int *list, int listCount, int sel) {
 int i;

 _pi[list[sel]] += _beta * (_reward[list[sel]] - _curReward) / (_curReward + 1.);

 if (_pi[list[sel]] > _maxPi) {
  _pi[list[sel]] = _maxPi;
 }
 else if (_pi[list[sel]] < _minPi) {
  _pi[list[sel]] = _minPi;
 }

 _curReward = ((1. - _alpha) * _curReward) + (_alpha * _reward[list[sel]]);
}

void mab::update(int *list, int listCount) {
 int i;

 for (i = 0; i < listCount; i++) {
  _pi[list[i]] -= _beta * _curReward;

  if (_pi[list[i]] < _minPi) {
   _pi[list[i]] = _minPi;
  }
 }

 _curReward = (1. - _alpha) * _curReward;
}
