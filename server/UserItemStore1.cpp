#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include "UserItemStore1h"

#define Alpha .25
#define MinCount 20

using namespace std;

UserItemStore::UserItemStore() {
 _userItemDict = new unordered_map<int, unordered_set<int> * >();
 _itemUserDict = new unordered_map<int, unordered_set<int> * >();
}

UserItemStore::~UserItemStore() {
 clear(_userItemDict);
 clear(_itemUserDict);

 delete _userItemDict;
 delete _itemUserDict;
}

void UserItemStore::add(unordered_map<int, unordered_set<int>* > *m, int key, int val) {
 unordered_map<int, unordered_set<int> * >::iterator itmap;
 unordered_set<int> *itemSet;

 itmap = m->find(key);
 if (itmap != m->end()) {
  itmap->second->insert(val);
 }
 else {
  itemSet = new unordered_set<int>();
  itemSet->insert(val);
  (*m)[key] = itemSet;
 }
}

void UserItemStore::remove(unordered_map<int, unordered_set<int>* > *m, int key, int val) {
 unordered_map<int, unordered_set<int> * >::iterator itmap;
 unordered_set<int>::iterator itset;
 unordered_set<int> *itemSet;

 itmap = m->find(key);
 if (itmap != m->end()) {
  itemSet = itmap->second;
  itset = itemSet->find(val);
  if (itset != itemSet->end()) {
   itemSet->erase(itset);

   if (itemSet->empty()) {
    delete itemSet;
    m->erase(itmap);
   }
  }
 }
}

bool UserItemStore::get(unordered_map<int, unordered_set<int>* > *m, int key, int val) {
 unordered_map<int, unordered_set<int> * >::iterator itmap;
 unordered_set<int>::iterator itset;
 unordered_set<int> *itemSet;

 itmap = m->find(key);
 if (itmap != m->end()) {
  itemSet = itmap->second;
  itset = itemSet->find(val);
  if (itset != itemSet->end()) {
   return true;
  }
 }

 return false;
}

void UserItemStore::clear(unordered_map<int, unordered_set<int>* > *m) {
 unordered_map<int, unordered_set<int> * >::iterator itmap;

 for (itmap = m->begin(); itmap != m->end(); ++itmap) {
  delete itmap->second;
 }

 m->clear();
}

void UserItemStore::print(unordered_map<int, unordered_set<int>* > *m) {
 unordered_map<int, unordered_set<int> * >::iterator itmap;
 unordered_set<int>::iterator itset;
 unordered_set<int> *itemSet;

 for (itmap = m->begin(); itmap != m->end(); ++itmap) {
  printf("key: %d\n", itmap->first);
  itemSet = itmap->second;
  for(itset = itemSet->begin(); itset != itemSet->end(); ++itset) {
   printf("        %d\n", *itset);
  }
 }
}

unordered_set<int> *UserItemStore::get(unordered_map<int, unordered_set<int>* > *m, int key) {
 unordered_map<int, unordered_set<int> * >::iterator itmap;

 itmap = m->find(key);
 if (itmap != m->end()) {
  return itmap->second;
 }
 else {
  return NULL;
 }
}

unordered_set<int> *UserItemStore::first(unordered_map<int, unordered_set<int>* > *m, int *key) {
 unordered_map<int, unordered_set<int> * >::iterator itmap;

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
 add(_userItemDict, user, item);
 add(_itemUserDict, item, user);
}

void UserItemStore::remove(int user, int item) {
 remove(_userItemDict, user, item);
 remove(_itemUserDict, item, user);
}

unordered_set<int> *UserItemStore::user_get(int item) {
 return get(_itemUserDict, item);
}

unordered_set<int> *UserItemStore::item_get(int user) {
 return get(_userItemDict, user);
}

bool UserItemStore::isSimilar(unordered_set<int> *s1, unordered_set<int> *s2) {
 unordered_set<int>::iterator probe_it;
 int cnt1, cnt2, min_cnt, intersection_cnt;

 cnt1 = s1->size();
 cnt2 = s2->size();

 min_cnt = (cnt1 < cnt2) ? cnt1 : cnt2;
 if (min_cnt >= MinCount) {
  intersection_cnt = 0;
  for (unordered_set<int>::iterator it = s1->begin(); it != s1->end(); ++it) {
   probe_it = s2->find(*it);
   if (probe_it != s2->end()) {
    intersection_cnt++;
   }
  }

  if (float(intersection_cnt) / float(min_cnt) >= Alpha) {
   return true;
  }
 }

 return false;
}

unordered_map<int, unordered_set<int> * > *UserItemStore::createItemClusters() {
 unordered_map<int, unordered_set<int> * >::iterator itmap1;
 unordered_map<int, unordered_set<int> * >::iterator itmap2;
 unordered_set<int>::iterator itset;
 unordered_set<int>::iterator itClusterSet;
 int *items;
 int *cluster;
 unordered_map<int, unordered_set<int> * > *clusterDict = new unordered_map<int, unordered_set<int> * >();
 unordered_map<int, unordered_set<int> * > *pairDict = new unordered_map<int, unordered_set<int> * >();
 unordered_set<int> *clusterSet;
 unordered_set<int> *pairSet;
 int clusterIndex, item, len, i, b, j, k, n;
 bool flag;

 len = _itemUserDict->size();

 items = new int[len];
 cluster = new int[len];

 i = 0;
 for (itmap1 = _itemUserDict->begin(); itmap1 != _itemUserDict->end(); ++itmap1) {
  items[i++] = itmap1->first;
 }
  
 for (i = 0; i < len; i++) {
  itmap1 = _itemUserDict->find(items[i]);

  for (j = i + 1; j < len; j++) {
   itmap2 = _itemUserDict->find(items[j]);

   if (isSimilar(itmap1->second, itmap2->second)) {
    add(pairDict, items[i], items[j]);
    add(pairDict, items[j], items[i]);
   }
  }
 }

 clusterIndex = 0;
 while (pairDict->size() > 0) {
  pairSet = first(pairDict, &cluster[0]);

  clusterSet = new unordered_set<int>();
  clusterSet->insert(cluster[0]);

  i = j = 0;
  flag = true;

  b = -1;
  while (j <= i && b <= i && pairDict->size() > 0) {
   b = i;

   if (flag) {
    flag = false;
   }
   else {
    pairSet = get(pairDict, cluster[j]);
   }

   if (pairSet != NULL) {
    for (itset = pairSet->begin(); itset != pairSet->end(); ++itset) {
     item = *itset;

     itClusterSet = clusterSet->find(item);
     if (itClusterSet == clusterSet->end()) {
      for (k = 0; k <= i; k++) {
       if (!get(pairDict, cluster[k], item)) {
        break;
       }
      }

      if (k == i + 1) {
       cluster[++i] = item;
       clusterSet->insert(item);
      }
     }
    }

    // Remove the pairs that were added to the cluster
    for (k = b + 1; k <= i; k++) {
     for (n = 0; n <= i; n++) {
      if (k != n) {
       remove(pairDict, cluster[k], cluster[n]);
       remove(pairDict, cluster[n], cluster[k]);
      }
     }
    }
   }

   j++;
  }

  (*clusterDict)[clusterIndex++] = clusterSet;
 }

 delete cluster;
 delete items;

 clear(pairDict);
 delete pairDict;

 return clusterDict;
}
