/**
 * File: symtable.h
 * ----------- 
 *  Header file for Symbol table implementation.
 */
#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <list>
#include "irgen.h"
#include "ast_type.h"
#include "ast_decl.h"
using namespace std;
class Symtable {
public:
  Symtable();
  ~Symtable();
  bool Lookup(string);
  void Insert(string,llvm::Value*);
  void Push();
  void Pop();
  int GetCurrentIndex() {return current;}
  vector<map<string,llvm::Value*> >& GetVector() {return list;}
  map<string,llvm::Value*>& GetMap() {return list.at(current);}
  map<string,llvm::Value*>& GetGlobalMap() {return list.at(0);}
  map<string,llvm::Value*>& GetIndexMap(int i) {return list.at(i);}
  map<string,llvm::Value*>& GetPrevMap() {return list.at(current-1);}
protected:
  vector<map<string,llvm::Value*> >list;
  int current;
};
#endif // _SYMTABLE_H_
