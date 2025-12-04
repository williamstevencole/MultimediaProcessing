#include <list>
#include <map>
#include <string>
#include <iostream>

struct Node{
    int frequency;
    char character;
    Node *left;
    Node *right;

    Node(char charac, int freq){
        character = charac;
        frequency = freq;
        left = nullptr;
        right = nullptr;
    };
};

bool compareNodes(const Node* left, const Node* right){
    return left->frequency < right->frequency;
}

void generateCodes(Node * root, std::string str, std::map<char, std::string>& codes){
    if(!root) return;

    if(!root->left && !root->right){
        codes[root->character] = str;
    }

    generateCodes(root->left, str+"0", codes);
    generateCodes(root->right, str+"1", codes);
}

void buildHuffmanTree(std::string text){
    std::map<char, int> frequency;

    for(char c: text){
        frequency[c]++;
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
        Node *top = new Node('$', sum);

        top -> left = left;    
        top -> right = right;

        bool inserted = false;
        for(auto it= nodes.begin(); it != nodes.end(); it++){
            if((*it)->frequency >= sum){
                inserted= true;
                nodes.insert(it, top);
                break;
            }
        }

        if(!inserted){
            nodes.push_back(top);
        }
    }

    Node *root = nodes.front();
    std::map<char, std::string> huffmanCodes;
    generateCodes(root, "", huffmanCodes);

    std::cout << "Huffman codes: \n";
    for(auto pair : huffmanCodes){
        std::cout << pair.first << " " << pair.second << std::endl;
    }

    std::cout << "original string: " << text << std::endl;

    std::string encodedString = "";
    for(auto ch: text){
        encodedString+=huffmanCodes[ch];
    }
    std::cout << "encoded string: " << encodedString << std::endl;

}