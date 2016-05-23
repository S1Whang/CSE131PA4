/*
 * Symbol table implementation
 *
 */

#include <sstream>
#include <iostream>
#include "symtable.h"
#include <string>
Symtable::Symtable() : current(0) {
  list = vector<map<string,llvm::Value*> >();
  list.push_back(map<string,llvm::Value*>());
}
Symtable::~Symtable() {}

bool
Symtable::Lookup(string name) {
  map<string,llvm::Value*>::iterator it;
  it = list.at(current).find(name);
  if (it != list.at(current).end())
    return true;
  return false;
}
llvm::Value*
Symtable::Lookup(int i, string name) {
  map<string,llvm::Value*>::iterator it;
  it = list.at(i).find(name);
  if (it != list.at(current).end())
    return list.at(i).find(name)->second;
  return NULL;
}
void
Symtable::Insert(string name, llvm::Value* val) {
  list.at(current).insert(pair<string,llvm::Value*>(name,val));
}

void
Symtable::Push() {
  list.push_back(map<string,llvm::Value*>());
  current++;
}

void
Symtable::Pop() {
  list.pop_back();
  current--;
}
