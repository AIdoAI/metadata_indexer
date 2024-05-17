#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include<iterator>
#include <fstream>
#include <sstream>
#include <chrono>
using namespace std;

vector <string> all_words;

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Usage: <cardinality>"  << endl;
        exit(0);
    }
    int cardinality = atoi(argv[1]);

    // Scan the stopwords and put into a set to filter
    string line;
    ifstream file("all_words.txt");
    std::string word;
    while (file >> word)
    {
        all_words.push_back(word);
    }

    for(int i = 1; i <= 30; i++) {
        ofstream out_file;
        string tmp = argv[1];
        out_file.open(tmp + "_words_sets/" + to_string(i));
        for(int j = 1; j <= cardinality; j++) {
            int ran = rand() % all_words.size();
            out_file << all_words[ran] << endl;
        }
    }
}
