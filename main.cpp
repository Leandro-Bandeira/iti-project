#include <iostream>
#include <vector>
#include <string>
#include <numeric>

#include "Structs.hpp"

using namespace std;

int normalizeAscii(char c){
    return c - 'a';
    
}
char getCaracter(int normalizedValue){
    return 'a' + normalizedValue;
}

int main(){
    TNode root;
    root.isroot = true;
    
    string msg = "abracadabra";
    
    /* Inicia todo o alfabeto como false */
    vector <bool> alphabet(26,false);
    
    /* Conseguimos definir quais letras aparecem no nosso alfabeto */
    for(char c : msg){
        int idx = normalizeAscii(c);
        alphabet[idx] = true;
    }
    /* Calcula a quantia simbolos que apareceram */
    double diffElements = accumulate(alphabet.begin(), alphabet.end(), 0);

    /* Com isso definimos a nossa probabilidade */ 
    root.alphabet = alphabet;
    root.probValue = 1 / diffElements;
    root.counter = diffElements;
    cout << "Primeiro valor de equiprobabilidade: " << root.probValue << "\n";

    TNode* base = &root;
    
    for(char c: msg){
        double currentProb = 0;
        cout << "Lendo: " << c << "\n";
        /* Verificamos se base esta apontado para o root */
        if(base->isroot){
            int idx = normalizeAscii(c);
            /* Verificamos se o alfabeto esta na raiz, caso sim
               Vamos codificar de acordo com equiprobabilidade e remover o alfabeto
            */
            if(base->alphabet[idx]){
                base->alphabet[c] = false;
                currentProb = base->probValue;
                cout << "O caracter (" << c << ") foi codificado com probabilidade: " << currentProb << "\n";
                /* Precisamos atualizar a probabilidade do root */
                base->counter--;
                base->probValue =  1 / base->counter;
            }
            
        }     
    }
    return 0;
}
