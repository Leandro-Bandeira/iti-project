#include <iostream>
#include <vector>
#include <string>
#include <numeric>

#include "Structs.hpp"

using namespace std;


TNode* createChild(TNode* father, unsigned char symbol){
    TNode* child = new TNode();
    if(father->isroot)child->vine = father;
    child->heigth = father->heigth + 1;    
    child->counter += 1;
    child->symbol = symbol;
    father->lastCreate = child;
    return child;
}


int main(){
    TNode root;
    root.isroot = true;
    
    string msg = "abracadabra";
    int k = 2;
    /* Inicia todo o alfabeto como false */
    vector <bool> alphabet(256,false);
    
    /* Conseguimos definir quais letras aparecem no nosso alfabeto */
    for(unsigned char c : msg){
        int idx = static_cast<int>(c);
        alphabet[idx] = true;
    }
    /* Calcula a quantia simbolos que apareceram */
    root.counter = accumulate(alphabet.begin(), alphabet.end(), 0);

    
    int diffElements = root.counter;
    /* Com isso definimos a nossa probabilidade */ 
    root.alphabet = alphabet;
    root.probValue = 1.0/ root.counter;
    cout << "Primeiro valor de equiprobabilidade: " << root.probValue << "\n"; 

    TNode* base = &root;
    TNode* lastBase = nullptr;
    int count = 0;
    int countDecodRoot = 0; // Quantia de simbolos codificados no root
    int count_msg = 0;
    for(unsigned char c: msg){
        
        double currentProb = 0.0;
        double currentProb_ro = 0.0;
        int minusCounter = 0; /* Contador responsavel pela exclusao */
        cout << "Lendo: " << c << "\n";
        TNode* cBase = base;
        
        if(lastBase){
            cout << "Alterando base para lastBase: " << lastBase->symbol << endl;
            cBase = lastBase->vine;
        }
        lastBase = nullptr;
        TNode* childCreated = nullptr; /* Ultimo vine acessado */
        TNode* foundedChild = nullptr; /* Ultimo filho encontrado*/
        bool attBase = false;
        while(cBase != nullptr){
                
            /* Verificamos se base esta apontado para o root */
            if(cBase->isroot){
                int idx = static_cast<int>(c);
                /* Verificamos se o alfabeto esta na raiz, caso sim
                   Vamos codificar de acordo com equiprobabilidade e remover o alfabeto
                */
                if(cBase->alphabet[idx]){
                    cBase->alphabet[c] = false;
                    countDecodRoot += 1;
                    currentProb = cBase->probValue;
                    cout << "O caracter (" << c << ") foi codificado com probabilidade: " << currentProb << "\n";
                    
                    /* Precisamos atualizar a probabilidade do root */
                    cBase->counter--;
                    cBase->probValue =  1.0 / cBase->counter;
                
                    /* Criando filho da raiz */
                    
                    TNode* child = createChild(&root, c);
                
                    if (count == 0){ 
                        base = child;
                        count += 1;
                        root.downPointer = child;
                        root.context = new Context();
                        root.context->counter_ro += 1;
                        root.context->counter = root.context->counter_ro + 1;
                    }
                    else{
                        cout << root.context->counter << " " << minusCounter << "\n";
                        currentProb_ro = (double)root.context->counter_ro / (root.context->counter - minusCounter);
                        cout << "Codificacao do ro: " << currentProb_ro << "\n";
                        root.context->counter_ro += 1;
                        root.context->counter += 2;
                        TNode* lastCreate = root.lastCreate;
                        lastCreate->rigthPointer = child;
                        root.lastCreate = child;
                    }
                    
                    if(countDecodRoot == diffElements){ 
                        root.context->counter -= root.context->counter_ro; // Remove a presenca do ro em k = 0, caso codifiquemos tudo
                        root.context->counter_ro = 0;
                    }
                    if(childCreated){
                        cout << childCreated->symbol << endl;
                        childCreated->vine = child;
                        cout << "oiii" << endl;
                    }
                }
                else{
                    /* Caso ele nao exista na raiz, vamos percorrer os filhos da raiz */
                    TNode* cNode = root.downPointer;

                    while(cNode){
                        if(cNode->symbol == c){
                            currentProb = (double)cNode->counter / root.context->counter;
                            cout << cNode->counter << " " << root.context->counter << endl;
                            cout << "Codificando (" << c << ") em k == 0 com probabilidade: " << currentProb << "\n";
                            cNode->counter++;
                            root.context->counter += 1;
                            if(childCreated)childCreated->vine = cNode;
                            break;
                        }
                        cNode = cNode->rigthPointer;
                    }
                    
                }
                break;
            
            }else{          
                char currentSymbol = cBase->symbol;
                cout << "Acessando node com simbolo: " << currentSymbol << "\n";
                if (currentSymbol == c){
                    cBase->counter += 1;
                    break;
                }

                /* Verifica se possui filhos, ou seja, se o contexto existe */
                TNode* child = nullptr;
                if(!cBase->downPointer){
                    cout << "Criou o filho: " << c << "\n";
                    child = createChild(cBase, c);
                    if (childCreated)childCreated->vine = child;
                    
                    childCreated = child;            
                    cBase->downPointer = child;

                    /* Adiciona o contexto criado */
                    Context* context = new Context();

                    context->counter_ro += 1;
                    context->counter = context->counter_ro + 1;
                    cBase->context = context;

                    
                }else{
                    cout << "Contexto ja existe, procurando filho: \n";
                    /* Se o meu contexto exitir, devemos percorrer
                    os elementos daquele contexto da direita para a esquerda */
                    TNode* cNode = cBase->downPointer;
                    bool found = false;
                    int totalMiss = 0;
                    while(cNode){
                        char currentSymbol = cNode->symbol;
                        totalMiss += 1;
                        if(currentSymbol == c){
                            cout << "Filho encontrado codificado com" << endl; 
                            currentProb = (double)cNode->counter / cBase->context->counter;
                            cout << "prob: " << currentProb << "\n"; 
                            cNode->counter += 1;
                            cBase->context->counter++;
                            found = true;
                            foundedChild = cNode;
                            break;
                        }
                        
                        cNode = cNode->rigthPointer;
                    }

                    /* Se ele nao foi encontrado, vamos adicionar */
                    if(!found){
                        cout << "Simbolo nunca ocorreu nesse contexto\n";
                        TNode* lastCreate = cBase->lastCreate;
                        child = createChild(cBase, c);
                        if(childCreated) childCreated->vine = child;
                        
                        childCreated = child;
                        lastCreate->rigthPointer = child;
                        
                        /* Codificando p */
                        currentProb_ro = (double)cBase->context->counter_ro  / (cBase->context->counter - minusCounter);
                        cout << "Codificando ro com probabilidade: " << currentProb_ro << endl;

                        cBase->context->counter_ro += 1;
                        cBase->context->counter += 2;
                    }
                    minusCounter = totalMiss;
                    cout << "Quantia de remocao na exclusao: " << minusCounter << endl;
                }

                /* Atualiza o base */
                
                if(foundedChild){
                    
                    
                    if(foundedChild->heigth < k){
                        cout << "atualizando base" << endl;
                        base = foundedChild;
                        lastBase = nullptr;
                    }else{
                        cout << "Nao criou novo filho, mas achou filho\n";
                        lastBase = foundedChild;
                    }
                    attBase = true;
                    break;
                }
                else{
                    /* So atualiza o base na primeira adicao */
                    if(!attBase){   
                        if(child->heigth < k){
                            base = child;
                            lastBase = nullptr;
                        }else{
                            lastBase = child; /* Salva para adicionar o base como vine no futuro*/
                            
                        }
                        attBase = true;
                    }
                    cBase = cBase->vine;
                }
                
                
            }
            
        
        }     
    }
    return 0;
}
