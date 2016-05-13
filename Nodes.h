#ifndef NODES_H_
#define NODES_H_

#include <string>
#include <vector>

struct Node{
    std::string data;
    std::vector<Node*> parents;
    std::vector<Node*> children;
    Node* from;
    int level;
    bool isEvaluated;
    // char search_state; // undiscovered
};

struct BinaryNode {
    std::string data;
    int count;
    BinaryNode* parent;
    BinaryNode* left;
    BinaryNode* right;
};

#endif