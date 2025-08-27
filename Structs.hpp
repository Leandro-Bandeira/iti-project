#ifndef STRUCTS
#define STRUCTS

#include <vector>
#include <string>

using namespace std;

/* Struct referente ao contexto */
struct Context{
  int counter; /* Contador total de elementos do contexto */
  int counter_ro; /* Contador do ro */
  
  Context(){       //"construtor"
    counter = 0;
    counter_ro = 0;
  }
};

struct TNode{
  vector<bool>alphabet;
  double probValue;   
  bool isroot;
  unsigned char symbol;
  int counter;
  TNode* vine;
  TNode* rigthPointer;
  TNode* downPointer;
  TNode* lastCreate; /* Ultimo no criado do pai */
  Context* context;
  int heigth;
    
  TNode() {           //"construtor"
        symbol = 0;
        counter = 0;
        isroot = false;
        probValue = 0.0;
        vine = nullptr;
        rigthPointer = nullptr;
        downPointer = nullptr; /* Primeiro filho */
        context = nullptr;
        lastCreate = nullptr;
        heigth = -1;
    }
};

#endif 
