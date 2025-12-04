#include <string>
#include <map>
#include <list>
#include <iostream>
#include <fstream>

struct Node{
    int frequency;
    char ch;
    Node *left = nullptr;
    Node *right = nullptr;

    Node(char c, int freq){
        frequency = freq;
        ch = c;
        left = nullptr;
        right = nullptr;
    }
};

bool compareNodes(const Node *l , const Node *r){
    return l->frequency < r->frequency;
}

void generateCodes(Node*root, std::string str, std::map<char, std::string>&codes){
    if(!root) return;

    if(!root->left && !root->right){
        codes[root->ch]=str;
    }

    generateCodes(root->left, str+"0", codes);
    generateCodes(root->right, str+"1", codes);
}


Node* buildHuffmanTreeFromMap(std::map<char, int>& frequency){
    std::list<Node*> nodes;

    for(auto pair: frequency){
        nodes.push_back(new Node(pair.first, pair.second));
    }

    nodes.sort(compareNodes);

    while(nodes.size() > 1){
        Node *left = nodes.front();
        nodes.pop_front();

        Node *right = nodes.front();
        nodes.pop_front();

        int sum = left->frequency + right->frequency;
        Node *top = new Node('$', sum);

        top->left = left;
        top->right = right;

        bool inserted = false;
        for(auto it = nodes.begin(); it != nodes.end(); it++){
            if((*it)->frequency >= sum){
                nodes.insert(it, top);
                inserted = true;
                break;
            }
        }
        if(!inserted){
            nodes.push_back(top);
        }
    }

    return nodes.front();
}


void comprimir(std::string archivoEntrada, std::string archivoTabla, std::string archivoBinario){
    std::ifstream in(archivoEntrada);
    if(!in.is_open()){
        std::cout << "Error abriendo archivo entrada" << std::endl;
        return;
    }

    std::map<char, int> frequencies;
    char c;
    long long totalCharacters = 0;

    while(in.get(c)){
        frequencies[c]++;
        totalCharacters++;
    }
    in.close();

    if(totalCharacters == 0) return;


    Node *root = buildHuffmanTreeFromMap(frequencies);
    std::map<char, std::string> codes;
    generateCodes(root, "", codes);

    std::ofstream outTable(archivoTabla);
    outTable << totalCharacters << std::endl; // total de characteres en palabra original
    outTable << frequencies.size() << std::endl; // cantidad de claves en el mapa


    for(auto pair: frequencies){
        outTable << static_cast<int>(pair.first) << " " << pair.second << std::endl;
    }

    outTable.close();


    char byteBuffer = 0;
    int bitCount = 0;

    std::ifstream inTexto(archivoEntrada);
    std::ofstream outBin(archivoBinario, std::ios::binary);

    while(inTexto.get(c)){
        std::string codigo = codes[c];

        for(char bit: codigo){
            byteBuffer = byteBuffer << 1;

            if(bit == '1'){
                byteBuffer = byteBuffer | 1;
            }

            bitCount++;

            if(bitCount == 8){
                outBin.put(byteBuffer);
                byteBuffer = 0;
                bitCount = 0;
            }
        }
    }

    if(bitCount > 0){
        byteBuffer = byteBuffer << (8-bitCount);
        outBin << byteBuffer;
    }


    inTexto.close();
    outBin.close();
}


void descomprimir(std::string archivoTabla, std::string archivoBinario, std::string archivoSalida){
    std::ifstream inTable(archivoTabla);
    if(!inTable.is_open()) return;

    long long totalCaracteresReales;
    int tamanoMapa;

    inTable >> totalCaracteresReales;
    inTable >> tamanoMapa;

    std::map<char, int> frecuencias;
    int asciiCode;
    int freq;

    for(int i = 0; i < tamanoMapa; i++){
        inTable >> asciiCode >> freq;
        frecuencias[static_cast<char>(asciiCode)] = freq;
    }
    inTable.close();

    Node* root = buildHuffmanTreeFromMap(frecuencias);

    std::ifstream inBin(archivoBinario, std::ios::binary);
    std::ofstream outSalida(archivoSalida);

    Node *actual = root;
    long long caracteresEncontrados = 0;
    char byteLeido;

    while(inBin.get(byteLeido)){

        for(int i = 7; i >= 0; i--){
            if(caracteresEncontrados == totalCaracteresReales) break;

            int bit = (byteLeido >> i) & 1;

            if(bit == 0){
                actual = actual->left;
            }else{
                actual = actual->right;
            }

            if(!actual->left && !actual->right){
                outSalida.put(actual->ch);
                caracteresEncontrados++;
                actual = root;
            }

        }
    }


    inBin.close();
    outSalida.close();

}



int main(){
    //buildHuffmanTree("abracadabra");

    std::ofstream f("prueba.txt");
    f << "abracadabra";
    f.close();

    comprimir("prueba.txt", "tabla.txt", "comprimido.bin");
    descomprimir("tabla.txt", "comprimido.bin", "resultado.txt");

    return 0;
}