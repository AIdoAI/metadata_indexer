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

int main(int argc, char **argv) {
    if (argc != 4) {
        cerr << "Usage: <index_name> <top-k> <set_cardinality>"<< endl;
        exit(0);
    }

    string index_name = argv[1];
    int topk = atoi(argv[2]);
    int cardinality = atoi(argv[3]);
    long long duration_total = 0;
    Xapian::Database db(index_name);

    //Building Query
    for(int i = 1; i <= 30; i++) {
        string line;
        string infile_name = argv[3];
        infile_name = infile_name + "_words_sets/"  + to_string(i);
        ifstream infile(infile_name);
        vector<string> terms;

        while (getline(infile, line)) {
            //cout<<line<<endl;
            terms.push_back(line);
        }
    
        Xapian::Query query(
            Xapian::Query::OP_OR,
            terms.begin(),
            terms.end()
        );

        auto start = chrono::steady_clock::now();
        //Start the query
        Xapian::Enquire enquire(db);
        enquire.set_query(query);
        Xapian::MSet matches = enquire.get_mset(0, topk); 
        auto end = chrono::steady_clock::now();

        printf("mset size is %d\n", matches.size());
        
        cout << "TIME : "
            << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
            << "nanoseconds" << endl;
        
        auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        //cout<<duration<<endl;
        duration_total += duration;
    }
    double average = duration_total*1.0/(30.0*1e6);
    cout<<"Average time is: "<<average<<" millisecond"<<endl;
}
