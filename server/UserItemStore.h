#ifndef USERITEMSTORE_H
#define USERITEMSTORE_H

using namespace std;

#include <map>
#include "ElementStore.h"

class UserItemStore {
 public:

 map<int, map<int, int>* > *_userItemCountMap;
 map<int, map<int, int>* > *_itemUserCountMap;

 map<int, map<int, float>* > *_userItemDistMap;
 map<int, map<int, float>* > *_itemUserDistMap;

 map<int, map<int, float>* > *_itemClusterMap;

 ElementStore *_userMap;
 ElementStore *_itemMap;

 UserItemStore();
 ~UserItemStore();

 void add(map<int, map<int, int>* > *m, int key, int val);
 void remove(map<int, map<int, int>* > *m, int key, int val);
 int get(map<int, map<int, int>* > *m, int key, int val);
 void clear(map<int, map<int, int>* > *m);
 void print(map<int, map<int, int>* > *m);
 void print(map<int, map<int, float>* > *m);
 map<int, int>* get(map<int, map<int, int>* > *m, int key);
 map<int, int>* first(map<int, map<int, int>* > *m, int *key);

 void add(int user, int item);
 void remove(int user, int item);

 void clear() {
  clear(_userItemCountMap);
  clear(_itemUserCountMap);
 }

 map<int, int>* item_get(int user);
 map<int, int>* user_get(int item);

 int user_size() {
  return _userItemCountMap->size();
 }

 int item_size() {
  return _itemUserCountMap->size();
 }

 void createDist();

 void addWeight(map<int, map<int, float>* > *m, int item1, int item2, float weight);
 void createItemClusters();

 int getUserRecs(int user, int recs[], int recLen);

 void createModel();
};

#endif
