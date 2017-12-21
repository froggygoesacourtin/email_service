#ifndef ELEMENTSTORE_H
#define ELEMENTSTORE_H

using namespace std;

#include <unordered_map>

class ElementStore {
 public:

 unordered_map<string, int> _extIdDict;
 unordered_map<int, string> _idDict;
 int _count = 0;

 int addElement(string extId) {
  unordered_map<string, int>::iterator itExtIdDict;
  int id;

  itExtIdDict = _extIdDict.find(extId);
  if (itExtIdDict == _extIdDict.end()) {
   _extIdDict[extId] = _count;
   _idDict[_count] = extId;

   id = _count;
   _count++;
  }
  else{
   id = itExtIdDict->second;
  }

  return id;
 }

 void removeElement(string extId) {
  unordered_map<string, int>::iterator itExtIdDict;
  unordered_map<int, string>::iterator itIdDict;
  int id;

  itExtIdDict = _extIdDict.find(extId);
  if (itExtIdDict != _extIdDict.end()) {
   id = itExtIdDict->second;

   _extIdDict.erase(itExtIdDict);

   itIdDict = _idDict.find(id);
   if (itIdDict != _idDict.end()) {
    _idDict.erase(itIdDict);
   }
  }
 }

 int getId(string extId) {
  unordered_map<string, int>::iterator itExtIdDict;
  int id;

  itExtIdDict = _extIdDict.find(extId);
  if (itExtIdDict != _extIdDict.end()) {
   id = itExtIdDict->second;
  }
  else {
   id = addElement(extId);
  }

  return id;
 }

 string getExtId(int id) {
  return _idDict[id];
 }
};

#endif
