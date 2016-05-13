#ifndef DICTIONARY_H_
#define DICTIONARY_H_

class Dictionary {
  private:
    int total;
  public:
    Dictionary();
    void add_word(std::string);
    bool build_from_file(std::string);
    int contains(std::string);
    static std::string reformat(std::string);
    int get_total();
};

#endif