#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <map>
#include <list>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "UserItemStore.h"

#define Alpha .25
#define Beta 1.0
#define PopularCount 100

using namespace std;

UserItemStore::UserItemStore() {
 _userItemCountMap = new map<int, map<int, int> * >();
 _itemUserCountMap = new map<int, map<int, int> * >();
 _userMap = new ElementStore();
 _itemMap = new ElementStore();
}

UserItemStore::~UserItemStore() {
 clear(_userItemCountMap);
 clear(_itemUserCountMap);

 delete _userItemCountMap;
 delete _itemUserCountMap;
}

// Add item to store
void UserItemStore::add(map<int, map<int, int>* > *m, int key, int val) {
 map<int, map<int, int> * >::iterator itmap;
 map<int, int> *itemMap;

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
  itemMap = new map<int, int>();
  (*itemMap)[val] = 1;
  (*m)[key] = itemMap;
 }
}

// Remove item from store
void UserItemStore::remove(map<int, map<int, int>* > *m, int key, int val) {
 map<int, map<int, int> * >::iterator itmap1;
 map<int, int>::iterator itmap2;
 map<int, int> *itemMap;

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

// Get item from store
int UserItemStore::get(map<int, map<int, int>* > *m, int key, int val) {
 map<int, map<int, int> * >::iterator itmap1;
 map<int, int>::iterator itmap2;
 map<int, int> *itemMap;

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

// Reset store
void UserItemStore::clear(map<int, map<int, int>* > *m) {
 map<int, map<int, int> * >::iterator itmap;

 for (itmap = m->begin(); itmap != m->end(); ++itmap) {
  delete itmap->second;
 }

 m->clear();
}

void UserItemStore::print(map<int, map<int, int>* > *m) {
 map<int, map<int, int> * >::iterator itmap1;
 map<int, int>::iterator itmap2;
 map<int, int> *itemMap;

 for (itmap1 = m->begin(); itmap1 != m->end(); ++itmap1) {
  printf("key: %d\n", itmap1->first);
  itemMap = itmap1->second;
  for(itmap2 = itemMap->begin(); itmap2 != itemMap->end(); ++itmap2) {
   printf("        %d:%d\n", itmap2->first, itmap2->second);
  }
 }
}

void UserItemStore::print(map<int, map<int, float>* > *m) {
 map<int, map<int, float> * >::iterator itmap1;
 map<int, float>::iterator itmap2;
 map<int, float> *itemMap;

 for (itmap1 = m->begin(); itmap1 != m->end(); ++itmap1) {
  printf("key: %d\n", itmap1->first);
  itemMap = itmap1->second;
  for(itmap2 = itemMap->begin(); itmap2 != itemMap->end(); ++itmap2) {
   printf("        %d:%f\n", itmap2->first, itmap2->second);
  }
 }
}

map<int, int> *UserItemStore::get(map<int, map<int, int>* > *m, int key) {
 map<int, map<int, int> * >::iterator itmap;

 itmap = m->find(key);
 if (itmap != m->end()) {
  return itmap->second;
 }
 else {
  return NULL;
 }
}

map<int, int> *UserItemStore::first(map<int, map<int, int>* > *m, int *key) {
 map<int, map<int, int> * >::iterator itmap;

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

map<int, int> *UserItemStore::user_get(int item) {
 return get(_itemUserCountMap, item);
}

map<int, int> *UserItemStore::item_get(int user) {
 return get(_userItemCountMap, user);
}

void UserItemStore::addWeight(map<int, map<int, float>* > *m, int item1, int item2, float weight) {
 map<int, map<int, float> * >::iterator itmap;
 map<int, float> *itemMap;

 itmap = m->find(item1);
 if (itmap != m->end()) {
  itemMap = itmap->second;
  (*itemMap)[item2] = weight;
 }
 else {
  itemMap = new map<int, float>();
  (*itemMap)[item2] = weight;
  (*m)[item1] = itemMap;
 }
}

void UserItemStore::createDist() {
 map<int, map<int, int> * >::iterator itmap1;
 map<int, int>::iterator itmap2;
 map<int, int> *countMap;
 map<int, float> *distMap;
 int sum;

 _userItemDistMap = new map<int, map<int, float> * >();

 for (itmap1 = _userItemCountMap->begin(); itmap1 != _userItemCountMap->end(); ++itmap1) {
  countMap = itmap1->second;

  sum = 0;
  for (itmap2 = countMap->begin(); itmap2 != countMap->end(); ++itmap2) {
   sum += itmap2->second;
  }

  distMap = new map<int, float>;
  for (itmap2 = countMap->begin(); itmap2 != countMap->end(); ++itmap2) {
   (*distMap)[itmap2->first] = float(itmap2->second) / float(sum);
  }

  (*_userItemDistMap)[itmap1->first] = distMap;
 }

 _itemUserDistMap = new map<int, map<int, float> * >();

 for (itmap1 = _itemUserCountMap->begin(); itmap1 != _itemUserCountMap->end(); ++itmap1) {
  countMap = itmap1->second;

  sum = 0;
  for (itmap2 = countMap->begin(); itmap2 != countMap->end(); ++itmap2) {
   sum += itmap2->second;
  }

  distMap = new map<int, float>;
  for (itmap2 = countMap->begin(); itmap2 != countMap->end(); ++itmap2) {
   (*distMap)[itmap2->first] = float(itmap2->second) / float(sum);
  }

  (*_itemUserDistMap)[itmap1->first] = distMap;
 }
}

void UserItemStore::createItemClusters() {
 map<int, map<int, float> * >::iterator itmap1;
 map<int, map<int, float> * >::iterator itmap2;
 map<int, float>::iterator itUserMap1;
 map<int, float>::iterator itUserMap2;
 map<int, float> *userMap1;
 map<int, float> *userMap2;
 int *items;
 int len, cnt, i, j;
 float sum1, sum2, weight;

 _itemClusterMap = new map<int, map<int, float> * >();

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
 map<int, map<int, float> * >::iterator itmap1;
 map<int, float>::iterator itmap2;
 map<int, float> *itemMap;
 int baseCnt, cnt, i;

 cnt = 0;
 itmap1 = _userItemDistMap->find(user);
 if (itmap1 != _userItemDistMap->end()) {
  itemMap = itmap1->second;

  for (itmap2 = itemMap->begin(); itmap2 != itemMap->end() && cnt < recLen; ++itmap2) {
   if (itmap2->second >= Alpha) {
    recs[cnt++] = itmap2->first;
   }
   else {
    break;
   }
  }

  if (cnt < recLen) {
   baseCnt = cnt;
   for (i = 0; i < baseCnt && cnt < recLen; i++) {
    itmap1 = _itemClusterMap->find(recs[i]);
    if (itmap1 != _itemClusterMap->end()) {
     itemMap = itmap1->second;
     for (itmap2 = itemMap->begin(); itmap2 != itemMap->end() && cnt < recLen; ++itmap2) {
      if (itmap2->second >= Beta) {
        recs[cnt++] = itmap2->first;
      }
     }
    }
   }
  }
 }

 return cnt;
}

void UserItemStore::createModel() {
 int popularItems[PopularCount];
 int popularItemCounts[PopularCount];
 sql::mysql::MySQL_Driver *driver;
 sql::Connection *conn;
 sql::Statement stmt;
 sql::ResultSet *res;
 map<int, map<int, float> * >::iterator itmap1;
 map<int, float>::iterator itmap2;
 map<int, float> *itemMap;
 String email, itemId;
 int user, item1, item2, i;
 float weight;

 driver = sql::mysql::get_mysql_driver_instance();
 conn = driver->connect("tcp://127.0.0.1", "SmartMail", "password");
 stmt = conn->createStatement();

 res = stmt->executeQuery("select email, itemId from Events");
 while (res->next()) {
  email = res->getString("email");
  itemId = res->getString("itemId");

  user = _userMap.add(email);
  item1 = _itemMap.add(itemId);
  add(user, item1);
 }

 delete res;

 i = 0;
 res = stmt->executeQuery("select itemId, count(*) as cnt from Events group by itemId order by count(*) desc limit" + string(PopularCount));
 while (res->next()) {
  itemId = res->getString("itemId");
  cnt = res->getInt("cnt");
  popularItems[i] = itemId;
  popularItemCounts[i] = cnt;
 }

 delete res;

 createDict();

 // Write out model
 stmt->execute("delete from PopularItems");
 for (j = 0; j < i; j++) {
  stmt->execute("insert int PopularItems(itemId, cnt) values(" + _itemMap->getExtId(popularItems[j]) + "," + string(popularItemCounts[i] + ")");
 }

 stmt->execute("delete from UserItemDist");
 for (itmap1 = _userItemDistMap->begin(); itmap1 != _userItemDistMap->end(); ++itmap1) {
  user = itmap1->first;
  itemMap = itmap1->second;
  for (itmap2 = itemMap->begin(); itmap2 != itemMap->end(); ++itmap2) {
   weight = itmap2->second;
   if (weight >= Alpha) {
    item1 = itmap2->first;
    stmt->execute("insert into UserItemDist(email, itemId, weight) values(" + _userMap->getExtId(user) + "," + _itemMap->getExtId(item1) + "," + string(weight) + ")");
   }
  }
 }


 stmt->execute("delete from ItemItemCluster");
 for (itmap1 = _itemClusterMap->begin(); itmap1 != _itemClusterMap->end(); ++itmap1) {
  item1 = itmap1->first;
  itemMap = itmap1->second;
  for (itmap2 = itemMap->begin(); itmap2 != itemMap->end(); ++itmap2) {
   weight = itmap2->second;
   if (weight >= Beta) {
    item2 = itmap2->first;
    stmt->execute("insert into ItemItemCluster(itemId1, itemId2, weight) values(" + _itemMap->getExtId(item1) + "," + _itemMap->getExtId(item2) + "," + string(weight) + ")");
   }
  }
 }

 delete stmt;
 delete conn;
}
