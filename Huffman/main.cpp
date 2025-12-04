#include <vector>
#include <iostream>
#include <list>
#include <map>
#include <string>

struct Node{
    char character;
    int frequency;
    Node* left;
    Node* right;

    Node(char ch, int freq){
        character = ch;
        frequency = freq;
        left = nullptr;
        right = nullptr;
    }
};


bool compareNodes(Node *l, Node *r){
    return l->frequency < r->frequency;
}

void generateCodes(Node *root, const std::string str, std::map<char, std::string> &code){
    if(!root) return;

    if(root->left == nullptr && root->right == nullptr){
        code[root->character] = str;
    }

    generateCodes(root->left, str + "0", code);
    generateCodes(root->right, str + "1", code);
}

void buildHuffmanTree(std::string text){
    std::map<char, int> frequency;
    for(char ch: text){
        frequency[ch]++;
    }

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
        Node *newNode = new Node('$', sum);

        newNode->left = left;
        newNode->right = right;

        bool inserted = false;
        for(auto it = nodes.begin(); it != nodes.end(); it++){
            if((*it)->frequency >= sum){
                nodes.insert(it, newNode);
                inserted = true;
                break;
            }
        }

        if(!inserted){
            nodes.push_back(newNode);
        }
    }

    Node *root = nodes.front();
    std::map<char, std::string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    std::cout << "Huffman codes:\n";
    for(auto pair: huffmanCode){
        std::cout << pair.first << ": " << pair.second << "\n";
    
    
    std::string encodedString = "";
    for(char ch: text){
        encodedString += huffmanCode[ch];
    }
    std::cout << "Encoded string:\n" << encodedString << "\n";
}

int main() {
    std::string text = "huffman coding example";
    buildHuffmanTree(text);
    return 0;
}