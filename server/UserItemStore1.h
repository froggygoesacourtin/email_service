#ifndef USERITEMSTORE_H
#define USERITEMSTORE_H

using namespace std;

#include <unordered_map>
#include <unordered_set>
#include "ElementStore.h"

class UserItemStore {
 public:

 ElementStore _userStore;
 ElementStore _itemStore;

 unordered_map<int, unordered_set<int>* > *_userItemDict;
 unordered_map<int, unordered_set<int>* > *_itemUserDict;

 UserItemStore();
 ~UserItemStore();

 void add(unordered_map<int, unordered_set<int>* > *m, int key, int val);
 void remove(unordered_map<int, unordered_set<int>* > *m, int key, int val);
 bool get(unordered_map<int, unordered_set<int>* > *m, int key, int val);
 void clear(unordered_map<int, unordered_set<int>* > *m);
 void print(unordered_map<int, unordered_set<int>* > *m);
 unordered_set<int>* get(unordered_map<int, unordered_set<int>* > *m, int key);
 unordered_set<int>* first(unordered_map<int, unordered_set<int>* > *m, int *key);

 void add(int user, int item);
 void remove(int user, int item);

 void clear() {
  clear(_userItemDict);
  clear(_itemUserDict);
 }

 unordered_set<int>* item_get(int user);
 unordered_set<int>* user_get(int item);

 int user_size() {
  return _userItemDict->size();
 }

 int item_size() {
  return _itemUserDict->size();
 }

 bool isSimilar(unordered_set<int> *s1, unordered_set<int> *s2);
 unordered_map<int, unordered_set<int> * >* createItemClusters();
};

#endif
