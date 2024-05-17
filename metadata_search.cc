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

set<string> needed_words;

string highlight(string s) {
    string ret = "";
    string temp = "";

    // Filter words and check if it is matched
    for(std::string::size_type i = 0; i < s.size(); i++)
    {
        if( isalnum(s[i]) ){
            temp+=s[i];
        }
        else
        {   
            if(temp != "" && needed_words.find(temp)!=needed_words.end())
            {
                ret = ret + "===" + temp + "===";
            }
            else 
            {
                ret = ret + temp;
            }
            ret = ret + s[i];
            temp = "";
        }
    }
    
    return ret;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        cerr << "Usage: <index_name> <top-k> <keyword1> <keyword2> ..."  << endl;
        exit(0);
    }

    string index_name = argv[1];
    int topk = atoi(argv[2]);
    Xapian::Database db(index_name);

    //Building Query
    vector<string> or_terms, and_terms;
    for(int i = 3; i < argc; i++) {
        char sign = argv[i][0];
        string cur = argv[i];
        if(sign == '+') {
            cur = cur.substr(1, (int)cur.size());
            cout<<cur<<endl;
            and_terms.push_back(cur);
        }
        else {
            cout<<cur<<endl;
            or_terms.push_back(cur);
        }

        needed_words.insert(cur);
    }
    
    Xapian::Query or_query(
        Xapian::Query::OP_OR,
        or_terms.begin(),
        or_terms.end()
    );
    Xapian::Query and_query(
        Xapian::Query::OP_AND,
        and_terms.begin(),
        and_terms.end()
    );

    //Combine 2 query with an AND
    Xapian::Query query(
        Xapian::Query::OP_AND_MAYBE,
        and_query,
        or_query
    );	
    
    if(!or_terms.size()) query = and_query;
    else if(!and_terms.size()) query = or_query;

    auto start = chrono::steady_clock::now();

    //Start the query
    Xapian::Enquire enquire(db);
    enquire.set_query(query);
 
    Xapian::MSet matches = enquire.get_mset(0, topk); 
    printf("mset size is %d\n", matches.size());

    auto end = chrono::steady_clock::now();
    
    for(Xapian::MSetIterator match = matches.begin();  match != matches.end(); match ++) {
        Xapian::Document doc = match.get_document();
        cout << highlight(doc.get_value(0)) << endl << highlight(doc.get_value(1)) <<endl<<endl;
    }
    
    auto nanosec = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    double duration = nanosec*1.0/(1e6 * 1.0);
    cout << "TIME : "
        << duration
        << "milliseconds" << endl;
}
