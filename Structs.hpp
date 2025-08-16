#ifndef STRUCTS
#define STRUCTS

#include <vector>
using namespace std;

struct TNode{
  vector<bool>alphabet;
  double probValue;   
  bool isroot;
  char symbol;
  int counter;
  TNode* vine;
  TNode* rigthPointer;
  TNode* downPointer;

  TNode() {
        symbol = 0;
        counter = 0;
        isroot = false;
        probValue = 0.0;
        vine = nullptr;
        rigthPointer = nullptr;
        downPointer = nullptr;
    }
};

#endif 
