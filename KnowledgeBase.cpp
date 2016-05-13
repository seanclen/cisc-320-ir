#include <fstream>
#include <iostream> 
#include <vector>
#include <deque>
#include <math.h>
#include <string>
#include <cstring>
#include "KnowledgeBase.h"
#include "Nodes.h"
#include "Dictionary.h"

using namespace std;

Node* graph;
std::deque<Node*> unclaimed;
bool checkingUnclaimed;

KnowledgeBase::KnowledgeBase() {
    checkingUnclaimed = false;
}

// Search the given start node for surrounding nodes qualified from parameters

// start:          Root node for search
// search:         String to match qualifying nodes
// minDistance:    The minimum distance from root to qualify nodes
// maxDistance:    The maximum distance from root to qualify nodes
// includeParents: Set to true to search for nodes up the heirarchy, search will
//                 always check below.
// limit:          Maximum number of nodes to accept
//
// Returns a vector of Node's.
vector<Node*> KnowledgeBase::breadth_first_search(Node* start, string search, float minDistance,
                                float maxDistance, bool includeParents, float limit) {
    vector<Node*> results;
    Node* previousNode = NULL;
    
    // EMPTY CASE: if start is null, return empty vector
    if(!start) {
        return results;
    }
    
    // print so far
    // print_graph(start);
    Node* current; // Declare temporary pointer for graph traversal
    
    search = Dictionary::reformat(search);
    //cout << "\nbreadth_first_search for " << search << endl;
    // Handle input. Make sure 
    if (!isinf(minDistance)) {minDistance = floor(minDistance);}
    if (!isinf(maxDistance)) {maxDistance = floor(maxDistance);}
    if (!isinf(limit)) {limit = floor(limit);}
    
    //cout << "start search at " << start->data << endl;
    
    // Initialize all nodes
    // We will start using the graph pointer so we only need to handle
    // the graph node's children
    deque<Node*> init;
    init.push_back(start);
    //cout << "initialize nodes" << endl;
    while (init.size() != 0) {
        current = init.front();
        current->level = 0;
        current->isEvaluated = false;
        for (int i = 0; i < current->children.size(); i++) {
            init.push_back(current->children[i]);
        }
        init.pop_front();
    }
    current = NULL;
    deque<Node*> q;
    start->level = 0;
    start->isEvaluated = false;
    q.push_back(start);
    while(q.size() != 0 && results.size() < limit) {
        previousNode = current;
        current = q.front();
        //cout  << " current: " << current->data << endl;
        // Make sure we are between the search limits
        if (current->level > maxDistance) { // || current->level < minDistance
            q.pop_front();
            //cout << "Past Limit current level " << current->level << endl;
            break;
        }
        //cout << "current: " << current->data << " a search: " << search << endl;
        if (current->data == search || search == "") {
            current->from = previousNode;
            results.push_back(current);
            //cout << "found it!" << endl;
            if (results.size() == limit) {
                return results;
            }
        }
        current->isEvaluated = true;
        q.pop_front();
        // go through children
        if(current->children.size() != 0){
            for(int i = 0; i < current->children.size(); i++){
                current->children[i]->level = current->level + 1;
                if(!current->children[i]->isEvaluated){
                    q.push_back(current->children[i]);
                }
            }
        }
        // go through parents
        if (includeParents && current->parents.size() != 0) {
            for(int i = 0; i < current->parents.size(); i++) {
                current->parents[i]->level = current->level + 1;
                if(!current->parents[i]->isEvaluated){
                    q.push_back(current->parents[i]);
                }
            }
        }
    }
    
    if(results.size() == 0){
        results.push_back(NULL);
        //cout << "no results" << endl;
    }
    
    // for(int i = 0; i < results.size(); i++){
    //     cout << i << ": " << results.at(i) << endl; 
    // }
    return results;
}

// Parse a line string into graph
//
// input:  line of text to parse
void KnowledgeBase::addLineToGraph(string input) {

    // Get the parent by pulling the substring of the parent
    // from index 0 to the index of the delimiter ":"
    
    int first = 0; // Used to store the index of the first char in a word
    int last = 0; // Used to store the index of the last char in a word
    
    // Find the parent delimiter
    last = input.find(":");
    
    // Get the substring from the line and reformat
    string parentString = Dictionary::reformat(input.substr(first, last));
    //cout << "addLineToGraph -> parent: " << parentString;
    Node *parentPointer;
    // If the node does not already exist, then create a new Node in memory
    // to insert into the graph. If it does exist, we continue working from 
    // that pointer.
    if (graph) {
        // cout << " -> graph exists";
        parentPointer = breadth_first_search(graph, parentString, 0, INFINITY, true, 1)[0];
        //cout << "pointer parent: " << parentPointer->from->data << endl;
    } else {
        // cout << " -> initializing new graph" << endl;
        // create new graph
        graph = new Node();
        parentPointer = graph;
        graph->data = parentString;
        // cout << ": " << parentString << endl;
    }
    

    // If the parentPointer is NULL then we need to add this line to the unclaimed
    // deque that holds Node's without a relationship to the graph.
    bool unrelated = false;
    if (!parentPointer){
        unrelated = true;
        
        if (unclaimed.front()) {
            parentPointer = breadth_first_search(unclaimed.front(), parentString, 0, INFINITY, true, 1)[0];
        }
        
        if (!parentPointer){
            // Doesn't exist in unclaimed either, create new pointer
            parentPointer = new Node();
            
            // We set the data of the new node to the extracted data string
            parentPointer->data = parentString;
            unclaimed.push_back(parentPointer);
        } // else found parent in unclaimed
    }
    
    // Continue the search at the index after the first delimiter
    first = last + 1;
    
    // Loop through the rest of the line starting at the index after
    // the parent delimiter to pull every child element
    
    while (last < input.size()) {
        // Get the index of the first char in the next word
        first = input.find_first_not_of(": ", first);
        // Get the index of the last char in the word
        last = input.find(',', first);
        // Get the substring of this child from the current line
        // and reformat
        string childString = Dictionary::reformat(input.substr(first, last - first));
        
        //cout << "childString: " << childString;
        //Move along to the next index
        first = last + 1;
        
        Node *childPointer = breadth_first_search(graph, childString, 0, INFINITY, true, 1)[0];
        // fixed by setting 0 case in bfs, adds empty node though
        
        // If the child already exists in the graph, we will work on that pointer
        // Else this is a new element and must create a new Node.
        
        if (unrelated && childPointer) {
            // A child was found in the graph and can be associated with the
            // unrelated Node and removed from unclaimed
            Node* temp;
            for (int i = 0; i < unclaimed.size(); i++) {
                if (unclaimed.front() == parentPointer) {
                    // Remove Node from unclaimes list
                    // Should come back in the future and check to make sure all
                    // relationships to this Node are also removed
                    unclaimed.pop_front();
                } else {
                    // Else push the front back to the end and keep checking
                    temp = unclaimed.front();
                    unclaimed.pop_front();
                    unclaimed.push_back(temp);
                }
            }
        } // else did not find child in graph
        
        if (!childPointer){
            //cout << "new child pointer for " << childString << " in " << parentString << endl;
            childPointer = new Node();
            childPointer->data = childString;
        }
        
        // Handle the insertion of the child Node into the graph
        childPointer->parents.push_back(parentPointer);
        parentPointer->children.push_back(childPointer);
    }
}

// Parse a .txt file and read into the graph
void KnowledgeBase::create_graph(string file)  {
    ifstream buffer (file);
    vector<string> text;
    string line;
    // read file
    if (buffer.is_open()) {
        while (getline(buffer, line)) {
            //cout << line << endl;
            text.push_back(line);
        }
    } else {
        cout << "Unable to open field";
    }
    
    // Add each element in the line into the graph
    for (int i = 0; i < text.size(); i++) {
        // Parse each line in the text into a node based on it's local relationship
        // i.e. Parent: child, child, child
        //cout << "line " << i << " adding." << endl;
        addLineToGraph(text[i]);
        //cout << "\n\n" << endl;
    }
    buffer.close();
}

// Get the species subtypes of a specific order (distance).
// A value of INFINITY in "order" will include species children

// species:  root species to search from
// order:    distance from root to search for subtypes
// subtypes: the number of subtypes to search for
//
// Returns a vector of Node's.
vector<Node*> KnowledgeBase::getSubtypes(string species, float order, float subtypes) {
    vector<Node*> out;
    Node* start = breadth_first_search(graph, species, 0, INFINITY, true, 1)[0];
    if (start) {
        if (isinf(order)) {
            out = breadth_first_search(start, "", 1, INFINITY, false, subtypes);
        } else {
            out = breadth_first_search(start, "", order, order, false, subtypes);
        }
    } else {
        cout << "The type " << species << "does not exist!" << endl;
    }
    cout << "getSubtypes[return size: " << out.size() << "]" << endl;
    return out;
}

// Search the graph for the species subtypes and return the count ofits children
int KnowledgeBase::getNumSubtypes(string species) {
    vector<Node*> out;
    Node* start = breadth_first_search(graph, species, 0, INFINITY, true, 1)[0];
    
    if (start) {
        out = breadth_first_search(start, "", 1, INFINITY, true, INFINITY);
        return out.size();
    } else {
        cout << "The species does not exist!" << endl;
        return 0;
    }
    return 0;
}

// Find the shortest path to species
bool KnowledgeBase::getPath(vector<Node *> &path, Node* node, string species) {
    
    path.push_back(node);
    species = Dictionary::reformat(species);
    if (node == NULL) {
        return false;
    }
    
    if (node->data == species) {
        return true;
    }
    
    for (int i = 0; i < node->children.size(); i++) {
        if (getPath(path, node->children[i], species)) {
            return true;
        }
    }
    
    path.pop_back();
    return false;
}

// Gets lowest common ancestor given two strings possibly in the graph
Node* KnowledgeBase::getLowestCommonAncestor(Node* start, string sp1, string sp2) {
    vector<Node*> pathSp1, pathSp2;
    
    if (!getPath(pathSp1, start, sp1) || !getPath(pathSp2, start, sp2)) {
        return 0;
    }
    
    int i = 1;
    for (i = 0; i<pathSp1.size() && i<pathSp2.size(); i++) {
        if (pathSp1[i] != pathSp2[i]) {
            break;
        }
    }
    return pathSp1[i-1];
}

Node* KnowledgeBase::ROOT() {
    return graph;
}