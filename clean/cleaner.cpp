#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <vector>
#include <dirent.h>
#include "Dictionary.h"
#include "Stemmer.h"
using namespace std;

void clean(string directory);
Dictionary stop_words;

int main() {
    clean("texts_project/");
    //Stemmer stemmer;
    //cout << "Stem: " << stemmer.stem_word("CATS") << endl;
    return 0;
}

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

void removeStopWords(string file) {
    string word;
    ifstream buffer (file);
    if (buffer.is_open())
    {
        stringstream temp;
        while (buffer >> word)
        {
            word = stop_words.reformat(word);
            if (!stop_words.contains(word)) {
                temp << word << " ";
            }
        }
        buffer.close();
        
        ofstream output (file);
        output << temp.rdbuf();
        
        output.close();
    }
    else cout << "Unable to open file";
}

void stemWords(string file) {
    string word;
    ifstream buffer (file);
    Stemmer stemmer;
    if (buffer.is_open())
    {
        stringstream temp;
        while (buffer >> word)
        {
            word = stop_words.reformat(word);
            temp << stemmer.stem_word(word) << " ";
        }
        buffer.close();
        
        ofstream output (file);
        output << temp.rdbuf();
        
        output.close();
    }
    else cout << "Unable to open file";
}

/* 
Copy the contents from file1 into file2
Input should be the file path string from the root
*/
void copyInto(string file1, string file2) {
    ifstream infile (file1);
    ofstream outfile (file2);
    string line = "";
    while(getline(infile, line)) {
        outfile << line;
    }
    infile.close();
    outfile.close();
}

/*
Clean a file by removing stop words and stemming the result.
Clean takes one parameter, directory, which is the root directory
that the function will search for *.txt files and clean them.
Will store the cleaned files ./cleaned_corpus.
Currently loads files from /clean/texts_project and stop words
from /clean/stop_words.txt
*/
void clean(string directory) {
    cout << "Type 1 if you want to clean your data and something else to exit" << endl;
    string input;
    cin >> input;
    if (input != "1") {
        cout << "Bye!";
        return;
    }
    stop_words.build_from_file("stop_words.txt");
    vector<string> files = openFiles(directory);
    cout << "Number of Files: " << files.size() << endl;
    
    int filecounter = 1;
    for (string file : files) {
        cout << "\rCleaning file " << filecounter << " of " << files.size();
        string inputPath = directory + file;
        string corpusPath = "cleaned_corpus/" + file.substr(0, (file.size() - 4)) + "_cleaned.txt";
        
        copyInto(inputPath, corpusPath);
        stemWords(corpusPath);
        removeStopWords(corpusPath);
        
        filecounter++;
    }
    
    cout << "\nCompleted" << endl;
}