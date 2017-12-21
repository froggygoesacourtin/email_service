#ifndef MAB_H
#define MAB_H

class mab {
 private:

 float _minPi;
 float _maxPi;

 float _alpha;
 float _beta;
 int _itemCount;
 int _curReward;
 float *_pi;
 float *_reward;


 public:

 mab(int itemCount, float alpha, float beta);

 void setReward(int item, float reward);

 void select(int *list, int listCount);

 void update(int *list, int listCount, int sel);

 void update(int *list, int listCount);
};

#endif
