#ifndef KNOWLEDGEBASE_H_
#define KNOWLEDGEBASE_H_
#include <vector>
#include <deque>
#include "Nodes.h"
class KnowledgeBase {
    
  public:
    KnowledgeBase();
    std::vector<Node*> breadth_first_search(Node*, std::string, float, float, bool, float);
    void addLineToGraph(std::string);
    void create_graph(std::string);
    std::vector<Node*> getSubtypes(std::string, float, float);
    int getNumSubtypes(std::string);
    bool getPath(std::vector<Node *>&, Node*, std::string);
    Node* getLowestCommonAncestor(Node*, std::string, std::string);
    Node* ROOT();
};

#endif