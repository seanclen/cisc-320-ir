#include <iostream> 
#include <fstream>
#include <vector>
#include <deque>
#include <math.h>
#include <sstream>
#include <string>
#include <cstring>
#include <dirent.h>
#include "Nodes.h"
#include "Dictionary.h"
#include "KnowledgeBase.h"
#include "Document.h"
using namespace std;

Dictionary* stop_words;
vector<Document*> documents;
KnowledgeBase kbase;

vector<string> openFiles(string path)
{
    DIR *dir;
    dirent *pdir;
    vector<string> files;
    dir = opendir(path.c_str());
    while (pdir = readdir(dir))
    {
        if (pdir->d_name[0] != '.') {
            files.push_back(pdir->d_name);
        }
    }
    return files;
}

void load_files(string directory) {
    stop_words->build_from_file("stop_words.txt");
    vector<string> files = openFiles(directory);
    
    for (string file : files) {
        file = directory + file;
        Document* doc = new Document();
        doc->load_file(file, stop_words);
        documents.push_back(doc);
    }
}

void search(string query) {
    int start = 0;
    vector<string> qwords;
    
    // Gather all words from the query into a vector
    string qword;
    stringstream is(query);
    while ( is >> qword ) qwords.push_back( qword );
    
    cout << "Query length: " << qwords.size() << qwords.at(1) << endl;
    
    // Create the index, it will be n documents wide and m words deep
    int numdocs = documents.size(), querysize = qwords.size();
    vector<vector<double> > index;
    index.resize(numdocs);
    for (int i = 0; i < numdocs; ++i)
        index[i].resize(querysize);
    
    // Now cycle through all of the documents searching for the words
    for (int i = 0; i < numdocs; i++) {
        Document* doc = documents[i];
        int docsize = doc->get_total();
        cout << "Document: " << i  << " " << doc->get_name() << endl;
        
        // Check for each word in the current document
        for (int j = 0; j < querysize; j++) {
            string word = qwords[j];
            int wordcount = doc->search(word);
            if (wordcount == 0) {
            /*  The word was not found in the document. Now we want to refer
                back to the constructed knowledge base and search for any 
                related subtypes of the current word
            */
                cout << "Wasn't found ";
                vector<Node*> subtypes = kbase.getSubtypes(word, 3, INFINITY);
                for (Node* node: subtypes) {
                    wordcount = doc->search(node->data);
                    if (wordcount) break;
                }
            }
            double frequency = (double) wordcount / docsize;
            cout << wordcount << " " << docsize << " " << frequency << endl;
            index[i][j] = frequency;
        }
    }
}

int main() {
    stop_words = new Dictionary();
    
    cout << "complete" << endl;
    string file = "hyponymy.txt";
    
    kbase.create_graph(file);
    
    // Get all of the subtype nodes
    vector<Node*> subtypes = kbase.getSubtypes("Cat", 1, INFINITY);
    // for (Node* n : subtypes) {
    //     cout << n->data << endl;
    // }
    
    // Get the number of subtypes
    cout << "\ngetNumSubtypes " << kbase.getNumSubtypes("CAT") << endl;
    
    // Find the shortest path from a starting node and search for the species
    vector<Node*> path;
    cout << "\ngetPath" << endl;
    kbase.getPath(path, kbase.ROOT(), "HORSE");
    for (Node* n : path) {
        cout << n->data << endl;
    }
    
    // Find the lowest common ancestor between species
    Node* node = kbase.getLowestCommonAncestor(kbase.ROOT(), "Dog", "Cat");
    cout << "\nLowest Common Ancestor: " << node->data << endl;
    
    load_files("texts_project/");
    
    search("Here's a test string");
    
    return 0;
}

