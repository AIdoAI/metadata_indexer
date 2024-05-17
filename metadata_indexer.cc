#include<xapian.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>
#include<iterator>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;

set<string> stopwords;


vector<string> tokenize(string s) {
    vector<string> tokens;
    string temp = "";
    
    // Filter special characters as well as stopwords
    for(std::string::size_type i = 0; i < s.size(); i++)
    {
        if(isalnum(s[i])){
            temp+=s[i];
        }
        else
        {   
            if(temp != "" && stopwords.find(temp)==stopwords.end())
            {
                tokens.push_back(temp);
            }
            temp = "";
        }
    }
    if (temp != "" && stopwords.find(temp)==stopwords.end()){
        tokens.push_back(temp);
    }
    return tokens;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        cerr << "Usage: <dataset_metadata> <index_name>"  << endl;
        exit(0);
    }
    char *dataset = argv[1];
    char *index_name = argv[2];

    // Scan the stopwords and put into a set to filter
    string line;
    ifstream stopwords_file("stopwords.txt");
    while (getline(stopwords_file, line)) {
        // cout << stopword << endl;
        stopwords.insert(line);
    }
    auto start = chrono::steady_clock::now();
    // Create or open the database
    Xapian::WritableDatabase db(index_name, Xapian::DB_CREATE_OR_OPEN);
    Xapian::Document doc;
 
    // Read the dataset
    vector<string> input;
    ifstream infile(dataset);
    while (getline(infile, line)){
        input.push_back(line);
    }
 
    int lineno = 0;
    for(int i = 0; i < input.size(); i+=2) {
        // cout << "dataset " << lineno << ": ";
        string name = input[i];
        string data = input[i+1];
 
        // perform tokenization on name + data to return a vector of terms for the whole document
        vector<string> tokens = tokenize(name + " " + data);
 
        // populate the document
        // value[0] will be the original line
        doc.clear_terms();
        doc.clear_values();

        // Document will have 2 values, one for the name and one for the data
        doc.add_value(0, name);
        doc.add_value(1, data);

        // a document is a bag of terms
        for(vector<string>::iterator it_token=tokens.begin(); 
            it_token != tokens.end(); 
            it_token++) {
            string &token = *it_token;
            // cout << token << " ";
            doc.add_term(token);
        }
        // cout << endl;
        db.add_document(doc);
        lineno += 1;
        if(lineno % 1000 == 0) {
            db.commit();
        }
    }
    db.commit();
    auto end = chrono::steady_clock::now();
    
    std::cout << "TIME : "
        << chrono::duration_cast<chrono::milliseconds>(end - start).count()
        << "milliseconds" << endl;
}
