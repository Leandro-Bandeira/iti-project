#ifndef STRUCTS
#define STRUCTS

#include <vector>
#include <string>

using namespace std;

/*=================================
Estrutura de dados
===================================

o nivel que estamos na arvore diz respeito ao contexto (ou seja a arvore terá no máximo N+1 níveis)
o nivel 0 é a raiz -> contexto 0 (ignorancia total)

=================================== */


struct Context{
  int counter;    //Contador total de elementos do contexto 
  int counter_ro; // Contador do ro 
  
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
  int counter;          //??????????????????????????
  TNode* vine;          //aponta para o contexto anterior
  TNode* rigthPointer;  //aponta para o nó à direita (mesmo contexto)
  TNode* downPointer;   //aponta para o FILHO esquerdo desse nó, próx. contexto  
  TNode* lastCreate;    /* Ultimo no criado do pai */
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
