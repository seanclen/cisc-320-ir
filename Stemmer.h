#ifndef STEMMER_H_
#define STEMMER_H_

class Stemmer {
    int endIndex;
    int stemIndex;
    char* word;
    
    bool is_vowel(char);
    bool has_vowel();
    bool ends_with(std::string);
    bool invalid_word();
    void set_end(std::string);
    void replace_end(std::string);
    int get_measure();
    bool has_double_consonants(int);
    bool is_cvc(int);
    void step1();
    void step2();
    void step3();
    void step4();
    void step5();
  public:
    Stemmer();
    std::string stem_word(std::string);
};

#endif