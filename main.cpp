#include <iostream> 
#include <fstream>
#include <vector>
#include <deque>
#include <math.h>
#include <sstream>
#include <string>
#include <cstring>
#include <dirent.h>
#include <algorithm>
#include "Nodes.h"
#include "Dictionary.h"
#include "KnowledgeBase.h"
#include "Document.h"
using namespace std;

Dictionary* stop_words;
vector<Document*> documents;
KnowledgeBase kbase;
int querysize;
vector<string> ignore_words = {};

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

int search_count = 0;
void build_search_index(string query) {
    search_count++;
    int start = 0;
    vector<string> qwords;
    
    // Gather all words from the query into a vector
    string qword;
    stringstream is(query);
    while ( is >> qword ) qwords.push_back( qword );
    
    // Create the index, it will be n documents wide and m words deep
    int numdocs = documents.size();
    querysize = qwords.size();
    vector<vector<double> > index;
    index.resize(numdocs);
    for (int i = 0; i < numdocs; ++i)
        index[i].resize(querysize);
    
    // Now cycle through all of the documents searching for the words
    for (int i = 0; i < numdocs; i++) {
        Document* doc = documents[i];
        int docsize = doc->get_total();
        
        double total_frequency = 0.0;
        // Check for each word in the current document
        for (int j = 0; j < querysize; j++) {
            string word = qwords[j];
            double frequency = (double) doc->search(word) / docsize;
            index[i][j] = frequency;
            total_frequency += abs((1/querysize) - frequency);
        }
        doc->set_frequency(sqrt(total_frequency));
    }
}

bool sort_documents(Document* left, Document* right) {
    return left->get_frequency() < right->get_frequency();
}

vector<Document*> filter_search(string query, int numResults, double threshold) {
    bool valid_result = true;
    vector<Document*> results;
    results.resize(numResults);
    build_search_index(query);
    sort(documents.begin(), documents.end(), sort_documents);
    for (int i = 0; i < numResults; i++) {
        results.push_back(documents[i]);
        cout << documents[i]->get_name() << endl;
        cout << documents[i]->get_frequency() << endl;
        if (documents[i]->get_frequency() < threshold && search_count < querysize + 1) {
            /* Find the original word in the query with the smallest frequency
            Add three subtypes of this word to the query and search again.
            When checking for the least significant frequency, we will want to
            ignore the subtypes that have already been injected into the query
            When - if - each original word from the query has been expanded then
            stop the search and display the current results as well as a
            notification that the search has not fulfilled the give threshold
            */
            string newQuery = query;
            stringstream ss(query);
            string qword;
            while ( ss >> qword) {
                if (find(ignore_words.begin(), ignore_words.end(), qword) != ignore_words.end()) {
                    continue;
                }
                vector<Node*> temp = kbase.getSubtypes(qword, 1, 3);
                for (Node* n : temp) {
                    newQuery += " " + n->data;
                    ignore_words.push_back(n->data);
                }
            }
            results = filter_search(newQuery, numResults, threshold);
            return results;
        }
    }
    return results;
}

int main() {
    stop_words = new Dictionary();
    string file = "hyponymy.txt";
    kbase.create_graph(file);
    load_files("texts_project/");
    
    // Enter the search query, then number of results, then search threshold.
    filter_search("Test string goes here right now a hello", 6, 0.01);
    
    return 0;
}

