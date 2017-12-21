#ifndef USERITEMSTORE_H
#define USERITEMSTORE_H

using namespace std;

#include <unordered_map>
#include "ElementStore.h"

class UserItemStore {
 public:

 static const int Alpha = .25;

 ElementStore _userStore;
 ElementStore _itemStore;

 unordered_map<int, unordered_map<int, int>* > *_userItemCountMap;
 unordered_map<int, unordered_map<int, int>* > *_itemUserCountMap;

 unordered_map<int, unordered_map<int, float>* > *_userItemDistMap;
 unordered_map<int, unordered_map<int, float>* > *_itemUserDistMap;

 unordered_map<int, unordered_map<int, float>* > *_itemClusterMap;

 UserItemStore();
 ~UserItemStore();

 void add(unordered_map<int, unordered_map<int, int>* > *m, int key, int val);
 void remove(unordered_map<int, unordered_map<int, int>* > *m, int key, int val);
 int get(unordered_map<int, unordered_map<int, int>* > *m, int key, int val);
 void clear(unordered_map<int, unordered_map<int, int>* > *m);
 void print(unordered_map<int, unordered_map<int, int>* > *m);
 void print(unordered_map<int, unordered_map<int, float>* > *m);
 unordered_map<int, int>* get(unordered_map<int, unordered_map<int, int>* > *m, int key);
 unordered_map<int, int>* first(unordered_map<int, unordered_map<int, int>* > *m, int *key);

 void add(int user, int item);
 void remove(int user, int item);

 void clear() {
  clear(_userItemCountMap);
  clear(_itemUserCountMap);
 }

 unordered_map<int, int>* item_get(int user);
 unordered_map<int, int>* user_get(int item);

 int user_size() {
  return _userItemCountMap->size();
 }

 int item_size() {
  return _itemUserCountMap->size();
 }

 void createDist();

 void addWeight(unordered_map<int, unordered_map<int, float>* > *m, int item1, int item2, float weight);
 void createItemClusters();

 int getUserRecs(int user, int recs[], int recLen);
};

#endif
