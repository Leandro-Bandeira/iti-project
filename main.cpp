#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <fstream>
#include <random>
#include <sstream>
#include <iomanip>


#include "Structs.hpp"

using namespace std;

#define model_order 2   //ordem do modelo
#define MAX_CHAR_GENERATE 10

TNode* createChild(TNode* father, unsigned char symbol){
    TNode* child = new TNode();
    if(father->isroot)child->vine = father;
    child->heigth = father->heigth + 1;    
    child->counter += 1;
    child->symbol = symbol;
    if(father->lastCreate){
        father->lastCreate->rigthPointer = child;
        father->lastCreate = child;

    }else{
        father->lastCreate = child;
    }
    return child;
}


/* Para cada contexto retorna sua vetor de frequencias */
vector<pair<int, int>>* getFrequenciesContext(TNode* father) {

    /* O par representar (ocorrencia, simbolo), o inteiro 256 equivale ao ro*/
    vector<pair<int, int>>* frequencies = new vector<pair<int, int>>();

    
    TNode* currentNode = father->downPointer;

    
    while (currentNode) {
        frequencies->push_back(make_pair(currentNode->counter,
                                         static_cast<int>(currentNode->symbol)));
        currentNode = currentNode->rigthPointer;
    }

    // adiciona o ro como símbolo 256
    frequencies->push_back(make_pair(father->context->counter_ro, 256));

    return frequencies;
}
/* Codigo responsavel por receber as frequencias e retornar o caracter gerado */
int spinRoulette(const std::vector<int>& occurrences) {
    int total = std::accumulate(occurrences.begin(), occurrences.end(), 0);


    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, total - 1);

    /* Caso eu tenha  {3, 7, 10} como as ocorrencias, minha distribuicao discreta e de (0, 20)
    * Caso, r caia entre {0, 1, 2, 3}, o primeiro valor eh escolho e assim por diante */
    int r = dist(gen);
    int cumulative = 0;
    for (int i = 0; i < occurrences.size(); i++) {
        cumulative += occurrences[i];
        if (r < cumulative) return i;
    }
    return -1;
}

/* Funcao responsavel para capturar apenas as frequencias */
std::vector<int> extractFrequencies(const vector<pair<int,int>>& freqTuples) {
    std::vector<int> freqs;
    freqs.reserve(freqTuples.size());
    for (auto& t : freqTuples) {
        freqs.push_back(t.first); // pega só o counter
    }
    return freqs;
}

// Função que imprime as probabilidades dos símbolos no contexto de um nó
void printProbabilitiesContext(TNode* contextNode) {
    if (!contextNode || !contextNode->context) {
        std::cout << "Contexto inválido ou sem dados.\n";
        return;
    }
    int total = contextNode->context->counter;
    std::cout << "Lista de probabilidades para contexto de ordem " << contextNode->heigth << ":\n";
    TNode* current = contextNode->downPointer;
    while (current) {
        double prob = static_cast<double>(current->counter) / total;
        std::cout << std::setprecision(4) << static_cast<unsigned char>(current->symbol) << " = " << prob << std::endl;
        current = current->rigthPointer;
    }
    // Probabilidade do símbolo especial 'ro' (escape)
    if (contextNode->context->counter_ro > 0) {
        double prob_ro = static_cast<double>(contextNode->context->counter_ro) / total;
        std::cout << "ro = " << prob_ro << std::endl;
    }
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
    
    /* Quantia codificado na raiz*/
    int countCodRoot = 0; 
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
                    countCodRoot += 1;
                    currentProb = cBase->probValue;
                    cout << "O caracter (" << c << ") foi codificado com probabilidade: " << currentProb << "\n";
                    // Imprime as probabilidades dos símbolos no contexto atual
                    printProbabilitiesContext(cBase);
                    //atualizacao dos contextos:
                    cBase->counter--;                         //nao sei oq danado eh isso
                    cBase->probValue =  1.0 / cBase->counter; //prob nova para o c0 (pq diminuiu um caractere)

                    TNode* child = createChild(&root, c);     //criando um filho que estará no contexto1 
                    
                    /* Caso seja o primeiro filho da raiz */
                    if (countCodRoot == 1){                
                        base = child;                                 
                        root.downPointer = child;    
                        root.context = new Context();    
                        root.context->counter_ro += 1;    
                        root.context->counter = root.context->counter_ro + 1;  
                    }
                    else{
                        currentProb_ro = (double)root.context->counter_ro / (root.context->counter - minusCounter);
                        root.context->counter_ro += 1;
                        root.context->counter += 2;
                    }
                    
                    if(countCodRoot == diffElements){ 
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
                    
                           
                            currentProb = (double)cNode->counter / root.context->counter;
                            cout << cNode->counter << " " << root.context->counter << endl;
                            cout << "Codificando (" << c << ") em k == 0 com probabilidade: " << currentProb << "\n";
                            cNode->counter++;
                            root.context->counter += 1;
                            if(childCreated)childCreated->vine = cNode;
                            
                            // Imprime as probabilidades dos símbolos no contexto atual
                            printProbabilitiesContext(cBase);
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
                    // Imprime as probabilidades dos símbolos no contexto atual
                    printProbabilitiesContext(cBase);
                    break;
                }

                TNode* child = nullptr;
                cout << "O node com simbolo: " << currentSymbol << endl;
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
                            cout << "Filho " << currentSymbol << " encontrado codificado com prob: " << currentProb << "\n"; 
                            //tem que incluir decodificacao
                            cNode->counter += 1;
                            cBase->context->counter++;
                            found = true;
                            foundedChild = cNode;
                            // Imprime as probabilidades dos símbolos no contexto atual
                            printProbabilitiesContext(cBase);
                            /* Se a gente condificar o elemento aqui, subir nos contextos e aumentar os contadores */
                            TNode* startNode = cNode->vine;
                            while(!startNode->isroot){
                                startNode->counter++;
                                startNode = startNode->vine;
                            }
                        
                            break;
                        }
                        cNode = cNode->rigthPointer;
                    }

                    if(!found){     //o simbolo nunca foi encontrado
                        
                        //TNode* lastCreate = cBase->lastCreate;
                        child = createChild(cBase, c);
                        if(childCreated) childCreated->vine = child;
                        
                        childCreated = child;
                        
                        
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
    

    /* Gerando texto a partir da arvore treinada */
    TNode *currentNode = &root;
    int count_generate_char = 0;

    stringstream generatedString;

    while(count_generate_char < MAX_CHAR_GENERATE){
        
        /* A partir das frequencias capturadas, jogamos na roleta russa viciada para capturar o caractere*/
        
        vector<pair<int, int>>* currentFrequencies = getFrequenciesContext(currentNode);
        vector<int> freqVec = extractFrequencies(*currentFrequencies);
        //if(currentNode->isroot)cout << "na raiz" << endl;
        //cout << "tabela de frequencias atual: ";
        //for(pair<int,int> a: *currentFrequencies){
          //  cout << "("<< a.first << "," << static_cast<unsigned char>(a.second) << ")";
        //}
        cout << endl;
        int idx = spinRoulette(freqVec);
        int chosenChar = currentFrequencies->at(idx).second;
        
        cout << "chosenChar: " << static_cast<unsigned char>(chosenChar) << endl;
        /* Caso nao caiamos no ro, gera o caractere e desce na arvore */
        if(chosenChar != 256){
            unsigned char generatedChar = static_cast<unsigned char>(chosenChar);
            generatedString << generatedChar;
            count_generate_char += 1;
            /* Encontra o no responsavel por aquele simbolo */
            TNode* auxNode = currentNode->downPointer;
            while(auxNode->symbol != generatedChar){
                auxNode = auxNode->rigthPointer;

            }
            currentNode = auxNode;
            /* Precisamos ir subindo ate encontrar algum contexto que consiga existir */
            while(!currentNode->downPointer){
                currentNode = currentNode->vine;
                //cout << "subindo no contexto\n";
            }
        }else{
            /* Caso geremos o ro, devemos subir no contexto */
            //cout << "Encontrou o ro\n";
            currentNode = currentNode->vine;
        }
    }
    
    string generatedText = generatedString.str();
    cout << "String gerada: " << generatedText << endl;
    return 0;
}
