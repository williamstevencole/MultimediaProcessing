#include <list>
#include <map>
#include <string>
#include <iostream>

struct Node{
    int frequency;
    char character;
    Node *left;
    Node *right;

    Node(char c, int freq){
        frequency = freq;
        character = c;
        left = nullptr;
        right = nullptr;
    }
};

bool compareNodes(const Node *l, const Node *r){
    return l->frequency < r->frequency;
}

void generateCodes(Node *root, const std::string str, std::map<char, std::string> &code){
    if(!root) return;

    if(!root->left && !root->right){
        code[root->character] = str;
    }

    generateCodes(root->left, str + "0", code);
    generateCodes(root->right, str + "1", code);  
}

void buildHuffmanTree(const std::string text){
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
        for(auto it = nodes.begin(); it != nodes.end(); ++it){
            if((*it)->frequency >= newNode->frequency){
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

    for(auto pair: huffmanCode){
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    std::string encodedString = "";
    for(char ch: text){
        encodedString += huffmanCode[ch];
    }

    std::cout << "Encoded string:\n" << encodedString << std::endl;
}

int main(){
    std::string text = "Abracadabra";
    buildHuffmanTree(text);
    return 0;
}