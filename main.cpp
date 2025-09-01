#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <fstream>

#include "Structs.hpp"

using namespace std;

#define model_order 2     //ordem do modelo

TNode* createChild(TNode* father, unsigned char symbol){
    TNode* child = new TNode();
    if(father->isroot)child->vine = father;
    child->heigth = father->heigth + 1;    
    child->counter += 1;
    child->symbol = symbol;
    father->lastCreate = child;
    return child;
}


/* Para cada contexto retorna sua vetor de frequencias */
vector<double>*getFrequenciesContext(TNode* father){
    vector<double>* frequencies = new vector<double>();

    TNode* currentNode = father->downPointer;
    double denominator = father->context->counter;
    double counter_ro = father->context->counter_ro;

    while(currentNode){
        double numerator = currentNode->counter;
        frequencies->push_back(numerator/denominator);
        currentNode = currentNode->rigthPointer;
    }

    frequencies->push_back(counter_ro / denominator);

    return frequencies;
}


int main(){
    
    TNode root;
    root.isroot = true;

    std::ifstream file("test.txt", std::ios::binary); 
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo!" << std::endl;
        return 1;
    }

    //le o arq byte por byte e define qual é o alfabeto existente--------------//
    std::vector<unsigned char> data;
    char c;
    while (file.get(c)) {
        data.push_back(static_cast<unsigned char>(c));
    }
    file.close();

    vector <bool> alphabet(256,false);
    for (unsigned char b : data) {      //define quais bytes aparecem no alfabeto
        int idx = static_cast<int>(b);        
        alphabet[idx] = true;
    }
    //----------------------------------------------------------------------------


    root.counter = accumulate(alphabet.begin(), alphabet.end(), 0);  //conta quantos simbolos diferentes no alfabeto existem
    int diffElements = root.counter;
    root.alphabet = alphabet;
    root.probValue = 1.0 / root.counter;
    cout << "Primeiro valor de equiprobabilidade: " << root.probValue << "\n"; 
     
    TNode* base = &root;          //base irá apontar sempre para o último nó adicionado/atualizado
    TNode* lastBase = nullptr;    
    int count = 0;
    int countDecodRoot = 0; 
    int count_msg = 0;
    for(unsigned char c: data){
        
        cout << "lendo: " << c << endl;
        double currentProb = 0.0;      //probabilidade atual
        double currentProb_ro = 0.0;   //probabilidade do rô
        int minusCounter = 0;          /* Contador responsavel pela exclusao */
        TNode* cBase = base;           //aponta para onde a base está apontando
        
        if(lastBase){          //se lastBase != 0 entao a cBase aponta pra outra coisa
            cBase = lastBase->vine;
        }
        
        lastBase = nullptr;
        TNode* childCreated = nullptr; /* Ultimo vine acessado */
        TNode* foundedChild = nullptr; /* Ultimo filho encontrado*/
        bool attBase = false;       

        while(cBase != nullptr){  
                
            if(cBase->isroot){  //cbase está apontando pra raiz
                
                int idx = static_cast<int>(c); 
            
                if(cBase->alphabet[idx]){         //se o caractere existe ainda na root 
                    cBase->alphabet[c] = false;
                    countDecodRoot += 1;
                    currentProb = cBase->probValue;
                    //=========DEVE-SE INCLUIR A CODIFICACAO AQUI =======================
                    cout << "O caracter (" << c << ") foi codificado com probabilidade: " << currentProb << "\n";
                    
                    //atualizacao dos contextos:
                    cBase->counter--;                         //nao sei oq danado eh isso
                    cBase->probValue =  1.0 / cBase->counter; //prob nova para o c0 (pq diminuiu um caractere)

                    TNode* child = createChild(&root, c);     //criando um filho que estará no contexto1 
                    
                    /* Caso seja o primeiro filho da raiz */
                    if (count == 0){                
                        base = child;                
                        count += 1;                  
                        root.downPointer = child;    
                        root.context = new Context();    
                        root.context->counter_ro += 1;    
                        root.context->counter = root.context->counter_ro + 1;  
                    }
                    else{
                        currentProb_ro = (double)root.context->counter_ro / (root.context->counter - minusCounter);
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
                    }
                
                }else{  //o caractere nao existe na raiz? 
                    
                    TNode* cNode = root.downPointer;

                    while(cNode){
                        if(cNode->symbol == c){
                            vector<double>*currentFrequencies = getFrequenciesContext(root.downPointer);
                            cout << "Frequencias em k = 0\n";
                            for(int i = 0; i < currentFrequencies->size(); i++){
                                cout << currentFrequencies->at(i) << " ";

                            }
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
            
            }else{    //cbase não está apontando pra raiz
                char currentSymbol = cBase->symbol;
        
                if (currentSymbol == c){
                    cBase->counter += 1;
                    break;
                }

                TNode* child = nullptr;
                if(!cBase->downPointer){   //se cBase não possui filhos entao
                    cout << "Nao possui filhos, criando.." << endl;
                    child = createChild(cBase, c);
                    if (childCreated)childCreated->vine = child;
                    childCreated = child;            
                    cBase->downPointer = child;

                    /* Adiciona o contexto criado */
                    Context* context = new Context();

                    context->counter_ro += 1;
                    context->counter = context->counter_ro + 1;
                    cBase->context = context;

                    
                }else{     //cBase já tem filho

                    /* Se o meu contexto exitir, devemos percorrer
                    os elementos daquele contexto da direita para a esquerda */
                    TNode* cNode = cBase->downPointer;  //cNode aponta para o contexto filho 
                    bool found = false;
                    int totalMiss = 0;

                    while(cNode){   //aqui vai pecorrer todos os nós do contexto que cnode está e tentar encontrar o simbolo c
                        char currentSymbol = cNode->symbol;
                        totalMiss += 1;
      
                        if(currentSymbol == c){                      
                            currentProb = (double)cNode->counter / cBase->context->counter;
                            cout << "Filho encontrado codificado com prob: " << currentProb << "\n"; 
                            //tem que incluir decodificacao
                            cNode->counter += 1;
                            cBase->context->counter++;
                            found = true;
                            foundedChild = cNode;
                            break;
                        }
                        cNode = cNode->rigthPointer;
                    }

                    if(!found){     //o simbolo nunca foi encontrado
                        
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
                }

                /* Atualiza o ponteiro base */
                if(foundedChild){
                    
                    if(foundedChild->heigth < model_order){
                        //cout << "atualizando base" << endl;
                        base = foundedChild;
                        lastBase = nullptr;
                    }else{
                        //cout << "Nao criou novo filho, mas achou filho\n";
                        lastBase = foundedChild;
                    }
                    attBase = true;
                    break;
                
                }else{
                    /* So atualiza o base na primeira adicao */
                    if(!attBase){   
                        if(child->heigth < model_order){
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
