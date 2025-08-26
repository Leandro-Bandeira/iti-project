#ifndef STRUCTS
#define STRUCTS

#include <vector>
#include <cstdint>
#include <algorithm>

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
  unsigned char symbol;
  int counter;
  TNode* vine;
  TNode* rigthPointer;
  TNode* downPointer;
  TNode* lastCreate; /* Ultimo no criado do pai */
  Context* context;
  int heigth;
    
  TNode() {
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


struct ArithmeticEncoder {
    double low = 0;
    double high = 1.0;
 

    void encode(TNode* cNode, unsigned char byteRead, bool isRoot, vector<bool>* alphabet, double current_prob_ro){
      vector<unsigned char> alphabetCod;

      if(isRoot){
        for (int i = 0; i < (int)alphabet->size(); i++) {
            if (alphabet->at(i)) {
                // faz o cast do índice i para caractere
                alphabetCod.push_back((unsigned char)i);
            }
        }

        // ordenar os símbolos em ordem crescente
        sort(alphabetCod.begin(), alphabetCod.end());


        double range = high - low;

        double clow = low;
        double chigh = 0.0;

        // Vamos percorrer o alfabeto e sair fatiando o high e o low ate encontrar o valor
        for(unsigned char b: alphabetCod){
          chigh = clow + (range * cNode->probValue);
          /* Se encontramos o valor*/
          if(b == byteRead){
            low = clow;
            high = chigh;
            cout << "probabilidade: " << cNode->probValue << endl;
            std::cout << "Low = " << low << ", High = " << high << std::endl;
            break;
          }
          clow = chigh;
          std::cout << "byte: " << b << "cLow = " << clow << ", chigh = " << chigh << std::endl;
        }
      }else{
        
        TNode* node = cNode->downPointer;
        while(node){
            alphabetCod.push_back(node->symbol); 
            node = node->rigthPointer;
        }
        sort(alphabetCod.begin(), alphabetCod.end());
        double range = high - low;

        double clow = low;
        double chigh = 0.0;

        for(unsigned char b: alphabetCod){
          node = cNode->downPointer;
          
          while(node){
            if(node->symbol == b) break; 
            node = node->rigthPointer;
          }
          
          chigh = clow + (range * node->counter / node->context->counter);
          /* Se encontramos o valor*/
          if(b == byteRead){
            low = clow;
            high = chigh;
            cout << "probabilidade: " << cNode->probValue << endl;
            std::cout << "Low = " << low << ", High = " << high << std::endl;
            break;
          }
          clow = chigh;
          std::cout << "byte: " << b << "cLow = " << clow << ", chigh = " << chigh << std::endl;
        }

        /* Se estamos aqui, devemos codificar o ro*/
        if(current_prob_ro != 0.0){
          chigh = clow + range * current_prob_ro;
          low = clow;
          high = chigh;
          cout << "probabilidade ro : " << current_prob_ro << endl;
            std::cout << "Low = " << low << ", High = " << high << std::endl;
        }
      }
      
      
    
      
    }
      
      
};
    


#endif 
