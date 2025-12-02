#include <vector>
#include <iostream>
#include <map>
#include <queue>
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

struct Compare{
    bool operator()(Node *l, Node *r){
        return l->frequency > r->frequency;
    }
};

void generateCodes(Node* root, std::string str, std::map<char, std::string> &code){
    if(!root) return;

    if(!root->left && !root->right){
        code[root->character] = str;
    }

    generateCodes(root->left, str + "0", code);
    generateCodes(root->right, str + "1", code);
}

void buildHuffmanTree(std::string text){
    // Count frequency of each character
    std::map<char, int> frequency;
    for(char ch: text){
        frequency[ch]++;
    }
    
    std::priority_queue <Node*, std::vector<Node*>, Compare> pq;

    // Create a leaf node for each character and add it to the priority queue
    for(auto pair: frequency){
        pq.push(new Node(pair.first, pair.second));
    }

    while(pq.size() > 1){
        Node *left = pq.top();
        pq.pop();

        Node *right = pq.top();
        pq.pop();

        int sum = left->frequency + right->frequency;
        Node *top = new Node('$', sum);

        top->left = left;
        top->right = right;

        pq.push(top);
    }

    Node *root = pq.top();
    std::map<char, std::string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    std::cout << "Huffman Codes:\n";
    for(auto pair: huffmanCode){
        std::cout << pair.first << ": " << pair.second << "\n";
    }

    std::cout << "Original string: " << text << "\n";

    std::string encodedString = "";
    for(char ch: text){
        encodedString += huffmanCode[ch];
    }

    std::cout << "Encodeds string: " << encodedString << "\n";
}


int main(){
    std::string text = "BIG BOB BITES BANANAS";
    buildHuffmanTree(text);

    return 0;
}