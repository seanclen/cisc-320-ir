#ifndef DICTIONARY_H_
#define DICTIONARY_H_

class Dictionary {
    void add_word(std::string);
  public:
    Dictionary();
    bool build_from_file(std::string);
    bool contains(std::string);
    std::string reformat(std::string);
};

#endif