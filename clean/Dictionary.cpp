#include <string>
#include <iostream>
#include <fstream>
#include "Dictionary.h"

using namespace std;

struct BinaryNode {
    string data;
    BinaryNode* parent;
    BinaryNode* left;
    BinaryNode* right;
};

BinaryNode* root;

Dictionary::Dictionary() {
    root = new BinaryNode;
    root->data = "";
    root->left = NULL;
    root->right = NULL;
    root->parent = NULL;
}

// THIS EXISTS IN MAIN, THIS WILL ONLY BE TEMPORARY
// Reformat a string to uppercase and replace spaces with underscores
// this sTrinG-here => THIS_STRING_HERE
string Dictionary::reformat(string s) {
    // Loop through each character and make necessary changes
    for (int i = 0; i < s.size(); i++) {
        switch (s[i]) {
            case ' ':
                s[i] = '_';
                break;
            case '_':
                break;
            case '-':
                s[i] = '_';
                break;
            default:
                s[i] = toupper(s[i]);
                break;
        }
    }
    return s;
};

/* 
Add the word to the Binary Tree. We will organize the words alphabetically where
A > B > C ... Y > Z
*/
void Dictionary::add_word(string word) {
    // Base Case: first word inserted into tree
    if (root->data == "") {
        root->data = word;
        root->left = NULL;
        root->right = NULL;
        root->parent = NULL;
        return;
    }
    
    /*
    Search the tree for the correct position to place the new word
    First we reformat the strings to eliminate type discrepancies.
    
    Parse through each character of the word, if the current char is less than
    the char in the node i.e.(search: b - node: a), then move to the node to
    the left. On the contrary, move to the right.
    
    For words that qualify as a substring of an already existing node, store
    to the left.
    */
    bool running = true;
    BinaryNode* temp = root;
    string temp_reformatted, word_reformatted = reformat(word);
    while (running) {
        temp_reformatted = reformat(temp->data);
        for (int i = 0; i < word_reformatted.size() && i < temp_reformatted.size(); i++) {
            if (word_reformatted.at(i) < temp_reformatted.at(i)) {
                if (temp->left) {
                    temp = temp->left;
                } else {
                    temp->left = new BinaryNode;
                    temp->left->parent = temp;
                    temp->left->data = word;
                    temp->left->left = NULL;
                    temp->left->right = NULL;
                    running = false;
                }
                break;
            } else if (word_reformatted.at(i) > temp_reformatted.at(i)) {
                if (temp->right) {
                    temp = temp->right;
                } else {
                    temp->right = new BinaryNode;
                    temp->right->parent = temp;
                    temp->right->data = word;
                    temp->right->left = NULL;
                    temp->right->right = NULL;
                    running = false;
                }
                break;
            } else if (i == word.size() - 1 || i == temp->data.size() - 1) {
                if (temp->left) {
                    temp = temp->left;
                } else {
                    temp->left = new BinaryNode;
                    temp->left->parent = temp;
                    temp->left->data = word;
                    temp->left->left = NULL;
                    temp->left->right = NULL;
                    running = false;
                }
                break;
            }
        }
    }
}

/*
Search the tree for the word.
First we reformat the strings to eliminate type discrepancies.

Parse through each character of the word, if the current char is less than
the char in the node i.e.(search: b - node: a), then move to the node to
the left. On the contrary, move to the right.
*/
bool Dictionary::contains(string word) {
    bool running = true;
    BinaryNode* temp = root;
    string temp_reformatted, word_reformatted = reformat(word);
    while (running) {
        temp_reformatted = reformat(temp->data);
        if (temp_reformatted == word_reformatted) {
            return true;
        }
        for (int i = 0; i < word_reformatted.size() && i < temp_reformatted.size(); i++) {
            if (word_reformatted.at(i) < temp_reformatted.at(i)) {
                if (temp->left) {
                    temp = temp->left;
                } else {
                    return false;
                }
                break;
            } else if (word_reformatted.at(i) > temp_reformatted.at(i)) {
                if (temp->right) {
                    temp = temp->right;
                } else {
                    return false;
                }
                break;
            } else if (i == word.size() - 1 || i == temp->data.size() - 1) {
                if (temp->left) {
                    temp = temp->left;
                } else {
                    return false;
                }
                break;
            }
        }
    }
    return false;
}

bool Dictionary::build_from_file(string file) {
    string word;
    ifstream buffer (file);
    if (buffer.is_open())
    {
        while (buffer >> word)
        {
            add_word(word);
        }
    }
}
// Dictionary::~Dictionary() {

// }