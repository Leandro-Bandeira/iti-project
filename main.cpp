#include <iostream>
#include <vector>
#include <string>
#include <numeric>

#include "Structs.hpp"

using namespace std;

int main(){
    TNode root;
    root.isroot = true;
    
    string msg = "abracadabra";
    
    /* Inicia todo o alfabeto como false */
    vector <bool> alphabet(256,false);
    
    /* Conseguimos definir quais letras aparecem no nosso alfabeto */
    for(unsigned char c : msg){
        int idx = static_cast<int>(c);
        alphabet[idx] = true;
    }
    /* Calcula a quantia simbolos que apareceram */
    root.counter = accumulate(alphabet.begin(), alphabet.end(), 0);

    /* Com isso definimos a nossa probabilidade */ 
    root.alphabet = alphabet;
    root.probValue = 1.0/ root.counter;
    cout << "Primeiro valor de equiprobabilidade: " << root.probValue << "\n"; 

    TNode* base = &root;
    int count = 0;
    for(unsigned char c: msg){
        double currentProb = 0.0;
        cout << "Lendo: " << c << "\n";
        TNode* father = base;
        while(father != nullptr){     
            /* Verificamos se base esta apontado para o root */
            if(father->isroot){
                int idx = static_cast<int>(c);
                /* Verificamos se o alfabeto esta na raiz, caso sim
                   Vamos codificar de acordo com equiprobabilidade e remover o alfabeto
                */
                if(father->alphabet[idx]){
                    father->alphabet[c] = false;
                    currentProb = father->probValue;
                    cout << "O caracter (" << c << ") foi codificado com probabilidade: " << currentProb << "\n";
                    /* Precisamos atualizar a probabilidade do root */
                    father->counter--;
                    father->probValue =  1.0 / father->counter;
                }
                /* Criando filho da raiz */
                TNode* child = new TNode();
                child->vine = &root;
                child->symbol = c;
                child->counter += 1;
                if (count == 0) base = child; /* Atualiza o base */
                count += 1;
                break;
            
            }else{          
                char currentSymbol = father->symbol;
                if (currentSymbol == c){
                    father->counter += 1;
                    break;
                }

                /* Verifica se possui filhos */
                if(!father->downPointer){
                    cout << "Criou o filho: " << c << "\n";
                    TNode* child = new TNode();
                    child->symbol = c;
                    child->vine = father;
                    child->counter = 1;
                    base = child;
                    father->downPointer = child;
                    
                }
                father = father->vine;
            }
        
        }     
    }
    return 0;
}
