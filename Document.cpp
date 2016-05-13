#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <vector>
#include <dirent.h>
#include "Document.h"
#include "Dictionary.h"
#include "Stemmer.h"
using namespace std;


    
Document::Document() {
    name = "";
    original_file = "";
    cleaned_file = "";
    index = new Dictionary();
}

void Document::load_file(string file, Dictionary* stop_words) {
    cout << "Document::load_file" << endl;
    name = file.substr(0, (file.size() - 4));
    original_file = file;
    cleaned_file = "cleaned_corpus/" + name + "_cleaned.txt";
    string word;
    ifstream buffer (file);
    Stemmer stemmer;
    if (buffer.is_open())
    {
        stringstream temp;
        while (buffer >> word)
        {
            word = Dictionary::reformat(word);
            if (!stop_words->contains(word)) {
                word = stemmer.stem_word(word);
                temp << word << " ";
                index->add_word(word);
            }
        }
        buffer.close();
        
        ofstream output (cleaned_file);
        output << temp.rdbuf();
        
        output.close();
    }
    else {
        cout << "Unable to open file";
    }
    
    cout << "Finished" << endl;
}

int Document::search(string word) {
    cout << "Document::search : " << word << endl;
    return index->contains(word);
}

int Document::get_total() {
    return index->get_total();
}

string Document::get_name() {
    return name;
}