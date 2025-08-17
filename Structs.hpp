#ifndef STRUCTS
#define STRUCTS

#include <vector>
using namespace std;

/* Struct referente ao contexto */
struct Context{
  int counter; /* Contador total de elementos do contexto */
  int counter_ro; /* Contador do ro */
  
  Context(){
    counter = 0;
    counter_ro = 0;
  }
};

struct TNode{
  vector<bool>alphabet;
  double probValue;   
  bool isroot;
  char symbol;
  int counter;
  TNode* vine;
  TNode* rigthPointer;
  TNode* downPointer;
  TNode* lastCreate; /* Ultimo no criado do pai */
  Context* context;
  
  TNode() {
        symbol = 0;
        counter = 0;
        isroot = false;
        probValue = 0.0;
        vine = nullptr;
        rigthPointer = nullptr;
        downPointer = nullptr;
        context = nullptr;
        lastCreate = nullptr;
    }
};

#endif 
