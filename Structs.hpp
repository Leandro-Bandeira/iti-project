#ifndef STRUCTS
#define STRUCTS

#include <vector>
using namespace std;

struct TNode{
  vector<bool>alphabet;
  double probValue;   
  bool isroot = false;
  char symbol;
  double counter;
  TNode* vine = nullptr;
  TNode* rigthPointer = nullptr;
  TNode* downPointer = nullptr;
};

#endif 
