#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <string>
#include "Dictionary.h"

class Document {
  private:
    std::string name;
    std::string original_file;
    std::string cleaned_file;
    Dictionary* index;
  public:
    Document();
    void load_file(std::string, Dictionary*);
    int search(std::string);
    int get_total();
    std::string get_name();
};

#endif