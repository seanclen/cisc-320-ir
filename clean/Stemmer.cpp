/*
Built using Porter's Stemming algorithm found at:
Tartarus - http://snowball.tartarus.org/algorithms/porter/stemmer.html
*/

#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include "Stemmer.h"

using namespace std;

Stemmer::Stemmer() {
    stemIndex = 0; // Used to store the temp potential word ending
}

// Check to make sure we are only working with a valid string
bool Stemmer::invalid_word() {
    if (strlen(word) == 0) return true;
    for (int i = 0; i < strlen(word); i++) {
        if (word[i] == ' ') return true;
    }
    return false;
}

// Check to see if c is a vowel
bool Stemmer::is_vowel(char c) {
    if (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') return true;
    // We might want to start considering how to handle 'y' here
    return false;
}

// Check word to see if the (potential) stem contains a vowel
bool Stemmer::has_vowel() {
	for (int i = 0; i <= stemIndex; i++) {
		if (is_vowel(i)) return true;
	}
	return false;
}

// Check if the word ends with string s
bool Stemmer::ends_with(string s) {
	int length = s.size();
	int index = endIndex - length + 1;
	if (index < 0) return false; // Don't be silly...

	for (int i = 0; i < length; i++) {
		if (word[index + i] != s[i]) return false;
	}
	stemIndex = endIndex - length;
	return true;
}

int Stemmer::get_measure() {
    int m = 0;
    int index = 0;
    
    // Find the index of the first vowel
    while (true) {
        if (index >= strlen(word)) return 0;
        if (is_vowel(word[index])) break;
        index++;
    }
    return 0;
}

// Set the end of the word to s.
void Stemmer::set_end(string s) {
	int length = s.size();
	int index = stemIndex + 1;
	for (int i = 0; i < length; i++) {
		word[index + i] = s[i];
	}
	endIndex = stemIndex + length;
}

// Use this to speed up the replacement process when checking the measure
void Stemmer::replace_end(string s) {
	if (get_measure() > 0) set_end(s);
}

//Check to see if there are two consonants at the given index
bool Stemmer::has_double_consonants(int index) {
    if (index < 1) return false;
	if (word[index] == word[index - 1] && !is_vowel(index)) {
	    return true;
	} else {
	    return false;
	}
}

// Check to see if the word has the form consonant vowel consonant ending at index
// i.e. index - 2: C, index - 1: V, index: C
bool Stemmer::is_cvc(int index) {
	if (index < 2 || is_vowel(index) || !is_vowel(index - 1) || is_vowel(index - 2)) return false;
	char c = word[index];
	if (c != 'W' && c != 'X' && c != 'Y') {
	    return true;
	} else {
	    return false;
	}
}


// Get rid of plurals and -ed or -ing
void Stemmer::step1() {
    if (word[endIndex] == 'S') {
        if (ends_with("SSES")) {
            endIndex -= 2;
        }
        else if (ends_with("IES")) {
            set_end("I");
        }
        else if (word[endIndex - 1] != 'S') {
            endIndex--;
        }
    }
    
    if (get_measure() > 0 && ends_with("EED")) {
        endIndex--;
    }
    else if (ends_with("ED") || ends_with("ING") && has_vowel()) {
        endIndex = stemIndex;
        if (ends_with("AT")) { set_end("ATE"); }
        else if (ends_with("BL")) { set_end("BLE"); }
        else if (ends_with("IZ")) { set_end("IZE"); }
        else if (has_double_consonants(endIndex)) {
            endIndex--;
            char c = word[endIndex];
			if (c == 'L' || c == 'S' || c == 'Z') { endIndex++; }
        }
        else if (get_measure() == 1 && is_cvc(endIndex)) { set_end("E"); }
    }
    
    // Step 1c refer to Tartarus
    if (ends_with("Y") && has_vowel()) {
		word[endIndex] = 'I';
    }
}

void Stemmer::step2() {
    if (endIndex == 0) return;

    /*
    - From Tartarus
    The test for string word can be made fast by doing a program switch on
    the penultimate letter of the word being tested
    */
	switch (word[endIndex - 1]) {
		case 'A':
			if (ends_with("ATIONAL")) { replace_end("ATE"); break; }
			if (ends_with("TIONAL")) { replace_end("TION"); break; }
			break;
		case 'C':
			if (ends_with("ENCI")) { replace_end("ENCE"); break; }
			if (ends_with("ANCI")) { replace_end("ANCE"); break; }
			break;
		case 'E':
			if (ends_with("IZER")) { replace_end("IZE"); break; }
			break;
		case 'L':
			if (ends_with("BLI")) { replace_end("BLE"); break; }
			if (ends_with("ALLI")) { replace_end("AL"); break; }
			if (ends_with("ENTLI")) { replace_end("ENT"); break; }
			if (ends_with("ELI")) { replace_end("E"); break; }
			if (ends_with("OUSLI")) { replace_end("OUS"); break; }
			break;
		case 'O':
			if (ends_with("IZATION")) { replace_end("IZE"); break; }
			if (ends_with("ATION")) { replace_end("ATE"); break; }
			if (ends_with("ATOR")) { replace_end("ATE"); break; }
			break;
		case 'S':
			if (ends_with("ALISM")) { replace_end("AL"); break; }
			if (ends_with("IVENESS")) { replace_end("IVE"); break; }
			if (ends_with("FULNESS")) { replace_end("FUL"); break; }
			if (ends_with("OUSNESS")) { replace_end("OUS"); break; }
			break;
		case 'T':
			if (ends_with("ALITI")) { replace_end("AL"); break; }
			if (ends_with("IVITI")) { replace_end("IVE"); break; }
			if (ends_with("BILITY")) { replace_end("BLE"); break; }
			break;
		case 'G':
			if (ends_with("LOGI")) { replace_end("LOG"); break; }
			break;
	}
}

void Stemmer::step3() {
    switch (word[endIndex]) {
		case 'E':
			if (ends_with("ICATE")) { replace_end("IC"); break; }
			if (ends_with("ATIVE")) { replace_end(""); break; }
			if (ends_with("ALIZE")) { replace_end("AL"); }
			break;
		case 'I':
			if (ends_with("ICITI")) { replace_end("IC"); }
			break;
		case 'L':
			if (ends_with("ICAL")) { replace_end("IC"); break; }
			if (ends_with("FUL")) { replace_end(""); }
			break;
		case 'S':
			if (ends_with("NESS")) { replace_end(""); }
			break;
	}
}

void Stemmer::step4() {
    if (endIndex == 0) return; //Again... don't be silly

	switch (word[endIndex - 1]) {
		case 'A':
			if (ends_with("AL")) break;
			return;
		case 'C':
			if (ends_with("ANCE")) break;
			if (ends_with("ENCE")) break;
			return;
		case 'E':
			if (ends_with("ER")) break;
			return;
		case 'I':
			if (ends_with("IC")) break;
			return;
		case 'L':
			if (ends_with("ABLE")) break;
			if (ends_with("IBLE")) break;
			return;
		case 'N':
			if (ends_with("ANT")) break;
			if (ends_with("EMENT")) break;
			if (ends_with("MENT")) break;
			if (ends_with("ENT")) break;
			return;
		case 'O':
			if (ends_with("ION") && stemIndex >= 0
			&& (word[stemIndex] == 'S' || word[stemIndex] == 'T')) break;
			if (ends_with("OU")) break;
			return;
		case 'S':
			if (ends_with("ISM")) break;
			return;
		case 'T':
			if (ends_with("ATE")) break;
			if (ends_with("ITI")) break;
			return;
		case 'U':
			if (ends_with("OUS")) break;
			return;
		case 'V':
			if (ends_with("IVE")) break;
			return;
		case 'Z':
			if (ends_with("IZE")) break;
			return;
		default:
			return;
	}
	if (get_measure() > 1) {
		endIndex = stemIndex;
	}
}

void Stemmer::step5() {
	stemIndex = endIndex;

	if (word[endIndex] == 'E') {
		int m = get_measure();
		if (m > 1 || m == 1 && !is_cvc(endIndex - 1))
			endIndex--;
	}
	if (word[endIndex] == 'L' && has_double_consonants(endIndex) && get_measure() > 1)
		endIndex--;
}

string Stemmer::stem_word(string input) {
    word = new char[input.size()];
    strcpy(word, input.c_str());
    
    // We will not stem a word of 2 chars or an empty word
    if (invalid_word() || strlen(word) <= 2) return word;
    
    // Initialize the indexes of the first and last chars in the word
    stemIndex = 0;
    endIndex = strlen(word) - 1;
    
    step1();
    step2();
    step3();
    step4();
    step5();
    return string (word, 0, endIndex + 1);
}