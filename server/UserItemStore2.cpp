#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unordered_map>
#include <list>
#include "UserItemStore.h"

using namespace std;

UserItemStore::UserItemStore() {
 _userItemCountMap = new unordered_map<int, unordered_map<int, int> * >();
 _itemUserCountMap = new unordered_map<int, unordered_map<int, int> * >();
}

UserItemStore::~UserItemStore() {
 clear(_userItemCountMap);
 clear(_itemUserCountMap);

 delete _userItemCountMap;
 delete _itemUserCountMap;
}

void UserItemStore::add(unordered_map<int, unordered_map<int, int>* > *m, int key, int val) {
 unordered_map<int, unordered_map<int, int> * >::iterator itmap;
 unordered_map<int, int> *itemMap;

 itmap = m->find(key);
 if (itmap != m->end()) {
  itemMap = itmap->second;
  if (itemMap->find(val) == itemMap->end()) {
   (*itemMap)[val] = 1;
  }
  else {
   (*itemMap)[val]++;
  }
 }
 else {
  itemMap = new unordered_map<int, int>();
  (*itemMap)[val] = 1;
  (*m)[key] = itemMap;
 }
}

void UserItemStore::remove(unordered_map<int, unordered_map<int, int>* > *m, int key, int val) {
 unordered_map<int, unordered_map<int, int> * >::iterator itmap1;
 unordered_map<int, int>::iterator itmap2;
 unordered_map<int, int> *itemMap;

 itmap1 = m->find(key);
 if (itmap1 != m->end()) {
  itemMap = itmap1->second;
  itmap2 = itemMap->find(val);
  if (itmap2 != itemMap->end()) {
   if ((*itemMap)[val] > 1) {
    (*itemMap)[val]--;
   }
   else {
    itemMap->erase(itmap2);

    if (itemMap->empty()) {
     delete itemMap;
     m->erase(itmap1);
    }
   }
  }
 }
}

int UserItemStore::get(unordered_map<int, unordered_map<int, int>* > *m, int key, int val) {
 unordered_map<int, unordered_map<int, int> * >::iterator itmap1;
 unordered_map<int, int>::iterator itmap2;
 unordered_map<int, int> *itemMap;

 itmap1 = m->find(key);
 if (itmap1 != m->end()) {
  itemMap = itmap1->second;
  itmap2 = itemMap->find(val);
  if (itmap2 != itemMap->end()) {
   return itmap2->second;
  }
 }

 return 0;
}

void UserItemStore::clear(unordered_map<int, unordered_map<int, int>* > *m) {
 unordered_map<int, unordered_map<int, int> * >::iterator itmap;

 for (itmap = m->begin(); itmap != m->end(); ++itmap) {
  delete itmap->second;
 }

 m->clear();
}

void UserItemStore::print(unordered_map<int, unordered_map<int, int>* > *m) {
 unordered_map<int, unordered_map<int, int> * >::iterator itmap1;
 unordered_map<int, int>::iterator itmap2;
 unordered_map<int, int> *itemMap;

 for (itmap1 = m->begin(); itmap1 != m->end(); ++itmap1) {
  printf("key: %d\n", itmap1->first);
  itemMap = itmap1->second;
  for(itmap2 = itemMap->begin(); itmap2 != itemMap->end(); ++itmap2) {
   printf("        %d:%d\n", itmap2->first, itmap2->second);
  }
 }
}

void UserItemStore::print(unordered_map<int, unordered_map<int, float>* > *m) {
 unordered_map<int, unordered_map<int, float> * >::iterator itmap1;
 unordered_map<int, float>::iterator itmap2;
 unordered_map<int, float> *itemMap;

 for (itmap1 = m->begin(); itmap1 != m->end(); ++itmap1) {
  printf("key: %d\n", itmap1->first);
  itemMap = itmap1->second;
  for(itmap2 = itemMap->begin(); itmap2 != itemMap->end(); ++itmap2) {
   printf("        %d:%f\n", itmap2->first, itmap2->second);
  }
 }
}

unordered_map<int, int> *UserItemStore::get(unordered_map<int, unordered_map<int, int>* > *m, int key) {
 unordered_map<int, unordered_map<int, int> * >::iterator itmap;

 itmap = m->find(key);
 if (itmap != m->end()) {
  return itmap->second;
 }
 else {
  return NULL;
 }
}

unordered_map<int, int> *UserItemStore::first(unordered_map<int, unordered_map<int, int>* > *m, int *key) {
 unordered_map<int, unordered_map<int, int> * >::iterator itmap;

 itmap = m->begin();
 if (itmap != m->end()) {
  *key = itmap->first;
  return itmap->second;
 }
 else {
  return NULL;
 }
}

void UserItemStore::add(int user, int item) {
 add(_userItemCountMap, user, item);
 add(_itemUserCountMap, item, user);
}

void UserItemStore::remove(int user, int item) {
 remove(_userItemCountMap, user, item);
 remove(_itemUserCountMap, item, user);
}

unordered_map<int, int> *UserItemStore::user_get(int item) {
 return get(_itemUserCountMap, item);
}

unordered_map<int, int> *UserItemStore::item_get(int user) {
 return get(_userItemCountMap, user);
}

void UserItemStore::addWeight(unordered_map<int, unordered_map<int, float>* > *m, int item1, int item2, float weight) {
 unordered_map<int, unordered_map<int, float> * >::iterator itmap;
 unordered_map<int, float> *itemMap;

 itmap = m->find(item1);
 if (itmap != m->end()) {
  itemMap = itmap->second;
  (*itemMap)[item2] = weight;
 }
 else {
  itemMap = new unordered_map<int, float>();
  (*itemMap)[item2] = weight;
  (*m)[item1] = itemMap;
 }
}

void UserItemStore::createDist() {
 unordered_map<int, unordered_map<int, int> * >::iterator itmap1;
 unordered_map<int, int>::iterator itmap2;
 unordered_map<int, int> *countMap;
 unordered_map<int, float> *distMap;
 int sum;

 _userItemDistMap = new unordered_map<int, unordered_map<int, float> * >();

 for (itmap1 = _userItemCountMap->begin(); itmap1 != _userItemCountMap->end(); ++itmap1) {
  countMap = itmap1->second;

  sum = 0;
  for (itmap2 = countMap->begin(); itmap2 != countMap->end(); ++itmap2) {
   sum += itmap2->second;
  }

  distMap = new unordered_map<int, float>;
  for (itmap2 = countMap->begin(); itmap2 != countMap->end(); ++itmap2) {
   (*distMap)[itmap2->first] = float(itmap2->second) / float(sum);
  }

  (*_userItemDistMap)[itmap1->first] = distMap;
 }

 _itemUserDistMap = new unordered_map<int, unordered_map<int, float> * >();

 for (itmap1 = _itemUserCountMap->begin(); itmap1 != _itemUserCountMap->end(); ++itmap1) {
  countMap = itmap1->second;

  sum = 0;
  for (itmap2 = countMap->begin(); itmap2 != countMap->end(); ++itmap2) {
   sum += itmap2->second;
  }

  distMap = new unordered_map<int, float>;
  for (itmap2 = countMap->begin(); itmap2 != countMap->end(); ++itmap2) {
   (*distMap)[itmap2->first] = float(itmap2->second) / float(sum);
  }

  (*_itemUserDistMap)[itmap1->first] = distMap;
 }
}

void UserItemStore::createItemClusters() {
 unordered_map<int, unordered_map<int, float> * >::iterator itmap1;
 unordered_map<int, unordered_map<int, float> * >::iterator itmap2;
 unordered_map<int, float>::iterator itUserMap1;
 unordered_map<int, float>::iterator itUserMap2;
 unordered_map<int, float> *userMap1;
 unordered_map<int, float> *userMap2;
 int *items;
 int len, cnt, i, j;
 float sum1, sum2, weight;

 _itemClusterMap = new unordered_map<int, unordered_map<int, float> * >();

 i = _itemUserDistMap->size();
 items = new int[i];

 len = 0;
 for (itmap1 = _itemUserDistMap->begin(); itmap1 != _itemUserDistMap->end(); ++itmap1) {
  items[len] = itmap1->first;
  len++;
 }
  

 for (i = 0; i < len; i++) {
  for (j = i + 1; j < len; j++) {
   itmap1 = _itemUserDistMap->find(items[i]);
   userMap1 = itmap1->second;

   itmap2 = _itemUserDistMap->find(items[j]);
   userMap2 = itmap2->second;

   sum1 = sum2 = 0;
   cnt = 0;
   for (itUserMap1 = userMap1->begin(); itUserMap1 != userMap1->end(); ++itUserMap1) {
    itUserMap2 = userMap2->find(itUserMap1->first);
    if (itUserMap2 != userMap2->end()) {
     sum1 += float(itUserMap2->second) / float(itUserMap1->second);
     sum2 += float(itUserMap1->second) / float(itUserMap2->second);
     cnt++;
    }
   }

   if (cnt > 0) {
    sum1 /= cnt;
    addWeight(_itemClusterMap, items[i], items[j], sum1);
   
    sum2 /= cnt;
    addWeight(_itemClusterMap, items[j], items[i], sum2);
   }
  }
 }

 delete items;
}

int UserItemStore::getUserRecs(int user, int recs[], int recLen) {
 unordered_map<int, unordered_map<int, float> * >::iterator itmap1;
 unordered_map<int, float>::iterator itmap2;
 unordered_map<int, float> *itemMap;
 cnt = 0;

 itmap1 = _userItemMap->find(user);
 if (itmap1 != _userItemMap->end()) {
  itemMap = itmap1.second;

  for (itmap2 = itemMap->begin(); itmap2 != itemMap->end() && cnt < recLen; ++itmap2) {
   if (itmap2.second > Alpha) {
    recs[cnt++] = itmap2.first;
   }
  }

  if (cnt < recLen) {
  }
 }
}
